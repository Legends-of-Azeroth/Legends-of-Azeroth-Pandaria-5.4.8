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

#include "AchievementMgr.h"
#include "AreaTrigger.h"
#include "Chat.h"
#include "Common.h"
#include "DatabaseEnv.h"
#include "DB2Structure.h"
#include "DB2Stores.h"
#include "DisableMgr.h"
#include "GameEventMgr.h"
#include "GossipDef.h"
#include "GroupMgr.h"
#include "GuildMgr.h"
#include "InstanceSaveMgr.h"
#include "Language.h"
#include "LFGMgr.h"
#include "Log.h"
#include "MapManager.h"
#include "ObjectGuid.h"
#include "ObjectMgr.h"
#include "Pet.h"
#include "PoolMgr.h"
#include "ReputationMgr.h"
#include "ScriptMgr.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "Transport.h"
#include "UpdateMask.h"
#include "Util.h"
#include "Vehicle.h"
#include "WaypointManager.h"
#include "World.h"
#include "Util.h"
#include "DBCStructure.h"

ScriptMapMap sSpellScripts;
ScriptMapMap sEventScripts;
ScriptMapMap sWaypointScripts;

std::string GetScriptsTableNameByType(ScriptsType type)
{
    std::string res = "";
    switch (type)
    {
        case SCRIPTS_SPELL:         res = "spell_scripts";      break;
        case SCRIPTS_EVENT:         res = "event_scripts";      break;
        case SCRIPTS_WAYPOINT:      res = "waypoint_scripts";   break;
        default: break;
    }
    return res;
}

ScriptMapMap* GetScriptsMapByType(ScriptsType type)
{
    ScriptMapMap* res = NULL;
    switch (type)
    {
        case SCRIPTS_SPELL:         res = &sSpellScripts;       break;
        case SCRIPTS_EVENT:         res = &sEventScripts;       break;
        case SCRIPTS_WAYPOINT:      res = &sWaypointScripts;    break;
        default: break;
    }
    return res;
}

std::string GetScriptCommandName(ScriptCommands command)
{
    std::string res = "";
    switch (command)
    {
        case SCRIPT_COMMAND_TALK: res = "SCRIPT_COMMAND_TALK"; break;
        case SCRIPT_COMMAND_EMOTE: res = "SCRIPT_COMMAND_EMOTE"; break;
        case SCRIPT_COMMAND_FIELD_SET: res = "SCRIPT_COMMAND_FIELD_SET"; break;
        case SCRIPT_COMMAND_MOVE_TO: res = "SCRIPT_COMMAND_MOVE_TO"; break;
        case SCRIPT_COMMAND_FLAG_SET: res = "SCRIPT_COMMAND_FLAG_SET"; break;
        case SCRIPT_COMMAND_FLAG_REMOVE: res = "SCRIPT_COMMAND_FLAG_REMOVE"; break;
        case SCRIPT_COMMAND_TELEPORT_TO: res = "SCRIPT_COMMAND_TELEPORT_TO"; break;
        case SCRIPT_COMMAND_QUEST_EXPLORED: res = "SCRIPT_COMMAND_QUEST_EXPLORED"; break;
        case SCRIPT_COMMAND_KILL_CREDIT: res = "SCRIPT_COMMAND_KILL_CREDIT"; break;
        case SCRIPT_COMMAND_RESPAWN_GAMEOBJECT: res = "SCRIPT_COMMAND_RESPAWN_GAMEOBJECT"; break;
        case SCRIPT_COMMAND_TEMP_SUMMON_CREATURE: res = "SCRIPT_COMMAND_TEMP_SUMMON_CREATURE"; break;
        case SCRIPT_COMMAND_OPEN_DOOR: res = "SCRIPT_COMMAND_OPEN_DOOR"; break;
        case SCRIPT_COMMAND_CLOSE_DOOR: res = "SCRIPT_COMMAND_CLOSE_DOOR"; break;
        case SCRIPT_COMMAND_ACTIVATE_OBJECT: res = "SCRIPT_COMMAND_ACTIVATE_OBJECT"; break;
        case SCRIPT_COMMAND_REMOVE_AURA: res = "SCRIPT_COMMAND_REMOVE_AURA"; break;
        case SCRIPT_COMMAND_CAST_SPELL: res = "SCRIPT_COMMAND_CAST_SPELL"; break;
        case SCRIPT_COMMAND_PLAY_SOUND: res = "SCRIPT_COMMAND_PLAY_SOUND"; break;
        case SCRIPT_COMMAND_CREATE_ITEM: res = "SCRIPT_COMMAND_CREATE_ITEM"; break;
        case SCRIPT_COMMAND_DESPAWN_SELF: res = "SCRIPT_COMMAND_DESPAWN_SELF"; break;
        case SCRIPT_COMMAND_LOAD_PATH: res = "SCRIPT_COMMAND_LOAD_PATH"; break;
        case SCRIPT_COMMAND_CALLSCRIPT_TO_UNIT: res = "SCRIPT_COMMAND_CALLSCRIPT_TO_UNIT"; break;
        case SCRIPT_COMMAND_KILL: res = "SCRIPT_COMMAND_KILL"; break;
        // TrinityCore only
        case SCRIPT_COMMAND_ORIENTATION: res = "SCRIPT_COMMAND_ORIENTATION"; break;
        case SCRIPT_COMMAND_EQUIP: res = "SCRIPT_COMMAND_EQUIP"; break;
        case SCRIPT_COMMAND_MODEL: res = "SCRIPT_COMMAND_MODEL"; break;
        case SCRIPT_COMMAND_CLOSE_GOSSIP: res = "SCRIPT_COMMAND_CLOSE_GOSSIP"; break;
        case SCRIPT_COMMAND_PLAYMOVIE: res = "SCRIPT_COMMAND_PLAYMOVIE"; break;
        default:
        {
            char sz[32];
            sprintf(sz, "Unknown command: %d", command);
            res = sz;
            break;
        }
    }
    return res;
}

std::string ScriptInfo::GetDebugInfo() const
{
    char sz[256];
    sprintf(sz, "%s ('%s' script id: %u)", GetScriptCommandName(command).c_str(), GetScriptsTableNameByType(type).c_str(), id);
    return std::string(sz);
}

bool normalizePlayerName(std::string& name)
{
    if (name.empty())
        return false;

    // Remove realmname for now
    name = name.substr(0, name.find("-"));

    std::wstring wstr;
    if (!Utf8toWStr(name, wstr))
        return false;

    if (wstr.length() > MAX_INTERNAL_PLAYER_NAME)
        return false;

    wstr[0] = wcharToUpper(wstr[0]);
    for (size_t i = 1; i < wstr.size(); ++i)
        wstr[i] = wcharToLower(wstr[i]);

    if (!WStrToUtf8(wstr, name))
        return false;

    return true;
}

LanguageDesc lang_description[LANGUAGES_COUNT] =
{
    { LANG_ADDON,                  0, 0                            },
    { LANG_UNIVERSAL,              0, 0                            },
    { LANG_ORCISH,               669, SKILL_LANG_ORCISH            },
    { LANG_DARNASSIAN,           671, SKILL_LANG_DARNASSIAN        },
    { LANG_TAURAHE,              670, SKILL_LANG_TAURAHE           },
    { LANG_DWARVISH,             672, SKILL_LANG_DWARVEN           },
    { LANG_COMMON,               668, SKILL_LANG_COMMON            },
    { LANG_DEMONIC,              815, SKILL_LANG_DEMON_TONGUE      },
    { LANG_TITAN,                816, SKILL_LANG_TITAN             },
    { LANG_THALASSIAN,           813, SKILL_LANG_THALASSIAN        },
    { LANG_DRACONIC,             814, SKILL_LANG_DRACONIC          },
    { LANG_KALIMAG,              817, SKILL_LANG_OLD_TONGUE        },
    { LANG_GNOMISH,             7340, SKILL_LANG_GNOMISH           },
    { LANG_TROLL,               7341, SKILL_LANG_TROLL             },
    { LANG_GUTTERSPEAK,        17737, SKILL_LANG_GUTTERSPEAK       },
    { LANG_DRAENEI,            29932, SKILL_LANG_DRAENEI           },
    { LANG_ZOMBIE,                 0, 0                            },
    { LANG_GNOMISH_BINARY,         0, 0                            },
    { LANG_GOBLIN_BINARY,          0, 0                            },
    { LANG_WORGEN,             69270, SKILL_LANG_WORGEN            },
    { LANG_GOBLIN,             69269, SKILL_LANG_GOBLIN            },
    { LANG_PANDAREN_NEUTRAL,  108127, SKILL_LANG_PANDAREN_NEUTRAL  },
    { LANG_PANDAREN_ALLIANCE, 108130, SKILL_LANG_PANDAREN_ALLIANCE },
    { LANG_PANDAREN_HORDE,    108131, SKILL_LANG_PANDAREN_HORDE    }
};

LanguageDesc const* GetLanguageDescByID(uint32 lang)
{
    for (uint8 i = 0; i < LANGUAGES_COUNT; ++i)
    {
        if (uint32(lang_description[i].lang_id) == lang)
            return &lang_description[i];
    }

    return NULL;
}

bool SpellClickInfo::IsFitToRequirements(Unit const* clicker, Unit const* clickee) const
{
    Player const* playerClicker = clicker->ToPlayer();
    if (!playerClicker)
        return true;

    Unit const* summoner = nullptr;
    // Check summoners for party
    if (clickee->IsSummon())
        summoner = clickee->ToTempSummon()->GetSummoner();
    if (!summoner)
        summoner = clickee;

    // This only applies to players
    switch (userType)
    {
        case SPELL_CLICK_USER_FRIEND:
            if (!playerClicker->IsFriendlyTo(summoner))
                return false;
            break;
        case SPELL_CLICK_USER_RAID:
            if (!playerClicker->IsInRaidWith(summoner))
                return false;
            break;
        case SPELL_CLICK_USER_PARTY:
            if (!playerClicker->IsInPartyWith(summoner))
                return false;
            break;
        default:
            break;
    }

    return true;
}

ObjectMgr::ObjectMgr()
{
    for (uint8 i = 0; i < MAX_CLASSES; ++i)
        for (uint8 j = 0; j < MAX_RACES; ++j)
            _playerInfo[j][i] = NULL;
}

ObjectMgr* ObjectMgr::instance()
{
    static ObjectMgr instance;
    return &instance;
}

ObjectMgr::~ObjectMgr()
{
    for (QuestMap::iterator i = _questTemplates.begin(); i != _questTemplates.end(); ++i)
        delete i->second;

    for (PetLevelInfoContainer::iterator i = _petInfoStore.begin(); i != _petInfoStore.end(); ++i)
        delete [] i->second;

    for (int race = 0; race < MAX_RACES; ++race)
    {
        for (int class_ = 0; class_ < MAX_CLASSES; ++class_)
        {
            if (_playerInfo[race][class_])
                delete [] _playerInfo[race][class_]->levelInfo;
            delete _playerInfo[race][class_];
        }
    }

    for (CacheVendorItemContainer::iterator itr = _cacheVendorItemStore.begin(); itr != _cacheVendorItemStore.end(); ++itr)
        itr->second.Clear();

    _cacheTrainerSpellStore.clear();
    _graveyardOrientations.clear();

    for (DungeonEncounterContainer::iterator itr = _dungeonEncounterStore.begin(); itr != _dungeonEncounterStore.end(); ++itr)
        for (DungeonEncounterList::iterator encounterItr = itr->second.begin(); encounterItr != itr->second.end(); ++encounterItr)
            delete *encounterItr;

    for (AccessRequirementContainer::iterator itr = _accessRequirementStore.begin(); itr != _accessRequirementStore.end(); ++itr)
        delete itr->second;
}

void ObjectMgr::AddLocaleString(std::string const& s, LocaleConstant locale, StringVector& data)
{
    if (!s.empty())
    {
        if (data.size() <= size_t(locale))
            data.resize(locale + 1);

        data[locale] = s;
    }
}

void ObjectMgr::LoadGraveyardOrientations()
{
    uint32 oldMSTime = getMSTime();

    _graveyardOrientations.clear();

    QueryResult result = WorldDatabase.Query("SELECT id, orientation FROM graveyard_orientation");

    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();

        uint32 id = fields[0].GetUInt32();
        if (!sWorldSafeLocsStore.LookupEntry(id))
        {
            TC_LOG_ERROR("server.loading", "Graveyard %u referenced in graveyard_orientation doesn't exist.", id);
            continue;
        }
        _graveyardOrientations[id] = fields[1].GetFloat();

    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %lu graveyard orientations in %u ms", (unsigned long)_graveyardOrientations.size(), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadCreatureLocales()
{
    uint32 oldMSTime = getMSTime();

    _creatureLocaleStore.clear(); // need for reload case

    //                                               0      1       2     3           4
    QueryResult result = WorldDatabase.Query("SELECT entry, locale, Name, FemaleName, Title FROM creature_template_locale");
    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();

        uint32 id               = fields[0].GetUInt32();
        std::string localeName  = fields[1].GetString();

        std::string name        = fields[2].GetString();
        std::string femaleName  = fields[3].GetString();
        std::string title       = fields[4].GetString();

        CreatureLocale& data = _creatureLocaleStore[id];
        LocaleConstant locale = GetLocaleByName(localeName);
        if (!sWorld->getBoolConfig(CONFIG_LOAD_LOCALES) || locale == LOCALE_enUS)
            continue;

        AddLocaleString(name,       locale, data.Name);
        AddLocaleString(femaleName, locale, data.FemaleName);
        AddLocaleString(title,      locale, data.Title);

    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u creature locale strings in %u ms", uint32(_creatureLocaleStore.size()), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadGossipMenuItemsLocales()
{
    uint32 oldMSTime = getMSTime();

    _gossipMenuItemsLocaleStore.clear();                              // need for reload case

    //                                               0       1            2       3           4
    QueryResult result = WorldDatabase.Query("SELECT MenuID, OptionID, Locale, OptionText, BoxText FROM gossip_menu_option_locale");

    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();

        uint32 menuId           = fields[0].GetUInt32();
        uint32 optionId         = fields[1].GetUInt32();
        std::string localeName  = fields[2].GetString();

        LocaleConstant locale = GetLocaleByName(localeName);
        if (!sWorld->getBoolConfig(CONFIG_LOAD_LOCALES) || locale == LOCALE_enUS)
            continue;

        GossipMenuItemsLocale& data = _gossipMenuItemsLocaleStore[std::make_pair(menuId, optionId)];
        AddLocaleString(fields[3].GetString(), locale, data.OptionText);
        AddLocaleString(fields[4].GetString(), locale, data.BoxText);
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %lu gossip_menu_option locale strings in %u ms", (unsigned long)_gossipMenuItemsLocaleStore.size(), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadPointOfInterestLocales()
{
    uint32 oldMSTime = getMSTime();

    _pointOfInterestLocaleStore.clear();                              // need for reload case

    //                                               0   1       2
    QueryResult result = WorldDatabase.Query("SELECT ID, locale, Name FROM points_of_interest_locale");

    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();

        uint32 id               = fields[0].GetUInt32();
        std::string localeName  = fields[1].GetString();

        LocaleConstant locale = GetLocaleByName(localeName);
        if (!sWorld->getBoolConfig(CONFIG_LOAD_LOCALES) || locale == LOCALE_enUS)
            continue;

        PointOfInterestLocale& data = _pointOfInterestLocaleStore[id];
        AddLocaleString(fields[2].GetString(), locale, data.Name);
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u points_of_interest locale strings in %u ms", uint32(_pointOfInterestLocaleStore.size()), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadCreatureTemplates()
{
    uint32 oldMSTime = getMSTime();

    //                                                 0              1                 2                  3                 4                   5                  6             7         
    QueryResult result = WorldDatabase.Query("SELECT entry, difficulty_entry_1, difficulty_entry_2, difficulty_entry_3, difficulty_entry_4, difficulty_entry_5, KillCredit1, KillCredit2,  "
    //                                           8       9         10        11        12             13        14      15     16      17       18        19         20             
                                             "name, femaleName, subname, IconName, gossip_menu_id, minlevel, maxlevel, exp, exp_unk, faction, npcflag, npcflag2, speed_walk, "
    //                                            21       22     23      24      25       26          27             28              29              30             31          32           33
                                             "speed_run, scale, `rank`, mindmg, maxdmg, dmgschool, attackpower, dmg_multiplier, BaseAttackTime, RangeAttackTime, unit_class, unit_flags, unit_flags2, "
    //                                             34         35         36             37            38           39           40              41           42
                                             "dynamicflags, family, trainer_type, trainer_class, trainer_race, minrangedmg, maxrangedmg, rangedattackpower, type, "
    //                                            43           44        45         46            47          48           49          50           51           52           53
                                             "type_flags, type_flags2, lootid, pickpocketloot, skinloot, resistance1, resistance2, resistance3, resistance4, resistance5, resistance6, "
    //                                          54      55      56      57      58      59      60      61          62           63        64       65       66         67
                                             "spell1, spell2, spell3, spell4, spell5, spell6, spell7, spell8, PetSpellDataId, VehicleId, mingold, maxgold, AIName, MovementType, "
    //                                            68          69        70          71         72          73                74
                                             "ctm.Ground, ctm.Swim, ctm.Flight, ctm.Rooted, ctm.Chase, ctm.Random, ctm.InteractionPauseTimer,"
    //                                            75           76         77           78           79          80            81          82          83          84          85
                                             "HoverHeight, Health_mod, Mana_mod, Mana_mod_extra, Armor_mod, RacialLeader, questItem1, questItem2, questItem3, questItem4, questItem5, "
    //                                            86           87          88           89            90                   91               92          93
                                             " questItem6, movementId, RegenHealth, VignetteID, TrackingQuestID,  mechanic_immune_mask, flags_extra, ScriptName "
                                             "FROM creature_template ct LEFT JOIN creature_template_movement ctm ON ct.entry = ctm.CreatureId;");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 creature template definitions. DB table `creature_template` is empty.");
        return;
    }

    _creatureTemplateStore.rehash(result->GetRowCount());
    do
    {
        Field* fields = result->Fetch();
        LoadCreatureTemplate(fields);
    }
    while (result->NextRow());

    // We load the creature models after loading but before checking
    LoadCreatureTemplateModels();

    // Checking needs to be done after loading because of the difficulty self referencing
    for (CreatureTemplateContainer::const_iterator itr = _creatureTemplateStore.begin(); itr != _creatureTemplateStore.end(); ++itr)
        CheckCreatureTemplate(&itr->second);

    TC_LOG_INFO("server.loading", ">> Loaded %u creature definitions in %u ms", uint32(_creatureTemplateStore.size()), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadCreatureTemplate(Field* fields)
{
        uint32 entry = fields[0].GetUInt32();

        CreatureTemplate& creatureTemplate = _creatureTemplateStore[entry];

        creatureTemplate.Entry = entry;

        for (uint8 i = 0; i < MAX_TEMPLATE_DIFFICULTY - 1; ++i)
            creatureTemplate.DifficultyEntry[i] = fields[1 + i].GetUInt32();

        for (uint8 i = 0; i < MAX_KILL_CREDIT; ++i)
            creatureTemplate.KillCredit[i] = fields[6 + i].GetUInt32();

        creatureTemplate.Name              = fields[8].GetString();
        creatureTemplate.FemaleName        = fields[9].GetString();
        creatureTemplate.SubName           = fields[10].GetString();
        creatureTemplate.IconName          = fields[11].GetString();
        creatureTemplate.GossipMenuId      = fields[12].GetUInt32();
        creatureTemplate.minlevel          = fields[13].GetInt16();
        creatureTemplate.maxlevel          = fields[14].GetInt16();
        creatureTemplate.expansion         = uint32(fields[15].GetInt16());
        creatureTemplate.expansionUnknown  = uint32(fields[16].GetUInt16());
        creatureTemplate.faction         = uint32(fields[17].GetUInt16());
        creatureTemplate.npcflag           = fields[18].GetUInt32();
        creatureTemplate.npcflag2          = fields[19].GetUInt32();
        creatureTemplate.speed_walk        = fields[20].GetFloat();
        creatureTemplate.speed_run         = fields[21].GetFloat();
        creatureTemplate.scale             = fields[22].GetFloat();
        creatureTemplate.rank              = uint32(fields[23].GetUInt8());
        creatureTemplate.mindmg            = fields[24].GetFloat();
        creatureTemplate.maxdmg            = fields[25].GetFloat();
        creatureTemplate.dmgschool         = uint32(fields[26].GetInt8());
        creatureTemplate.attackpower       = fields[27].GetUInt32();
        creatureTemplate.dmg_multiplier    = fields[28].GetFloat();
        creatureTemplate.BaseAttackTime    = fields[29].GetUInt32();
        creatureTemplate.RangeAttackTime   = fields[30].GetUInt32();
        creatureTemplate.unit_class        = uint32(fields[31].GetUInt8());
        creatureTemplate.unit_flags        = fields[32].GetUInt32();
        creatureTemplate.unit_flags2       = fields[33].GetUInt32();
        creatureTemplate.dynamicflags      = fields[34].GetUInt32();
        creatureTemplate.family            = uint32(fields[35].GetUInt8());
        creatureTemplate.trainer_type      = uint32(fields[36].GetUInt8());
        creatureTemplate.trainer_class     = uint32(fields[37].GetUInt8());
        creatureTemplate.trainer_race      = uint32(fields[38].GetUInt8());
        creatureTemplate.minrangedmg       = fields[39].GetFloat();
        creatureTemplate.maxrangedmg       = fields[40].GetFloat();
        creatureTemplate.rangedattackpower = uint32(fields[41].GetUInt16());
        creatureTemplate.type              = uint32(fields[42].GetUInt8());
        creatureTemplate.type_flags        = fields[43].GetUInt32();
        creatureTemplate.type_flags2       = fields[44].GetUInt32();
        creatureTemplate.lootid            = fields[45].GetUInt32();
        creatureTemplate.pickpocketLootId  = fields[46].GetUInt32();
        creatureTemplate.SkinLootId        = fields[47].GetUInt32();

        for (uint8 i = SPELL_SCHOOL_HOLY; i < MAX_SPELL_SCHOOL; ++i)
            creatureTemplate.resistance[i] = fields[48 + i - 1].GetInt16();

        for (uint8 i = 0; i < CREATURE_MAX_SPELLS; ++i)
            creatureTemplate.spells[i] = fields[54 + i].GetUInt32();

        creatureTemplate.PetSpellDataId = fields[62].GetUInt32();
        creatureTemplate.VehicleId      = fields[63].GetUInt32();
        creatureTemplate.mingold        = fields[64].GetUInt32();
        creatureTemplate.maxgold        = fields[65].GetUInt32();
        creatureTemplate.AIName         = fields[66].GetString();
        creatureTemplate.MovementType   = uint32(fields[67].GetUInt8());
        if (!fields[68].IsNull())
            creatureTemplate.Movement.Ground = static_cast<CreatureGroundMovementType>(fields[68].GetUInt8());

        if (!fields[69].IsNull())
            creatureTemplate.Movement.Swim = fields[69].GetBool();

        if (!fields[70].IsNull())
            creatureTemplate.Movement.Flight = static_cast<CreatureFlightMovementType>(fields[70].GetUInt8());

        if (!fields[71].IsNull())
            creatureTemplate.Movement.Rooted = fields[71].GetBool();

        if (!fields[72].IsNull())
            creatureTemplate.Movement.Chase = static_cast<CreatureChaseMovementType>(fields[72].GetUInt8());

        if (!fields[73].IsNull())
            creatureTemplate.Movement.Random = static_cast<CreatureRandomMovementType>(fields[73].GetUInt8());

        if (!fields[74].IsNull())
            creatureTemplate.Movement.InteractionPauseTimer = fields[74].GetUInt32();

        creatureTemplate.HoverHeight    = fields[75].GetFloat();
        creatureTemplate.ModHealth      = fields[76].GetFloat();
        creatureTemplate.ModMana        = fields[77].GetFloat();
        creatureTemplate.ModManaExtra   = fields[78].GetFloat();
        creatureTemplate.ModArmor       = fields[79].GetFloat();
        creatureTemplate.RacialLeader   = fields[80].GetBool();

        for (uint8 i = 0; i < MAX_CREATURE_QUEST_ITEMS; ++i)
            creatureTemplate.questItems[i] = fields[81 + i].GetUInt32();

        creatureTemplate.movementId         = fields[87].GetUInt32();
        creatureTemplate.RegenHealth        = fields[88].GetBool();
        creatureTemplate.VignetteID         = fields[89].GetUInt32();
        creatureTemplate.TrackingQuestID    = fields[90].GetUInt32();
        creatureTemplate.MechanicImmuneMask = fields[91].GetUInt32();
        creatureTemplate.flags_extra        = fields[92].GetUInt32();
        creatureTemplate.ScriptID           = GetScriptId(fields[93].GetString());
}

void ObjectMgr::LoadCreatureTemplateModels()
{
    uint32 oldMSTime = getMSTime();

    //                                               0           1                  2             3
    QueryResult result = WorldDatabase.Query("SELECT CreatureID, CreatureDisplayID, DisplayScale, Probability FROM creature_template_model ORDER BY Idx ASC");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 creature template model definitions. DB table `creature_template_model` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 creatureId = fields[0].GetUInt32();
        uint32 creatureDisplayId = fields[1].GetUInt32();
        float displayScale = fields[2].GetFloat();
        float probability = fields[3].GetFloat();

        CreatureTemplate const* cInfo = GetCreatureTemplate(creatureId);
        if (!cInfo)
        {
            TC_LOG_ERROR("sql.sql", "Creature template (Entry: %u) does not exist but has a record in `creature_template_model`", creatureId);
            continue;
        }

        CreatureDisplayInfoEntry const* displayEntry = sCreatureDisplayInfoStore.LookupEntry(creatureDisplayId);
        if (!displayEntry)
        {
            TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) lists non-existing CreatureDisplayID id (%u), this can crash the client.", creatureId, creatureDisplayId);
            continue;
        }

        CreatureModelInfo const* modelInfo = GetCreatureModelInfo(creatureDisplayId);
        if (!modelInfo)
            TC_LOG_ERROR("sql.sql", "No model data exist for `CreatureDisplayID` = %u listed by creature (Entry: %u).", creatureDisplayId, creatureId);

        if (displayScale <= 0.0f)
            displayScale = 1.0f;

        const_cast<CreatureTemplate*>(cInfo)->Models.emplace_back(creatureDisplayId, displayScale, probability);

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u creature template models in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

static bool CheckCreatureAddonSpell(uint32 entry, SpellInfo const* spellInfo)
{
    for (auto&& eff : spellInfo->Effects)
    {
        if (eff.Effect == SPELL_EFFECT_SUMMON && eff.MiscValue == entry)
            return false;

        if (eff.Effect == SPELL_EFFECT_TRIGGER_SPELL ||
            eff.Effect == SPELL_EFFECT_TRIGGER_SPELL_2 ||
            eff.Effect == SPELL_EFFECT_TRIGGER_SPELL_WITH_VALUE)
            if (eff.TriggerSpell)
                if (SpellInfo const* trigger = sSpellMgr->GetSpellInfo(eff.TriggerSpell))
                    if (!CheckCreatureAddonSpell(entry, trigger))
                        return false;
    }
    return true;
}


void ObjectMgr::LoadCreatureTemplateAddons()
{
    uint32 oldMSTime = getMSTime();

    //                                                 0       1       2           3       4          5         6           7        8        9             10                   11        12                          
    QueryResult result = WorldDatabase.Query("SELECT entry, path_id, mount, StandState, AnimTier, VisFlags, SheathState, PvPFlags, emote, aiAnimKit, movementAnimKit, meleeAnimKit, auras FROM creature_template_addon");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 creature template addon definitions. DB table `creature_template_addon` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        if (!sObjectMgr->GetCreatureTemplate(entry))
        {
            TC_LOG_ERROR("sql.sql", "Creature template (Entry: %u) does not exist but has a record in `creature_template_addon`", entry);
            continue;
        }

        CreatureAddon& creatureAddon = _creatureTemplateAddonStore[entry];

        creatureAddon.path_id           = fields[1].GetUInt32();
        creatureAddon.mount             = fields[2].GetUInt32();
        creatureAddon.standState        = fields[3].GetUInt8();
        creatureAddon.animTier          = fields[4].GetUInt8();
        creatureAddon.visFlags          = fields[5].GetUInt8();
        creatureAddon.sheathState       = fields[6].GetUInt8();
        creatureAddon.pvpFlags          = fields[7].GetUInt8();
        creatureAddon.emote             = fields[8].GetUInt32();
        creatureAddon.aiAnimKit         = fields[9].GetUInt16();
        creatureAddon.movementAnimKit   = fields[10].GetUInt16();
        creatureAddon.meleeAnimKit      = fields[11].GetUInt16();

        Tokenizer tokens(fields[12].GetString(), ' ');
        uint8 i = 0;
        creatureAddon.auras.resize(tokens.size());
        for (Tokenizer::const_iterator itr = tokens.begin(); itr != tokens.end(); ++itr)
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(uint32(atol(*itr)));
            if (!spellInfo)
            {
                TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has wrong spell %u defined in `auras` field in `creature_template_addon`.", entry, uint32(atol(*itr)));
                continue;
            }

            if (spellInfo->HasAura(SPELL_AURA_CONTROL_VEHICLE))
                TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has SPELL_AURA_CONTROL_VEHICLE aura %u defined in `auras` field in `creature_template_addon`.", entry, spellInfo->Id);

            if (!CheckCreatureAddonSpell(entry, spellInfo))
            {
                TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has spell %u defined in `auras` field in `creature_addon` which summon this creature", entry, spellInfo->Id);
                continue;
            }

            creatureAddon.auras[i++] = spellInfo->Id;
        }

        if (creatureAddon.mount)
        {
            if (!sCreatureDisplayInfoStore.LookupEntry(creatureAddon.mount))
            {
                TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has invalid displayInfoId (%u) for mount defined in `creature_template_addon`", entry, creatureAddon.mount);
                creatureAddon.mount = 0;
            }
        }

        if (EmotesEntry const* emote = sEmotesStore.LookupEntry(creatureAddon.emote))
        {
            if (emote->Id != 0 && emote->EmoteType == 0)
            {
                TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) cant`t has oneshot emote %d in `creature_template_addon` field, you should use scripts, skipped.", entry, creatureAddon.emote);
                creatureAddon.emote = 0;
            }
        }
        else
        {
            TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has invalid emote (%u) defined in `creature_template_addon`.", entry, creatureAddon.emote);
            creatureAddon.emote = 0;
        }

        if (creatureAddon.aiAnimKit && !sAnimKitStore.LookupEntry(creatureAddon.aiAnimKit))
        {
            TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has invalid aiAnimKit (%u) defined in `creature_template_addon`.", entry, creatureAddon.aiAnimKit);
            creatureAddon.aiAnimKit = 0;
        }

        if (creatureAddon.movementAnimKit && !sAnimKitStore.LookupEntry(creatureAddon.movementAnimKit))
        {
            TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has invalid movementAnimKit (%u) defined in `creature_template_addon`.", entry, creatureAddon.movementAnimKit);
            creatureAddon.movementAnimKit = 0;
        }

        if (creatureAddon.meleeAnimKit && !sAnimKitStore.LookupEntry(creatureAddon.meleeAnimKit))
        {
            TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has invalid meleeAnimKit (%u) defined in `creature_template_addon`.", entry, creatureAddon.meleeAnimKit);
            creatureAddon.meleeAnimKit = 0;
        }

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u creature template addons in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadCreatureDifficultyModifiers()
{
    _creatureDifficulty.clear();
    _creatureDifficulty.resize(MAX_CREATURE_DIFFICULTY);

    //                                               0        1            2           3            4         5          6          7       8         9                  10           11
    QueryResult result = WorldDatabase.Query("SELECT id, difficulty+0, damage_mod, health_mod, level_min, level_max, attackpower, mindmg, maxdmg, rangedattackpower, minrangedmg, maxrangedmg  FROM creature_difficulty");
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 creature difficulty modifiers. DB table `creature_difficulty_modifier` is empty.");
        return;
    }

    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        uint32 creature = fields[0].GetUInt32();
        uint8 difficulty = fields[1].GetUInt8();
        if (difficulty >= MAX_CREATURE_DIFFICULTY)
        {
            TC_LOG_ERROR("sql.sql", "ObjectMgr::LoadCreatureDifficultyModifiers - Invalid difficulty (%u) for creature %u", difficulty, creature);
            continue;
        }

        if (!GetCreatureTemplate(creature))
        {
            TC_LOG_ERROR("sql.sql", "ObjectMgr::LoadCreatureDifficultyModifiers - Creature entry not found in DB: %u", difficulty);
            continue;
        }
        CreatureDifficultyInfo info;
        info.DamageMod         = fields[2].GetFloat();
        info.HealthMod         = fields[3].GetFloat();
        info.LevelMin          = fields[4].GetUInt8();
        info.LevelMax          = fields[5].GetUInt8();
        info.AttackPower       = fields[6].GetUInt32();
        info.MinDamage         = fields[7].GetFloat();
        info.MaxDamage         = fields[8].GetFloat();
        info.RangedAttackPower = fields[9].GetFloat();
        info.MinRangeDamage    = fields[10].GetFloat();
        info.MaxRangeDamage    = fields[11].GetFloat();
        ++count;

        _creatureDifficulty[difficulty].insert({ creature, info });
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u creature difficulty modifiers in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadCreatureSparringTemplate()
{
    uint32 oldMSTime = getMSTime();

    //                                                   0               1
    QueryResult result = WorldDatabase.Query("SELECT creature_id, health_limit_pct FROM creature_sparring_template");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 creature template sparring definitions. DB table `creature_sparring_template` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();
        float healthPct = fields[1].GetFloat();

        if (!sObjectMgr->GetCreatureTemplate(entry))
        {
            TC_LOG_ERROR("sql.sql", "Creature template (Entry: %u) does not exist but has a record in `creature_sparring_template`", entry);
            continue;
        }

        if (healthPct > 100.0f)
        {
            TC_LOG_ERROR("sql.sql", "Sparring entry (Entry: %u) exceeds the health percentage limit. Setting to 100.", entry);
            healthPct = 100.0f;
        }

        if (healthPct <= 0.0f)
        {
            TC_LOG_ERROR("sql.sql", "Sparring entry (Entry: %u) has a negative or too small health percentage. Setting to 0.1.", entry);
            healthPct = 0.1f;
        }

        _creatureSparringTemplateStore[entry] = healthPct;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u creature sparring templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::CheckCreatureTemplate(CreatureTemplate const* cInfo)
{
    if (!cInfo)
        return;

    bool ok = true;                                     // bool to allow continue outside this loop
    for (uint32 diff = 0; diff < MAX_TEMPLATE_DIFFICULTY - 1 && ok; ++diff)
    {
        if (!cInfo->DifficultyEntry[diff])
            continue;

        ok = false;                                     // will be set to true at the end of this loop again

        CreatureTemplate const* difficultyInfo = GetCreatureTemplate(cInfo->DifficultyEntry[diff]);
        if (!difficultyInfo)
        {
            TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has `difficulty_entry_%u`=%u but creature entry %u does not exist.",
                cInfo->Entry, diff + 1, cInfo->DifficultyEntry[diff], cInfo->DifficultyEntry[diff]);
            continue;
        }

        bool ok2 = true;
        for (uint32 diff2 = 0; diff2 < MAX_TEMPLATE_DIFFICULTY - 1 && ok2; ++diff2)
        {
            ok2 = false;

            if (_difficultyEntries[diff2].find(cInfo->Entry) != _difficultyEntries[diff2].end())
            {
                TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) is listed as `difficulty_entry_%u` of another creature, but itself lists %u in `difficulty_entry_%u`.",
                    cInfo->Entry, diff2 + 1, cInfo->DifficultyEntry[diff], diff + 1);
                continue;
            }

            if (_difficultyEntries[diff2].find(cInfo->DifficultyEntry[diff]) != _difficultyEntries[diff2].end())
            {
                TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) already listed as `difficulty_entry_%u` for another entry.", cInfo->DifficultyEntry[diff], diff2 + 1);
                continue;
            }

            if (_hasDifficultyEntries[diff2].find(cInfo->DifficultyEntry[diff]) != _hasDifficultyEntries[diff2].end())
            {
                TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has `difficulty_entry_%u` = %u but creature entry %u has itself a value in `difficulty_entry_%u`.",
                    cInfo->Entry, diff + 1, cInfo->DifficultyEntry[diff], cInfo->DifficultyEntry[diff], diff2 + 1);
                continue;
            }
            ok2 = true;
        }
        if (!ok2)
            continue;

        if (cInfo->unit_class != difficultyInfo->unit_class)
        {
            TC_LOG_ERROR("sql.sql", "Creature (Entry: %u, class %u) has different `unit_class` in difficulty %u mode (Entry: %u, class %u).",
                cInfo->Entry, cInfo->unit_class, diff + 1, cInfo->DifficultyEntry[diff], difficultyInfo->unit_class);
            continue;
        }

        if (cInfo->npcflag != difficultyInfo->npcflag)
        {
            TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has different `npcflag` in difficulty %u mode (Entry: %u).", cInfo->Entry, diff + 1, cInfo->DifficultyEntry[diff]);
            continue;
        }

        if (cInfo->trainer_class != difficultyInfo->trainer_class)
        {
            TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has different `trainer_class` in difficulty %u mode (Entry: %u).", cInfo->Entry, diff + 1, cInfo->DifficultyEntry[diff]);
            continue;
        }

        if (cInfo->trainer_race != difficultyInfo->trainer_race)
        {
            TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has different `trainer_race` in difficulty %u mode (Entry: %u).", cInfo->Entry, diff + 1, cInfo->DifficultyEntry[diff]);
            continue;
        }

        if (cInfo->trainer_type != difficultyInfo->trainer_type)
        {
            TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has different `trainer_type` in difficulty %u mode (Entry: %u).", cInfo->Entry, diff + 1, cInfo->DifficultyEntry[diff]);
            continue;
        }

        if (!difficultyInfo->AIName.empty())
        {
            TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) lists difficulty %u mode entry %u with `AIName` filled in. `AIName` of difficulty 0 mode creature is always used instead.",
                cInfo->Entry, diff + 1, cInfo->DifficultyEntry[diff]);
            continue;
        }

        if (difficultyInfo->ScriptID)
        {
            TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) lists difficulty %u mode entry %u with `ScriptName` filled in. `ScriptName` of difficulty 0 mode creature is always used instead.",
                cInfo->Entry, diff + 1, cInfo->DifficultyEntry[diff]);
            continue;
        }

        _hasDifficultyEntries[diff].insert(cInfo->Entry);
        _difficultyEntries[diff].insert(cInfo->DifficultyEntry[diff]);
        ok = true;
    }

    FactionTemplateEntry const* factionTemplate = sFactionTemplateStore.LookupEntry(cInfo->faction);
    if (!factionTemplate)
        TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has non-existing faction template (%u).", cInfo->Entry, cInfo->faction);

    for (int k = 0; k < MAX_KILL_CREDIT; ++k)
    {
        if (cInfo->KillCredit[k])
        {
            if (!GetCreatureTemplate(cInfo->KillCredit[k]))
            {
                TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) lists non-existing creature entry %u in `KillCredit%d`.", cInfo->Entry, cInfo->KillCredit[k], k + 1);
                const_cast<CreatureTemplate*>(cInfo)->KillCredit[k] = 0;
            }
        }
    }

    if (!cInfo->Models.size())
        TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) does not have any existing display id in creature_template_model.", cInfo->Entry);
    else if (std::accumulate(cInfo->Models.begin(), cInfo->Models.end(), 0.0f, [](float sum, CreatureModel const& model) { return sum + model.Probability; }) <= 0.0f)
        TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has zero total chance for all models in creature_template_model.", cInfo->Entry);

    if (!cInfo->unit_class || ((1 << (cInfo->unit_class - 1)) & CLASSMASK_ALL_CREATURES) == 0)
    {
        TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has invalid unit_class (%u) in creature_template. Set to 1 (UNIT_CLASS_WARRIOR).", cInfo->Entry, cInfo->unit_class);
        const_cast<CreatureTemplate*>(cInfo)->unit_class = UNIT_CLASS_WARRIOR;
    }

    if (cInfo->dmgschool >= MAX_SPELL_SCHOOL)
    {
        TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has invalid spell school value (%u) in `dmgschool`.", cInfo->Entry, cInfo->dmgschool);
        const_cast<CreatureTemplate*>(cInfo)->dmgschool = SPELL_SCHOOL_NORMAL;
    }

    if (cInfo->BaseAttackTime == 0)
        const_cast<CreatureTemplate*>(cInfo)->BaseAttackTime = BASE_ATTACK_TIME;

    if (cInfo->RangeAttackTime == 0)
        const_cast<CreatureTemplate*>(cInfo)->RangeAttackTime = BASE_ATTACK_TIME;

    if ((cInfo->npcflag & UNIT_NPC_FLAG_TRAINER) && cInfo->trainer_type >= MAX_TRAINER_TYPE)
        TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has wrong trainer type %u.", cInfo->Entry, cInfo->trainer_type);

    if (cInfo->speed_walk == 0.0f)
    {
        TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has wrong value (%f) in speed_walk, set to 1.", cInfo->Entry, cInfo->speed_walk);
        const_cast<CreatureTemplate*>(cInfo)->speed_walk = 1.0f;
    }

    if (cInfo->speed_run == 0.0f)
    {
        TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has wrong value (%f) in speed_run, set to 1.14286.", cInfo->Entry, cInfo->speed_run);
        const_cast<CreatureTemplate*>(cInfo)->speed_run = 1.14286f;
    }

    if (cInfo->type && !sCreatureTypeStore.LookupEntry(cInfo->type))
    {
        TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has invalid creature type (%u) in `type`.", cInfo->Entry, cInfo->type);
        const_cast<CreatureTemplate*>(cInfo)->type = CREATURE_TYPE_HUMANOID;
    }

    // must exist or used hidden but used in data horse case
    if (cInfo->family && !sCreatureFamilyStore.LookupEntry(cInfo->family) && cInfo->family != CREATURE_FAMILY_HORSE_CUSTOM)
    {
        TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has invalid creature family (%u) in `family`.", cInfo->Entry, cInfo->family);
        const_cast<CreatureTemplate*>(cInfo)->family = 0;
    }

    CheckCreatureMovement("creature_template_movement", cInfo->Entry, const_cast<CreatureTemplate*>(cInfo)->Movement);

    if (cInfo->HoverHeight < 0.0f)
    {
        TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has wrong value (%f) in `HoverHeight`", cInfo->Entry, cInfo->HoverHeight);
        const_cast<CreatureTemplate*>(cInfo)->HoverHeight = 1.0f;
    }

    if (cInfo->VehicleId)
    {
        VehicleEntry const* vehId = sVehicleStore.LookupEntry(cInfo->VehicleId);
        if (!vehId)
        {
             TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has a non-existing VehicleId (%u). This *WILL* cause the client to freeze!", cInfo->Entry, cInfo->VehicleId);
             const_cast<CreatureTemplate*>(cInfo)->VehicleId = 0;
        }
    }

    if (cInfo->PetSpellDataId)
    {
        CreatureSpellDataEntry const* spellDataId = sCreatureSpellDataStore.LookupEntry(cInfo->PetSpellDataId);
        if (!spellDataId)
            TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has non-existing PetSpellDataId (%u).", cInfo->Entry, cInfo->PetSpellDataId);
    }

    for (uint8 j = 0; j < CREATURE_MAX_SPELLS; ++j)
    {
        if (cInfo->spells[j] && !sSpellMgr->GetSpellInfo(cInfo->spells[j]))
        {
            TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has non-existing Spell%d (%u), set to 0.", cInfo->Entry, j + 1, cInfo->spells[j]);
            const_cast<CreatureTemplate*>(cInfo)->spells[j] = 0;
        }
    }

    if (cInfo->MovementType >= MAX_DB_MOTION_TYPE)
    {
        TC_LOG_ERROR("sql.sql", "Creature (Entry: %u) has wrong movement generator type (%u), ignored and set to IDLE.", cInfo->Entry, cInfo->MovementType);
        const_cast<CreatureTemplate*>(cInfo)->MovementType = IDLE_MOTION_TYPE;
    }

    if (cInfo->expansion > MAX_CREATURE_BASE_HP)
    {
        TC_LOG_ERROR("sql.sql", "Table `creature_template` lists creature (Entry: %u) with `exp` %u. Ignored and set to 0.", cInfo->Entry, cInfo->expansion);
        const_cast<CreatureTemplate*>(cInfo)->expansion = 0;
    }

    if (cInfo->expansionUnknown > MAX_CREATURE_BASE_HP)
    {
        TC_LOG_ERROR("sql.sql", "Table `creature_template` lists creature (Entry: %u) with `exp_unk` %u. Ignored and set to 0.", cInfo->Entry, cInfo->expansionUnknown);
        const_cast<CreatureTemplate*>(cInfo)->expansionUnknown = 0;
    }

    if (uint32 badFlags = (cInfo->flags_extra & ~CREATURE_FLAG_EXTRA_DB_ALLOWED))
    {
        TC_LOG_ERROR("sql.sql", "Table `creature_template` lists creature (Entry: %u) with disallowed `flags_extra` %u, removing incorrect flag.", cInfo->Entry, badFlags);
        const_cast<CreatureTemplate*>(cInfo)->flags_extra &= CREATURE_FLAG_EXTRA_DB_ALLOWED;
    }

    if (cInfo->VignetteID && !sVignetteStore.LookupEntry(cInfo->VignetteID))
    {
        TC_LOG_ERROR("sql.sql", "Creature(Entry: %u) has a non - existing vignette id (%u)", cInfo->Entry, cInfo->VignetteID);
        const_cast<CreatureTemplate*>(cInfo)->VignetteID = 0;
    }

    // -1 is used in the client for auto-updating the levels
    // having their expansion set to it to the latest one
    if (cInfo->expansion == -1)
    {
        const_cast<CreatureTemplate*>(cInfo)->minlevel = (MAX_LEVEL + cInfo->minlevel);
        const_cast<CreatureTemplate*>(cInfo)->maxlevel = (MAX_LEVEL + cInfo->maxlevel);
        const_cast<CreatureTemplate*>(cInfo)->expansion = EXPANSION_MISTS_OF_PANDARIA;
    }

    if (cInfo->minlevel < 1 || cInfo->minlevel > STRONG_MAX_LEVEL)
    {
        TC_LOG_ERROR("sql.sql", "Creature (ID: %u): MinLevel %i is not within [1, 255], value has been set to 1.", cInfo->Entry, cInfo->minlevel);
        const_cast<CreatureTemplate*>(cInfo)->minlevel = 1;
    }

    if (cInfo->maxlevel < 1 || cInfo->maxlevel > STRONG_MAX_LEVEL)
    {
        TC_LOG_ERROR("sql.sql", "Creature (ID: %u): MaxLevel %i is not within [1, 255], value has been set to 1.", cInfo->Entry, cInfo->maxlevel);
        const_cast<CreatureTemplate*>(cInfo)->maxlevel = 1;
    }

    const_cast<CreatureTemplate*>(cInfo)->dmg_multiplier *= Creature::_GetDamageMod(cInfo->rank);
}

void ObjectMgr::CheckCreatureMovement(char const* table, uint64 id, CreatureMovementData& creatureMovement)
{
    if (creatureMovement.Ground >= CreatureGroundMovementType::Max)
    {
        TC_LOG_ERROR("sql.sql", "`%s`.`Ground` wrong value (%u) for Id %lu, setting to Run.",
            table, uint32(creatureMovement.Ground), id);
        creatureMovement.Ground = CreatureGroundMovementType::Run;
    }

    if (creatureMovement.Flight >= CreatureFlightMovementType::Max)
    {
        TC_LOG_ERROR("sql.sql", "`%s`.`Flight` wrong value (%u) for Id %lu, setting to None.",
            table, uint32(creatureMovement.Flight), id);
        creatureMovement.Flight = CreatureFlightMovementType::None;
    }

    if (creatureMovement.Chase >= CreatureChaseMovementType::Max)
    {
        TC_LOG_ERROR("sql.sql", "`%s`.`Chase` wrong value (%u) for Id %lu, setting to Run.",
                     table, uint32(creatureMovement.Chase), id);
        creatureMovement.Chase = CreatureChaseMovementType::Run;
    }

    if (creatureMovement.Random >= CreatureRandomMovementType::Max)
    {
        TC_LOG_ERROR("sql.sql", "`%s`.`Random` wrong value (%u) for Id %lu, setting to Walk.",
                     table, uint32(creatureMovement.Random), id);
        creatureMovement.Random = CreatureRandomMovementType::Walk;
    }
}

void ObjectMgr::LoadCreatureAddons()
{
    uint32 oldMSTime = getMSTime();

    //                                                 0       1       2        3          4        5           6          7        8         9                      10                11           12            13
    QueryResult result = WorldDatabase.Query("SELECT guid, path_id, mount, StandState, AnimTier, VisFlags, SheathState, PvPFlags, emote, visibilityDistanceType, aiAnimKit, movementAnimKit, meleeAnimKit, auras FROM creature_addon");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 creature addon definitions. DB table `creature_addon` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint64 guid = fields[0].GetUInt64();

        CreatureData const* creData = GetCreatureData(guid);
        if (!creData)
        {
            TC_LOG_ERROR("sql.sql", "Creature (GUID: %lu) does not exist but has a record in `creature_addon`", guid);
            continue;
        }

        CreatureAddon& creatureAddon = _creatureAddonStore[guid];

        creatureAddon.path_id = fields[1].GetUInt32();
        if (creData->movementType == WAYPOINT_MOTION_TYPE && !creatureAddon.path_id)
        {
            const_cast<CreatureData*>(creData)->movementType = IDLE_MOTION_TYPE;
            TC_LOG_ERROR("sql.sql", "Creature (GUID %lu) has movement type set to WAYPOINT_MOTION_TYPE but no path assigned", guid);
        }

        creatureAddon.mount                     = fields[2].GetUInt32();
        creatureAddon.standState                = fields[3].GetUInt8();
        creatureAddon.animTier                  = fields[4].GetUInt8();
        creatureAddon.visFlags                  = fields[5].GetUInt8();
        creatureAddon.sheathState               = fields[6].GetUInt8();
        creatureAddon.pvpFlags                  = fields[7].GetUInt8();
        creatureAddon.emote                     = fields[8].GetUInt32();
        creatureAddon.visibilityDistanceType    = VisibilityDistanceType(fields[9].GetUInt8());
        creatureAddon.aiAnimKit               = fields[10].GetUInt16();
        creatureAddon.movementAnimKit         = fields[11].GetUInt16();
        creatureAddon.meleeAnimKit            = fields[12].GetUInt16();

        Tokenizer tokens(fields[13].GetString(), ' ');
        uint8 i = 0;
        creatureAddon.auras.resize(tokens.size());
        for (Tokenizer::const_iterator itr = tokens.begin(); itr != tokens.end(); ++itr)
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(uint32(atol(*itr)));
            if (!spellInfo)
            {
                TC_LOG_ERROR("sql.sql", "Creature (GUID: %lu) has wrong spell %u defined in `auras` field in `creature_addon`.", guid, uint32(atol(*itr)));
                continue;
            }

            if (spellInfo->HasAura(SPELL_AURA_CONTROL_VEHICLE))
                TC_LOG_ERROR("sql.sql", "Creature (GUID: %lu) has SPELL_AURA_CONTROL_VEHICLE aura %u defined in `auras` field in `creature_addon`.", guid, uint32(atol(*itr)));

            if (!CheckCreatureAddonSpell(creData->id, spellInfo))
            {
                TC_LOG_ERROR("sql.sql", "Creature (GUID: %lu) has spell %u defined in `auras` field in `creature_addon`  which summon this creature", guid, spellInfo->Id);
                continue;
            }

            creatureAddon.auras[i++] = spellInfo->Id;
        }

        if (creatureAddon.mount)
        {
            if (!sCreatureDisplayInfoStore.LookupEntry(creatureAddon.mount))
            {
                TC_LOG_ERROR("sql.sql", "Creature (GUID: %lu) has invalid displayInfoId (%u) for mount defined in `creature_addon`", guid, creatureAddon.mount);
                creatureAddon.mount = 0;
            }
        }

        if (EmotesEntry const* emote = sEmotesStore.LookupEntry(creatureAddon.emote))
        {
            if (emote->Id != 0 && emote->EmoteType == 0)
            {
                TC_LOG_ERROR("sql.sql", "Creature (GUID: %lu) cant`t has oneshot emote %d in `creature_addon` field, you should use scripts, skipped.", guid, creatureAddon.emote);
                creatureAddon.emote = 0;
            }
        }
        else
        {
            TC_LOG_ERROR("sql.sql", "Creature (GUID: %lu) has invalid emote (%u) defined in `creature_addon`.", guid, creatureAddon.emote);
            creatureAddon.emote = 0;
        }

        if (creatureAddon.aiAnimKit && !sAnimKitStore.LookupEntry(creatureAddon.aiAnimKit))
        {
            TC_LOG_ERROR("sql.sql", "Creature (GUID: %lu) has invalid aiAnimKit (%u) defined in `creature_addon`.", guid, creatureAddon.aiAnimKit);
            creatureAddon.aiAnimKit = 0;
        }

        if (creatureAddon.movementAnimKit && !sAnimKitStore.LookupEntry(creatureAddon.movementAnimKit))
        {
            TC_LOG_ERROR("sql.sql", "Creature (GUID: %lu) has invalid movementAnimKit (%u) defined in `creature_addon`.", guid, creatureAddon.movementAnimKit);
            creatureAddon.movementAnimKit = 0;
        }

        if (creatureAddon.meleeAnimKit && !sAnimKitStore.LookupEntry(creatureAddon.meleeAnimKit))
        {
            TC_LOG_ERROR("sql.sql", "Creature (GUID: %lu) has invalid meleeAnimKit (%u) defined in `creature_addon`.", guid, creatureAddon.meleeAnimKit);
            creatureAddon.meleeAnimKit = 0;
        }

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u creature addons in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadGameObjectAddons()
{
    uint32 oldMSTime = getMSTime();

    //                                               0     1                 2
    QueryResult result = WorldDatabase.Query("SELECT guid, invisibilityType, invisibilityValue FROM gameobject_addon");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 gameobject addon definitions. DB table `gameobject_addon` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint64 guid = fields[0].GetUInt64();

        const GameObjectData* goData = GetGOData(guid);
        if (!goData)
        {
            TC_LOG_ERROR("sql.sql", "GameObject (GUID: %lu) does not exist but has a record in `gameobject_addon`", guid);
            continue;
        }

        GameObjectAddon& gameObjectAddon = _gameObjectAddonStore[guid];
        gameObjectAddon.InvisibilityType = InvisibilityType(fields[1].GetUInt8());
        gameObjectAddon.InvisibilityValue = fields[2].GetUInt32();

        if (gameObjectAddon.InvisibilityType >= TOTAL_INVISIBILITY_TYPES)
        {
            TC_LOG_ERROR("sql.sql", "GameObject (GUID: %lu) has invalid InvisibilityType in `gameobject_addon`", guid);
            gameObjectAddon.InvisibilityType = INVISIBILITY_GENERAL;
            gameObjectAddon.InvisibilityValue = 0;
        }

        if (gameObjectAddon.InvisibilityType && !gameObjectAddon.InvisibilityValue)
        {
            TC_LOG_ERROR("sql.sql", "GameObject (GUID: %lu) has InvisibilityType set but has no InvisibilityValue in `gameobject_addon`, set to 1", guid);
            gameObjectAddon.InvisibilityValue = 1;
        }

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u gameobject addons in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

GameObjectAddon const* ObjectMgr::GetGameObjectAddon(uint32 lowguid)
{
    GameObjectAddonContainer::const_iterator itr = _gameObjectAddonStore.find(lowguid);
    if (itr != _gameObjectAddonStore.end())
        return &(itr->second);

    return NULL;
}

CreatureAddon const* ObjectMgr::GetCreatureAddon(uint32 lowguid)
{
    CreatureAddonContainer::const_iterator itr = _creatureAddonStore.find(lowguid);
    if (itr != _creatureAddonStore.end())
        return &(itr->second);

    return NULL;
}

CreatureAddon const* ObjectMgr::GetCreatureTemplateAddon(uint32 entry) const
{
    CreatureAddonContainer::const_iterator itr = _creatureTemplateAddonStore.find(entry);
    if (itr != _creatureTemplateAddonStore.end())
        return &(itr->second);

    return NULL;
}

CreatureMovementData const* ObjectMgr::GetCreatureMovementOverride(ObjectGuid::LowType spawnId) const
{
    return Trinity::Containers::MapGetValuePtr(_creatureMovementOverrides, spawnId);
}

EquipmentInfo const* ObjectMgr::GetEquipmentInfo(uint32 entry, int8& id)
{
    EquipmentInfoContainer::const_iterator itr = _equipmentInfoStore.find(entry);
    if (itr == _equipmentInfoStore.end())
        return NULL;

    if (itr->second.empty())
        return NULL;

    if (id == -1) // select a random element
    {
        EquipmentInfoContainerInternal::const_iterator ritr = itr->second.begin();
        std::advance(ritr, urand(0u, itr->second.size() - 1));
        id = std::distance(itr->second.begin(), ritr) + 1;
        return &ritr->second;
    }
    else
    {
        EquipmentInfoContainerInternal::const_iterator itr2 = itr->second.find(id);
        if (itr2 != itr->second.end())
            return &itr2->second;
    }

    return NULL;
}

void ObjectMgr::LoadEquipmentTemplates()
{
    uint32 oldMSTime = getMSTime();

    //                                               0           1   2        3        4
    QueryResult result = WorldDatabase.Query("SELECT CreatureID, ID, ItemID1, ItemID2, ItemID3 FROM creature_equip_template");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 creature equipment templates. DB table `creature_equip_template` is empty!");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        if (!sObjectMgr->GetCreatureTemplate(entry))
        {
            TC_LOG_ERROR("sql.sql", "Creature template (Entry: %u) does not exist but has a record in `creature_equip_template`", entry);
            continue;
        }

        uint8 id = fields[1].GetUInt8();
        if (!id)
        {
            TC_LOG_ERROR("sql.sql", "Creature equipment template with id 0 found for creature %u, skipped.", entry);
            continue;
        }

        EquipmentInfo& equipmentInfo = _equipmentInfoStore[entry][id];

        equipmentInfo.ItemEntry[0] = fields[2].GetUInt32();
        equipmentInfo.ItemEntry[1] = fields[3].GetUInt32();
        equipmentInfo.ItemEntry[2] = fields[4].GetUInt32();

        for (uint8 i = 0; i < MAX_EQUIPMENT_ITEMS; ++i)
        {
            if (!equipmentInfo.ItemEntry[i])
                continue;

            ItemEntry const* dbcItem = sItemStore.LookupEntry(equipmentInfo.ItemEntry[i]);

            if (!dbcItem)
            {
                TC_LOG_ERROR("sql.sql", "Unknown item (entry=%u) in creature_equip_template.itemEntry%u for entry = %u and id=%u, forced to 0.",
                    equipmentInfo.ItemEntry[i], i+1, entry, id);
                equipmentInfo.ItemEntry[i] = 0;
                continue;
            }

            if (dbcItem->InventoryType != INVTYPE_WEAPON &&
                dbcItem->InventoryType != INVTYPE_SHIELD &&
                dbcItem->InventoryType != INVTYPE_RANGED &&
                dbcItem->InventoryType != INVTYPE_2HWEAPON &&
                dbcItem->InventoryType != INVTYPE_WEAPONMAINHAND &&
                dbcItem->InventoryType != INVTYPE_WEAPONOFFHAND &&
                dbcItem->InventoryType != INVTYPE_HOLDABLE &&
                dbcItem->InventoryType != INVTYPE_THROWN &&
                dbcItem->InventoryType != INVTYPE_RANGEDRIGHT)
            {
                TC_LOG_ERROR("sql.sql", "Item (entry=%u) in creature_equip_template.itemEntry%u for entry = %u and id = %u is not equipable in a hand, forced to 0.",
                    equipmentInfo.ItemEntry[i], i+1, entry, id);
                equipmentInfo.ItemEntry[i] = 0;
            }
        }

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u equipment templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadCreatureMovementOverrides()
{
    uint32 oldMSTime = getMSTime();

    _creatureMovementOverrides.clear();

    // Load the data from creature_movement_override and if NULL fallback to creature_template_movement
    QueryResult result = WorldDatabase.Query(
        "SELECT cmo.SpawnId,"
        "COALESCE(cmo.Ground, ctm.Ground),"
        "COALESCE(cmo.Swim, ctm.Swim),"
        "COALESCE(cmo.Flight, ctm.Flight),"
        "COALESCE(cmo.Rooted, ctm.Rooted),"
        "COALESCE(cmo.Chase, ctm.Chase),"
        "COALESCE(cmo.Random, ctm.Random),"
        "COALESCE(cmo.InteractionPauseTimer, ctm.InteractionPauseTimer) "
        "FROM creature_movement_override AS cmo "
        "LEFT JOIN creature AS c ON c.guid = cmo.SpawnId "
        "LEFT JOIN creature_template_movement AS ctm ON ctm.CreatureId = c.id");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 creature movement overrides. DB table `creature_movement_override` is empty!");
        return;
    }

    do
    {
        Field* fields = result->Fetch();
        ObjectGuid::LowType spawnId = fields[0].GetUInt32();
        if (!GetCreatureData(spawnId))
        {
            TC_LOG_ERROR("sql.sql", "Creature (GUID: %u) does not exist but has a record in `creature_movement_override`", spawnId);
            continue;
        }

        CreatureMovementData& movement = _creatureMovementOverrides[spawnId];
        if (!fields[1].IsNull())
            movement.Ground = static_cast<CreatureGroundMovementType>(fields[1].GetUInt8());
        if (!fields[2].IsNull())
            movement.Swim = fields[2].GetBool();
        if (!fields[3].IsNull())
            movement.Flight = static_cast<CreatureFlightMovementType>(fields[3].GetUInt8());
        if (!fields[4].IsNull())
            movement.Rooted = fields[4].GetBool();
        if (!fields[5].IsNull())
            movement.Chase = static_cast<CreatureChaseMovementType>(fields[5].GetUInt8());
        if (!fields[6].IsNull())
            movement.Random = static_cast<CreatureRandomMovementType>(fields[6].GetUInt8());
        if (!fields[7].IsNull())
            movement.InteractionPauseTimer = fields[7].GetUInt32();

        CheckCreatureMovement("creature_movement_override", spawnId, movement);
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded " SZFMTD " movement overrides in %u ms", _creatureMovementOverrides.size(), GetMSTimeDiffToNow(oldMSTime));
}

CreatureModelInfo const* ObjectMgr::GetCreatureModelInfo(uint32 modelId) const
{
    CreatureModelContainer::const_iterator itr = _creatureModelStore.find(modelId);
    if (itr != _creatureModelStore.end())
        return &(itr->second);

    return nullptr;
}

CreatureModel const* ObjectMgr::ChooseDisplayId(CreatureTemplate const* cinfo, CreatureData const* data /*= nullptr*/)
{
    // Load creature model (display id)
    if (data && data->displayid)
        if (CreatureModel const* model = cinfo->GetModelWithDisplayId(data->displayid))
            return model;

    if (!(cinfo->flags_extra & CREATURE_FLAG_EXTRA_TRIGGER))
        if (CreatureModel const* model = cinfo->GetRandomValidModel())
            return model;

    // Triggers by default receive the invisible model
    return cinfo->GetFirstInvisibleModel();
}

void ObjectMgr::ChooseCreatureFlags(const CreatureTemplate* cinfo, uint32& npcflag, uint32& unit_flags, uint32& dynamicflags, const CreatureData* data /*= NULL*/)
{
    npcflag = cinfo->npcflag;
    unit_flags = cinfo->unit_flags;
    dynamicflags = cinfo->dynamicflags;

    if (data)
    {
        if (data->npcflag)
            npcflag = data->npcflag;

        if (data->unit_flags)
            unit_flags = data->unit_flags;

        if (data->dynamicflags)
            dynamicflags = data->dynamicflags;
    }
}

CreatureModelInfo const* ObjectMgr::GetCreatureModelRandomGender(CreatureModel* model, CreatureTemplate const* creatureTemplate) const
{
    CreatureModelInfo const* modelInfo = GetCreatureModelInfo(model->CreatureDisplayID);
    if (!modelInfo)
        return nullptr;

    // If a model for another gender exists, 50% chance to use it
    if (modelInfo->modelid_other_gender != 0 && urand(0, 1) == 0)
    {
        CreatureModelInfo const* minfo_tmp = GetCreatureModelInfo(modelInfo->modelid_other_gender);
        if (!minfo_tmp)
            TC_LOG_ERROR("sql.sql", "Model (Entry: %u) has modelid_other_gender %u not found in table `creature_model_info`. ", model->CreatureDisplayID, modelInfo->modelid_other_gender);
        else
        {
            // DisplayID changed
            model->CreatureDisplayID = modelInfo->modelid_other_gender;
            if (creatureTemplate)
            {
                auto itr = std::find_if(creatureTemplate->Models.begin(), creatureTemplate->Models.end(), [&](CreatureModel const& templateModel)
                    {
                        return templateModel.CreatureDisplayID == modelInfo->modelid_other_gender;
                    });
                if (itr != creatureTemplate->Models.end())
                    *model = *itr;
            }
            return minfo_tmp;
        }
    }

    return modelInfo;
}

void ObjectMgr::LoadCreatureModelInfo()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT modelid, bounding_radius, combat_reach, gender, modelid_other_gender FROM creature_model_info");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 creature model definitions. DB table `creature_model_info` is empty.");
        return;
    }

    _creatureModelStore.rehash(result->GetRowCount());
    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 modelId = fields[0].GetUInt32();
        CreatureDisplayInfoEntry const* creatureDisplay = sCreatureDisplayInfoStore.LookupEntry(modelId);
        if (!creatureDisplay)
        {
            TC_LOG_ERROR("sql.sql", "Table `creature_model_info` has model for nonexistent display id (%u).", modelId);
            continue;
        }

        CreatureModelInfo& modelInfo = _creatureModelStore[modelId];

        modelInfo.bounding_radius      = fields[1].GetFloat();
        modelInfo.combat_reach         = fields[2].GetFloat();
        modelInfo.gender               = fields[3].GetUInt8();
        modelInfo.modelid_other_gender = fields[4].GetUInt32();
        modelInfo.is_trigger           = false;

        // Checks

        if (!sCreatureDisplayInfoStore.LookupEntry(modelId))
            TC_LOG_ERROR("sql.sql", "Table `creature_model_info` has model for not existed display id (%u).", modelId);

        if (modelInfo.gender > GENDER_NONE)
        {
            TC_LOG_ERROR("sql.sql", "Table `creature_model_info` has wrong gender (%u) for display id (%u).", uint32(modelInfo.gender), modelId);
            modelInfo.gender = GENDER_MALE;
        }

        if (modelInfo.modelid_other_gender && !sCreatureDisplayInfoStore.LookupEntry(modelInfo.modelid_other_gender))
        {
            TC_LOG_ERROR("sql.sql", "Table `creature_model_info` has not existed alt.gender model (%u) for existed display id (%u).", modelInfo.modelid_other_gender, modelId);
            modelInfo.modelid_other_gender = 0;
        }

        if (CreatureModelDataEntry const* modelData = sCreatureModelDataStore.LookupEntry(creatureDisplay->ModelID))
            modelInfo.is_trigger = strstr(modelData->ModelName, "INVISIBLESTALKER");

        if (modelInfo.combat_reach < 0.1f)
            modelInfo.combat_reach = DEFAULT_PLAYER_COMBAT_REACH;

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u creature model based info in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadLinkedRespawn()
{
    uint32 oldMSTime = getMSTime();

    _linkedRespawnStore.clear();
    //                                                 0        1          2
    QueryResult result = WorldDatabase.Query("SELECT guid, linkedGuid, linkType FROM linked_respawn ORDER BY guid ASC");

    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 linked respawns. DB table `linked_respawn` is empty.");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 guidLow = fields[0].GetUInt32();
        uint32 linkedGuidLow = fields[1].GetUInt32();
        uint8  linkType = fields[2].GetUInt8();

        ObjectGuid guid = ObjectGuid::Empty, linkedGuid = ObjectGuid::Empty;
        bool error = false;
        switch (linkType)
        {
            case CREATURE_TO_CREATURE:
            {
                const CreatureData* slave = GetCreatureData(guidLow);
                if (!slave)
                {
                    TC_LOG_ERROR("sql.sql", "Couldn't get creature data for GUIDLow %u", guidLow);
                    error = true;
                    break;
                }

                const CreatureData* master = GetCreatureData(linkedGuidLow);
                if (!master)
                {
                    TC_LOG_ERROR("sql.sql", "Couldn't get creature data for GUIDLow %u", linkedGuidLow);
                    error = true;
                    break;
                }

                const MapEntry* const map = sMapStore.LookupEntry(master->mapId);
                if (!map || !map->Instanceable() || (master->mapId != slave->mapId))
                {
                    TC_LOG_ERROR("sql.sql", "Creature '%u' linking to '%u' on an unpermitted map.", guidLow, linkedGuidLow);
                    error = true;
                    break;
                }

                if (!(master->spawnMask & slave->spawnMask))  // they must have a possibility to meet (normal/heroic difficulty)
                {
                    TC_LOG_ERROR("sql.sql", "LinkedRespawn: Creature '%u' linking to '%u' with not corresponding spawnMask", guidLow, linkedGuidLow);
                    error = true;
                    break;
                }

                guid = ObjectGuid(HighGuid::Unit, slave->id, guidLow);
                linkedGuid = ObjectGuid(HighGuid::Unit, master->id, linkedGuidLow);
                break;
            }
            case CREATURE_TO_GO:
            {
                const CreatureData* slave = GetCreatureData(guidLow);
                if (!slave)
                {
                    TC_LOG_ERROR("sql.sql", "Couldn't get creature data for GUIDLow %u", guidLow);
                    error = true;
                    break;
                }

                const GameObjectData* master = GetGOData(linkedGuidLow);
                if (!master)
                {
                    TC_LOG_ERROR("sql.sql", "Couldn't get gameobject data for GUIDLow %u", linkedGuidLow);
                    error = true;
                    break;
                }

                const MapEntry* const map = sMapStore.LookupEntry(master->mapid);
                if (!map || !map->Instanceable() || (master->mapid != slave->mapId))
                {
                    TC_LOG_ERROR("sql.sql", "Creature '%u' linking to '%u' on an unpermitted map.", guidLow, linkedGuidLow);
                    error = true;
                    break;
                }

                if (!(master->spawnMask & slave->spawnMask))  // they must have a possibility to meet (normal/heroic difficulty)
                {
                    TC_LOG_ERROR("sql.sql", "LinkedRespawn: Creature '%u' linking to '%u' with not corresponding spawnMask", guidLow, linkedGuidLow);
                    error = true;
                    break;
                }

                guid = ObjectGuid(HighGuid::Unit, slave->id, guidLow);
                linkedGuid = ObjectGuid(HighGuid::GameObject, master->id, linkedGuidLow);
                break;
            }
            case GO_TO_GO:
            {
                const GameObjectData* slave = GetGOData(guidLow);
                if (!slave)
                {
                    TC_LOG_ERROR("sql.sql", "Couldn't get gameobject data for GUIDLow %u", guidLow);
                    error = true;
                    break;
                }

                const GameObjectData* master = GetGOData(linkedGuidLow);
                if (!master)
                {
                    TC_LOG_ERROR("sql.sql", "Couldn't get gameobject data for GUIDLow %u", linkedGuidLow);
                    error = true;
                    break;
                }

                const MapEntry* const map = sMapStore.LookupEntry(master->mapid);
                if (!map || !map->Instanceable() || (master->mapid != slave->mapid))
                {
                    TC_LOG_ERROR("sql.sql", "Creature '%u' linking to '%u' on an unpermitted map.", guidLow, linkedGuidLow);
                    error = true;
                    break;
                }

                if (!(master->spawnMask & slave->spawnMask))  // they must have a possibility to meet (normal/heroic difficulty)
                {
                    TC_LOG_ERROR("sql.sql", "LinkedRespawn: Creature '%u' linking to '%u' with not corresponding spawnMask", guidLow, linkedGuidLow);
                    error = true;
                    break;
                }

                guid = ObjectGuid(HighGuid::GameObject, slave->id, guidLow);
                linkedGuid = ObjectGuid(HighGuid::GameObject, master->id, linkedGuidLow);
                break;
            }
            case GO_TO_CREATURE:
            {
                const GameObjectData* slave = GetGOData(guidLow);
                if (!slave)
                {
                    TC_LOG_ERROR("sql.sql", "Couldn't get gameobject data for GUIDLow %u", guidLow);
                    error = true;
                    break;
                }

                const CreatureData* master = GetCreatureData(linkedGuidLow);
                if (!master)
                {
                    TC_LOG_ERROR("sql.sql", "Couldn't get creature data for GUIDLow %u", linkedGuidLow);
                    error = true;
                    break;
                }

                const MapEntry* const map = sMapStore.LookupEntry(master->mapId);
                if (!map || !map->Instanceable() || (master->mapId != slave->mapid))
                {
                    TC_LOG_ERROR("sql.sql", "Creature '%u' linking to '%u' on an unpermitted map.", guidLow, linkedGuidLow);
                    error = true;
                    break;
                }

                if (!(master->spawnMask & slave->spawnMask))  // they must have a possibility to meet (normal/heroic difficulty)
                {
                    TC_LOG_ERROR("sql.sql", "LinkedRespawn: Creature '%u' linking to '%u' with not corresponding spawnMask", guidLow, linkedGuidLow);
                    error = true;
                    break;
                }

                guid = ObjectGuid(HighGuid::GameObject, slave->id, guidLow);
                linkedGuid = ObjectGuid(HighGuid::Unit, master->id, linkedGuidLow);
                break;
            }
        }

        if (!error)
            _linkedRespawnStore[guid] = linkedGuid;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded " UI64FMTD " linked respawns in %u ms", uint64(_linkedRespawnStore.size()), GetMSTimeDiffToNow(oldMSTime));
}

bool ObjectMgr::SetCreatureLinkedRespawn(uint32 guidLow, uint32 linkedGuidLow)
{
    if (!guidLow)
        return false;

    const CreatureData* master = GetCreatureData(guidLow);
    ObjectGuid guid(HighGuid::Unit, master->id, guidLow);

    if (!linkedGuidLow) // we're removing the linking
    {
        _linkedRespawnStore.erase(guid);
        WorldDatabasePreparedStatement *stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_CRELINKED_RESPAWN);
        stmt->setUInt32(0, guidLow);
        WorldDatabase.Execute(stmt);
        return true;
    }

    const CreatureData* slave = GetCreatureData(linkedGuidLow);

    const MapEntry* const map = sMapStore.LookupEntry(master->mapId);
    if (!map || !map->Instanceable() || (master->mapId != slave->mapId))
    {
        TC_LOG_ERROR("sql.sql", "Creature '%u' linking to '%u' on an unpermitted map.", guidLow, linkedGuidLow);
        return false;
    }

    if (!(master->spawnMask & slave->spawnMask))  // they must have a possibility to meet (normal/heroic difficulty)
    {
        TC_LOG_ERROR("sql.sql", "LinkedRespawn: Creature '%u' linking to '%u' with not corresponding spawnMask", guidLow, linkedGuidLow);
        return false;
    }

    ObjectGuid linkedGuid(HighGuid::Unit, slave->id, linkedGuidLow);

    _linkedRespawnStore[guid] = linkedGuid;
    WorldDatabasePreparedStatement *stmt = WorldDatabase.GetPreparedStatement(WORLD_REP_CREATURE_LINKED_RESPAWN);
    stmt->setUInt32(0, guidLow);
    stmt->setUInt32(1, linkedGuidLow);
    WorldDatabase.Execute(stmt);
    return true;
}

void ObjectMgr::LoadTempSummons()
{
    uint32 oldMSTime = getMSTime();

    _tempSummonDataStore.clear();   // needed for reload case

    //                                               0           1             2        3      4           5           6           7            8           9
    QueryResult result = WorldDatabase.Query("SELECT summonerId, summonerType, groupId, entry, position_x, position_y, position_z, orientation, summonType, summonTime FROM creature_summon_groups");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 temp summons. DB table `creature_summon_groups` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 summonerId               = fields[0].GetUInt32();
        SummonerType summonerType       = SummonerType(fields[1].GetUInt8());
        uint8 group                     = fields[2].GetUInt8();

        switch (summonerType)
        {
            case SUMMONER_TYPE_CREATURE:
                if (!GetCreatureTemplate(summonerId))
                {
                    TC_LOG_ERROR("sql.sql", "Table `creature_summon_groups` has summoner with non existing entry %u for creature summoner type, skipped.", summonerId);
                    continue;
                }
                break;
            case SUMMONER_TYPE_GAMEOBJECT:
                if (!GetGameObjectTemplate(summonerId))
                {
                    TC_LOG_ERROR("sql.sql", "Table `creature_summon_groups` has summoner with non existing entry %u for gameobject summoner type, skipped.", summonerId);
                    continue;
                }
                break;
            case SUMMONER_TYPE_MAP:
                if (!sMapStore.LookupEntry(summonerId))
                {
                    TC_LOG_ERROR("sql.sql", "Table `creature_summon_groups` has summoner with non existing entry %u for map summoner type, skipped.", summonerId);
                    continue;
                }
                break;
            default:
                TC_LOG_ERROR("sql.sql", "Table `creature_summon_groups` has unhandled summoner type %u for summoner %u, skipped.", summonerType, summonerId);
                continue;
        }

        TempSummonData data;
        data.entry                      = fields[3].GetUInt32();

        if (!GetCreatureTemplate(data.entry))
        {
            TC_LOG_ERROR("sql.sql", "Table `creature_summon_groups` has creature in group [Summoner ID: %u, Summoner Type: %u, Group ID: %u] with non existing creature entry %u, skipped.", summonerId, summonerType, group, data.entry);
            continue;
        }

        float posX                      = fields[4].GetFloat();
        float posY                      = fields[5].GetFloat();
        float posZ                      = fields[6].GetFloat();
        float orientation               = fields[7].GetFloat();

        data.pos.Relocate(posX, posY, posZ, orientation);

        data.type                       = TempSummonType(fields[8].GetUInt8());

        if (data.type > TEMPSUMMON_MANUAL_DESPAWN)
        {
            TC_LOG_ERROR("sql.sql", "Table `creature_summon_groups` has unhandled temp summon type %u in group [Summoner ID: %u, Summoner Type: %u, Group ID: %u] for creature entry %u, skipped.", data.type, summonerId, summonerType, group, data.entry);
            continue;
        }

        data.time                       = fields[9].GetUInt32();

        TempSummonGroupKey key(summonerId, summonerType, group);
        _tempSummonDataStore[key].push_back(data);

        ++count;

    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u temp summons in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadCreatures()
{
    uint32 oldMSTime = getMSTime();

    //                                                      0        1   2      3          4            5           6           7            8            9                10                11
    QueryResult result = WorldDatabase.Query("SELECT creature.guid, id, map, modelid, equipment_id, position_x, position_y, position_z, orientation, spawntimesecs, spawntimesecs_max, wander_distance, "
    //         12            13         14         15          16         17       18                19                   20          21          22                23                 24                   25                    26                     27                   28
        "currentwaypoint, curhealth, curmana, MovementType, spawnMask, phaseMask, creature.phaseid, creature.phasegroup, eventEntry, pool_entry, creature.npcflag, creature.npcflag2, creature.unit_flags, creature.unit_flags2, creature.dynamicflags, creature.ScriptName, creature.walk_mode "
        "FROM creature "
        "LEFT OUTER JOIN game_event_creature ON creature.guid = game_event_creature.guid "
        "LEFT OUTER JOIN pool_creature ON creature.guid = pool_creature.guid");

    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 creatures. DB table `creature` is empty.");
        return;
    }

    // Build single time for check spawnmask
    std::map<uint32, uint32> spawnMasks;
    for (uint32 i = 0; i < sMapStore.GetNumRows(); ++i)
        if (sMapStore.LookupEntry(i))
            for (int k = 0; k < MAX_DIFFICULTY; ++k)
                if (GetMapDifficultyData(i, Difficulty(k)))
                    spawnMasks[i] |= (1 << k);

    _creatureDataStore.rehash(result->GetRowCount());
    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 guid               = fields[0].GetUInt32();
        uint32 entry              = fields[1].GetUInt32();

        CreatureTemplate const* cInfo = GetCreatureTemplate(entry);
        if (!cInfo)
        {
            TC_LOG_ERROR("sql.sql", "Table `creature` has creature (GUID: %u) with non existing creature entry %u, skipped.", guid, entry);
            continue;
        }

        CreatureData& data        = _creatureDataStore[guid];
        data.id                   = entry;
        data.mapId                = fields[2].GetUInt16();
        data.displayid            = fields[3].GetUInt32();
        data.equipmentId          = fields[4].GetInt8();
        data.posX                 = fields[5].GetFloat();
        data.posY                 = fields[6].GetFloat();
        data.posZ                 = fields[7].GetFloat();
        data.orientation          = fields[8].GetFloat();
        data.spawntimesecs        = fields[9].GetUInt32();
        data.spawntimesecs_max    = fields[10].GetUInt32();
        data.wander_distance      = fields[11].GetFloat();
        data.currentwaypoint      = fields[12].GetUInt32();
        data.curhealth            = fields[13].GetUInt32();
        data.curmana              = fields[14].GetUInt32();
        data.movementType         = fields[15].GetUInt8();
        data.spawnMask            = fields[16].GetUInt16();
        data.phaseMask            = fields[17].GetUInt32();
        data.phaseid              = fields[18].GetUInt32();
        data.phaseGroup           = fields[19].GetUInt32();
        int16 gameEvent           = fields[20].GetInt8();
        uint32 PoolId             = fields[21].GetUInt32();
        data.npcflag              = fields[22].GetUInt32();
        data.npcflag2             = fields[23].GetUInt32();
        data.unit_flags           = fields[24].GetUInt32();
        data.unit_flags2          = fields[25].GetUInt32();
        data.dynamicflags         = fields[26].GetUInt32();
        data.ScriptId             = GetScriptId(fields[27].GetString());

        data.WalkMode             = fields[28].GetFloat();

        data.gameEventId = gameEvent;

        MapEntry const* mapEntry = sMapStore.LookupEntry(data.mapId);
        if (!mapEntry)
        {
            TC_LOG_ERROR("sql.sql", "Table `creature` have creature (GUID: %u) that spawned at not existed map (Id: %u), skipped.", guid, data.mapId);
            continue;
        }

        // Skip spawnMask check for transport maps
        if (!_transportMaps.count(data.mapId) && data.spawnMask & ~spawnMasks[data.mapId])
            TC_LOG_ERROR("sql.sql", "Table `creature` have creature (GUID: %u) that have wrong spawn mask %u including not supported difficulty modes for map (Id: %u) spawnMasks[data.mapId]: %u.", guid, data.spawnMask, data.mapId, spawnMasks[data.mapId]);

        bool ok = true;
        for (uint32 diff = 0; diff < MAX_TEMPLATE_DIFFICULTY - 1 && ok; ++diff)
        {
            if (_difficultyEntries[diff].find(data.id) != _difficultyEntries[diff].end())
            {
                TC_LOG_ERROR("sql.sql", "Table `creature` have creature (GUID: %u) that listed as difficulty %u template (entry: %u) in `creature_template`, skipped.",
                    guid, diff + 1, data.id);
                ok = false;
            }
        }
        if (!ok)
            continue;

        // -1 random, 0 no equipment,
        if (data.equipmentId != 0)
        {
            if (!GetEquipmentInfo(data.id, data.equipmentId))
            {
                TC_LOG_ERROR("sql.sql", "Table `creature` have creature (Entry: %u) with equipment_id %u not found in table `creature_equip_template`, set to no equipment.", data.id, data.equipmentId);
                data.equipmentId = 0;
            }
        }

        if (cInfo->flags_extra & CREATURE_FLAG_EXTRA_INSTANCE_BIND)
        {
            if (!mapEntry || !mapEntry->IsDungeon())
                TC_LOG_ERROR("sql.sql", "Table `creature` have creature (GUID: %u Entry: %u) with `creature_template`.`flags_extra` including CREATURE_FLAG_EXTRA_INSTANCE_BIND but creature are not in instance.", guid, data.id);
        }

        if (data.wander_distance < 0.0f)
        {
            TC_LOG_ERROR("sql.sql", "Table `creature` have creature (GUID: %u Entry: %u) with `wander_distance`< 0, set to 0.", guid, data.id);
            data.wander_distance = 0.0f;
        }
        else if (data.movementType == RANDOM_MOTION_TYPE)
        {
            if (data.wander_distance == 0.0f)
            {
                TC_LOG_ERROR("sql.sql", "Table `creature` have creature (GUID: %u Entry: %u) with `movement_type`=1 (random movement) but with `wander_distance`=0, replace by idle movement type (0).", guid, data.id);
                data.movementType = IDLE_MOTION_TYPE;
            }
        }
        else if (data.movementType == IDLE_MOTION_TYPE)
        {
            if (data.wander_distance != 0.0f)
            {
                TC_LOG_ERROR("sql.sql", "Table `creature` have creature (GUID: %u Entry: %u) with `movement_type`=0 (idle) have `wander_distance`<>0, set to 0.", guid, data.id);
                data.wander_distance = 0.0f;
            }
        }

        if (data.WalkMode < 0.0f)
        {
            TC_LOG_ERROR("sql.sql", "Table `creature` has creature (GUID: %u Entry: %u) with `walk_mode`< 0, set to 0.",
                guid, data.id);
            data.WalkMode = 0.0f;
        }

        if (data.WalkMode > 2.0f)
        {
            TC_LOG_ERROR("sql.sql", "Table `creature` has creature (GUID: %u Entry: %u) with `walk_mode` > 2, set to 2.",
                guid, data.id);
            data.WalkMode = 2.0f;
        }

        if (data.phaseMask == 0)
        {
            TC_LOG_ERROR("sql.sql", "Table `creature` have creature (GUID: %u Entry: %u) with `phaseMask`=0 (not visible for anyone), set to 1.", guid, data.id);
            data.phaseMask = 1;
        }

        if (data.phaseGroup && GetPhasesForGroup(data.phaseGroup).empty())
        {
            TC_LOG_ERROR("sql.sql", "Table `creature` has creature (GUID: %u Entry: %u) with non-existing `phasegroup` (%u) set, `phasegroup` set to 0", guid, data.id, data.phaseGroup);
            data.phaseGroup = 0;
        }

        if (data.phaseGroup && data.phaseid)
        {
            TC_LOG_ERROR("sql.sql", "Table `creature` have creature (GUID: %u Entry: %u) with both `phaseid` and `phasegroup` set, `phasegroup` set to 0", guid, data.id);
            data.phaseGroup = 0;
        }

        if (sWorld->getBoolConfig(CONFIG_CALCULATE_CREATURE_ZONE_AREA_DATA))
        {
            uint32 zoneId = sMapMgr->GetZoneId(data.phaseMask, data.mapId, data.posX, data.posY, data.posZ);
            uint32 areaId = sMapMgr->GetAreaId(data.phaseMask, data.mapId, data.posX, data.posY, data.posZ);

            WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_UPD_CREATURE_ZONE_AREA_DATA);

            stmt->setUInt32(0, zoneId);
            stmt->setUInt32(1, areaId);
            stmt->setUInt64(2, guid);

            WorldDatabase.Execute(stmt);
        }

        // Add to grid if not managed by the game event or pool system
        if (gameEvent == 0 && PoolId == 0)
            AddCreatureToGrid(guid, &data);

        ++count;

    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u creatures in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::AddCreatureToGrid(uint32 guid, CreatureData const* data)
{
    uint16 mask = data->spawnMask;
    for (uint16 i = 0; mask != 0; i++, mask >>= 1)
    {
        if (mask & 1)
        {
            CellCoord cellCoord = Trinity::ComputeCellCoord(data->posX, data->posY);
            CellObjectGuids& cell_guids = _mapObjectGuidsStore[MAKE_PAIR32(data->mapId, i)][cellCoord.GetId()];
            cell_guids.creatures.insert(guid);
        }
    }
}

void ObjectMgr::RemoveCreatureFromGrid(uint32 guid, CreatureData const* data)
{
    uint16 mask = data->spawnMask;
    for (uint16 i = 0; mask != 0; i++, mask >>= 1)
    {
        if (mask & 1)
        {
            CellCoord cellCoord = Trinity::ComputeCellCoord(data->posX, data->posY);
            CellObjectGuids& cell_guids = _mapObjectGuidsStore[MAKE_PAIR32(data->mapId, i)][cellCoord.GetId()];
            cell_guids.creatures.erase(guid);
        }
    }
}

uint32 ObjectMgr::AddGOData(uint32 entry, uint32 mapId, Position const& position, uint32 spawntimedelay, G3D::Quat const& rotation)
{
    GameObjectTemplate const* goinfo = GetGameObjectTemplate(entry);
    if (!goinfo)
        return 0;

    Map* map = sMapMgr->CreateBaseMap(mapId);
    if (!map)
        return 0;

    ObjectGuid::LowType guid = map->GenerateLowGuid<HighGuid::GameObject>();
    GameObjectData& data = NewGOData(guid);
    data.id             = entry;
    data.mapid          = mapId;
    data.posX           = position.GetPositionX();
    data.posY           = position.GetPositionY();
    data.posZ           = position.GetPositionZ();
    data.orientation    = position.GetOrientation();
    data.rotation.x     = rotation.x;
    data.rotation.y     = rotation.y;
    data.rotation.z     = rotation.z;
    data.rotation.w     = rotation.w;
    data.spawntimesecs  = spawntimedelay;
    data.animprogress   = 100;
    data.spawnMask      = 1;
    data.go_state       = GO_STATE_READY;
    data.phaseMask      = PHASEMASK_NORMAL;
	data.phaseid        = 169;
    data.phaseGroup     = 0;
    data.artKit         = goinfo->type == GAMEOBJECT_TYPE_CAPTURE_POINT ? 21 : 0;
    data.dbData = false;

    AddGameobjectToGrid(guid, &data);

    // Spawn if necessary (loaded grids only)
    // We use spawn coords to spawn
    if (!map->Instanceable() && map->IsGridLoaded(data.posX, data.posY))
    {
        GameObject* go = new GameObject;
        if (!go->LoadGameObjectFromDB(guid, map))
        {
            TC_LOG_ERROR("misc", "AddGOData: cannot add gameobject entry %u to map", entry);
            delete go;
            return 0;
        }
    }

    TC_LOG_DEBUG("maps", "AddGOData: dbguid %u entry %u map %u x %f y %f z %f o %f", guid, entry, mapId, position.GetPositionX(), position.GetPositionY(), position.GetPositionZ(), position.GetOrientation());

    return guid;
}

bool ObjectMgr::MoveCreData(uint32 guid, uint32 mapId, const Position& pos)
{
    CreatureData& data = NewOrExistCreatureData(guid);
    if (!data.id)
        return false;

    RemoveCreatureFromGrid(guid, &data);
    if (data.posX == pos.GetPositionX() && data.posY == pos.GetPositionY() && data.posZ == pos.GetPositionZ())
        return true;
    data.posX = pos.GetPositionX();
    data.posY = pos.GetPositionY();
    data.posZ = pos.GetPositionZ();
    data.orientation = pos.GetOrientation();
    AddCreatureToGrid(guid, &data);

    // Spawn if necessary (loaded grids only)
    if (Map* map = sMapMgr->CreateBaseMap(mapId))
    {
        // We use spawn coords to spawn
        if (!map->Instanceable() && map->IsGridLoaded(data.posX, data.posY))
        {
            Creature* creature = new Creature;
            if (!creature->LoadCreatureFromDB(guid, map))
            {
                TC_LOG_ERROR("misc", "MoveCreData: Cannot add creature guid %u to map", guid);
                delete creature;
                return false;
            }
        }
    }
    return true;
}

uint32 ObjectMgr::AddCreData(uint32 entry, uint32 /*team*/, uint32 mapId, float x, float y, float z, float o, uint32 spawntimedelay)
{
    CreatureTemplate const* cInfo = GetCreatureTemplate(entry);
    if (!cInfo)
        return 0;

    uint32 level = cInfo->minlevel == cInfo->maxlevel ? cInfo->minlevel : urand(cInfo->minlevel, cInfo->maxlevel); // Only used for extracting creature base stats
    CreatureBaseStats const* stats = GetCreatureBaseStats(level, cInfo->unit_class);

    // Spawn if necessary (loaded grids only)
    if (Map* map = sMapMgr->CreateBaseMap(mapId))
    {
        ObjectGuid::LowType guid = map->GenerateLowGuid<HighGuid::Unit>();
        CreatureData& data = NewOrExistCreatureData(guid);
        data.id = entry;
        data.mapId = mapId;
        data.displayid = 0;
        data.equipmentId = 0;
        data.posX = x;
        data.posY = y;
        data.posZ = z;
        data.orientation = o;
        data.spawntimesecs = spawntimedelay;
        data.spawntimesecs_max = 0;
        data.wander_distance = 0;
        data.currentwaypoint = 0;
        data.curhealth = stats->GenerateHealth(cInfo);
        data.curmana = stats->GenerateMana(cInfo);
        data.movementType = cInfo->MovementType;
        data.spawnMask = 1;
        data.phaseMask = PHASEMASK_NORMAL;
        data.phaseid = 169;
        data.phaseGroup = 0;
        data.dbData = false;
        data.npcflag = cInfo->npcflag;
        data.npcflag2 = cInfo->npcflag2;
        data.unit_flags = cInfo->unit_flags;
        data.unit_flags2 = cInfo->unit_flags2;
        data.dynamicflags = cInfo->dynamicflags;
        data.WalkMode = 0;

        AddCreatureToGrid(guid, &data);

        // We use spawn coords to spawn
        if (!map->Instanceable() && !map->IsRemovalGrid(x, y))
        {
            Creature* creature = new Creature;
            if (!creature->LoadCreatureFromDB(guid, map))
            {
                TC_LOG_ERROR("misc", "AddCreature: Cannot add creature entry %u to map", entry);
                delete creature;
                return 0;
            }
        }

        return guid;
    }

    return 0;
}

void ObjectMgr::LoadGameobjects()
{
    uint32 oldMSTime = getMSTime();

    uint32 count = 0;

    //                                                0                1   2    3           4           5           6
    QueryResult result = WorldDatabase.Query("SELECT gameobject.guid, id, map, position_x, position_y, position_z, orientation, "
    //   7          8          9          10         11             12             13       14         15      16       17          18          19          20
        "rotation0, rotation1, rotation2, rotation3, spawntimesecs, animprogress, state, spawnMask, phaseMask, phaseid, phasegroup, eventEntry, pool_entry, ScriptName "
        "FROM gameobject LEFT OUTER JOIN game_event_gameobject ON gameobject.guid = game_event_gameobject.guid "
        "LEFT OUTER JOIN pool_gameobject ON gameobject.guid = pool_gameobject.guid");

    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 gameobjects. DB table `gameobject` is empty.");
        return;
    }

    // build single time for check spawnmask
    std::map<uint32, uint32> spawnMasks;
    for (uint32 i = 0; i < sMapStore.GetNumRows(); ++i)
        if (sMapStore.LookupEntry(i))
            for (int k = 0; k < MAX_DIFFICULTY; ++k)
                if (GetMapDifficultyData(i, Difficulty(k)))
                    spawnMasks[i] |= (1 << k);

    _gameObjectDataStore.rehash(result->GetRowCount());
    do
    {
        Field* fields = result->Fetch();

        uint32 guid         = fields[0].GetUInt32();
        uint32 entry        = fields[1].GetUInt32();

        GameObjectTemplate const* gInfo = GetGameObjectTemplate(entry);
        if (!gInfo)
        {
            TC_LOG_ERROR("sql.sql", "Table `gameobject` has gameobject (GUID: %u) with non existing gameobject entry %u, skipped.", guid, entry);
            continue;
        }

        if (!gInfo->displayId)
        {
            switch (gInfo->type)
            {
                case GAMEOBJECT_TYPE_TRAP:
                case GAMEOBJECT_TYPE_SPELL_FOCUS:
                    break;
                default:
                    TC_LOG_ERROR("sql.sql", "Gameobject (GUID: %u Entry %u GoType: %u) doesn't have a displayId (%u), not loaded.", guid, entry, gInfo->type, gInfo->displayId);
                    break;
            }
        }

        if (gInfo->displayId && !sGameObjectDisplayInfoStore.LookupEntry(gInfo->displayId))
        {
            TC_LOG_ERROR("sql.sql", "Gameobject (GUID: %u Entry %u GoType: %u) has an invalid displayId (%u), not loaded.", guid, entry, gInfo->type, gInfo->displayId);
            continue;
        }

        GameObjectData& data = _gameObjectDataStore[guid];

        data.id             = entry;
        data.mapid          = fields[2].GetUInt16();
        data.posX           = fields[3].GetFloat();
        data.posY           = fields[4].GetFloat();
        data.posZ           = fields[5].GetFloat();
        data.orientation    = fields[6].GetFloat();
        data.rotation.x     = fields[7].GetFloat();
        data.rotation.y     = fields[8].GetFloat();
        data.rotation.z     = fields[9].GetFloat();
        data.rotation.w     = fields[10].GetFloat();
        data.spawntimesecs  = fields[11].GetInt32();

        MapEntry const* mapEntry = sMapStore.LookupEntry(data.mapid);
        if (!mapEntry)
        {
            TC_LOG_ERROR("sql.sql", "Table `gameobject` has gameobject (GUID: %u Entry: %u) spawned on a non-existed map (Id: %u), skip", guid, data.id, data.mapid);
            continue;
        }

        if (data.spawntimesecs == 0 && gInfo->IsDespawnAtAction())
        {
            TC_LOG_ERROR("sql.sql", "Table `gameobject` has gameobject (GUID: %u Entry: %u) with `spawntimesecs` (0) value, but the gameobejct is marked as despawnable at action.", guid, data.id);
        }

        data.animprogress   = fields[12].GetUInt8();
        data.artKit         = 0;

        uint32 go_state     = fields[13].GetUInt8();
        if (go_state != GO_STATE_ACTIVE && go_state != GO_STATE_READY && go_state != GO_STATE_ACTIVE_ALTERNATIVE && go_state != GO_STATE_PREPARE_TRANSPORT)
        {
            TC_LOG_ERROR("sql.sql", "Table `gameobject` has gameobject (GUID: %u Entry: %u) with invalid `state` (%u) value, skip", guid, data.id, go_state);
            continue;
        }
        data.go_state       = GOState(go_state);

        data.spawnMask      = fields[14].GetUInt16();

        if (!_transportMaps.count(data.mapid) && data.spawnMask & ~spawnMasks[data.mapid])
            TC_LOG_ERROR("sql.sql", "Table `gameobject` has gameobject (GUID: %u Entry: %u) that has wrong spawn mask %u including not supported difficulty modes for map (Id: %u), skip", guid, data.id, data.spawnMask, data.mapid);

        data.phaseMask      = fields[15].GetUInt32();
        data.phaseid        = fields[16].GetUInt32();
        data.phaseGroup     = fields[17].GetUInt32();
        int16 gameEvent     = fields[18].GetInt8();
        uint32 PoolId       = fields[19].GetUInt32();
        data.ScriptId       = GetScriptId(fields[20].GetString());

        data.gameEventId = gameEvent;

        if (data.rotation.x < -1.0f || data.rotation.x > 1.0f)
        {
            TC_LOG_ERROR("sql.sql", "Table `gameobject` have gameobject (GUID: %u Entry: %u) with invalid rotationX (%f) value, skip", guid, data.id, data.rotation.x);
            continue;
        }

        if (data.rotation.y < -1.0f || data.rotation.y > 1.0f)
        {
            TC_LOG_ERROR("sql.sql", "Table `gameobject` have gameobject (GUID: %u Entry: %u) with invalid rotationY (%f) value, skip", guid, data.id, data.rotation.y);
            continue;
        }

        if (data.rotation.z < -1.0f || data.rotation.z > 1.0f)
        {
            TC_LOG_ERROR("sql.sql", "Table `gameobject` have gameobject (GUID: %u Entry: %u) with invalid rotationZ (%f) value, skip", guid, data.id, data.rotation.z);
            continue;
        }

        if (data.rotation.w < -1.0f || data.rotation.w > 1.0f)
        {
            TC_LOG_ERROR("sql.sql", "Table `gameobject` have gameobject (GUID: %u Entry: %u) with invalid rotationW (%f) value, skip", guid, data.id, data.rotation.w);
            continue;
        }

        if (!MapManager::IsValidMapCoord(data.mapid, data.posX, data.posY, data.posZ, data.orientation))
        {
            TC_LOG_ERROR("sql.sql", "Table `gameobject` has gameobject (GUID: %u Entry: %u) with invalid coordinates, skip", guid, data.id);
            continue;
        }

        if (data.phaseMask == 0)
        {
            TC_LOG_ERROR("sql.sql", "Table `gameobject` has gameobject (GUID: %u Entry: %u) with `phaseMask`=0 (not visible for anyone), set to 1.", guid, data.id);
            data.phaseMask = 1;
        }

        if (data.phaseGroup && GetPhasesForGroup(data.phaseGroup).empty())
        {
            TC_LOG_ERROR("sql.sql", "Table `gameobject` has gameobject (GUID: %u Entry: %u) with non-existing `phasegroup` (%u) set, `phasegroup` set to 0", guid, data.id, data.phaseGroup);
            data.phaseGroup = 0;
        }

        if (data.phaseGroup && data.phaseid)
        {
            TC_LOG_ERROR("sql.sql", "Table `gameobject` have gameobject (GUID: %u Entry: %u) with both `phaseid` and `phasegroup` set, `phasegroup` set to 0", guid, data.id);
            data.phaseGroup = 0;
        }

        if (sWorld->getBoolConfig(CONFIG_CALCULATE_GAMEOBJECT_ZONE_AREA_DATA))
        {
            uint32 zoneId = sMapMgr->GetZoneId(data.phaseMask, data.mapid, data.posX, data.posY, data.posZ);
            uint32 areaId = sMapMgr->GetAreaId(data.phaseMask, data.mapid, data.posX, data.posY, data.posZ);

            WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_UPD_GAMEOBJECT_ZONE_AREA_DATA);

            stmt->setUInt32(0, zoneId);
            stmt->setUInt32(1, areaId);
            stmt->setUInt64(2, guid);

            WorldDatabase.Execute(stmt);
        }

        if (gInfo->type == GAMEOBJECT_TYPE_TRANSPORT)
            sTransportMgr->AddLocalTransportSpawn(data.mapid, data.spawnMask, guid);
        else if (gameEvent == 0 && PoolId == 0)                      // if not this is to be managed by GameEvent System or Pool system or Transport system
            AddGameobjectToGrid(guid, &data);
        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %lu gameobjects in %u ms", (unsigned long)_gameObjectDataStore.size(), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::AddGameobjectToGrid(uint32 guid, GameObjectData const* data)
{
    uint16 mask = data->spawnMask;
    for (uint16 i = 0; mask != 0; i++, mask >>= 1)
    {
        if (mask & 1)
        {
            CellCoord cellCoord = Trinity::ComputeCellCoord(data->posX, data->posY);
            CellObjectGuids& cell_guids = _mapObjectGuidsStore[MAKE_PAIR32(data->mapid, i)][cellCoord.GetId()];
            cell_guids.gameobjects.insert(guid);
        }
    }
}

void ObjectMgr::RemoveGameobjectFromGrid(uint32 guid, GameObjectData const* data)
{
    uint16 mask = data->spawnMask;
    for (uint16 i = 0; mask != 0; i++, mask >>= 1)
    {
        if (mask & 1)
        {
            CellCoord cellCoord = Trinity::ComputeCellCoord(data->posX, data->posY);
            CellObjectGuids& cell_guids = _mapObjectGuidsStore[MAKE_PAIR32(data->mapid, i)][cellCoord.GetId()];
            cell_guids.gameobjects.erase(guid);
        }
    }
}

Player* ObjectMgr::GetPlayerByLowGUID(uint32 lowguid) const
{
    ObjectGuid guid(HighGuid::Player, lowguid);
    return ObjectAccessor::FindPlayer(guid);
}

// name must be checked to correctness (if received) before call this function
ObjectGuid ObjectMgr::GetPlayerGUIDByName(std::string const& name) const
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_GUID_BY_NAME);

    stmt->setString(0, name);

    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (result)
        return ObjectGuid(HighGuid::Player, (*result)[0].GetUInt32());

    return ObjectGuid::Empty;
}

bool ObjectMgr::GetPlayerNameByGUID(ObjectGuid guid, std::string& name) const
{
    // prevent DB access for online player
    if (Player* player = ObjectAccessor::FindPlayer(guid))
    {
        name = player->GetName();
        return true;
    }

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_NAME);

    stmt->setUInt32(0, guid.GetCounter());

    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (result)
    {
        name = (*result)[0].GetString();
        return true;
    }

    return false;
}

uint32 ObjectMgr::GetPlayerTeamByGUID(ObjectGuid guid) const
{
    // prevent DB access for online player
    if (Player* player = ObjectAccessor::FindPlayer(guid))
    {
        return Player::TeamForRace(player->GetRace());
    }

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_RACE);

    stmt->setUInt32(0, guid.GetCounter());

    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (result)
    {
        uint8 race = (*result)[0].GetUInt8();
        return Player::TeamForRace(race);
    }

    return 0;
}

uint32 ObjectMgr::GetPlayerAccountIdByGUID(ObjectGuid guid) const
{
    // prevent DB access for online player
    if (Player* player = ObjectAccessor::FindPlayer(guid))
    {
        return player->GetSession()->GetAccountId();
    }

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_ACCOUNT_BY_GUID);

    stmt->setUInt32(0, guid.GetCounter());

    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (result)
    {
        uint32 acc = (*result)[0].GetUInt32();
        return acc;
    }

    return 0;
}

uint32 ObjectMgr::GetPlayerAccountIdByPlayerName(const std::string& name) const
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_ACCOUNT_BY_NAME);

    stmt->setString(0, name);

    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (result)
    {
        uint32 acc = (*result)[0].GetUInt32();
        return acc;
    }

    return 0;
}

void ObjectMgr::LoadItemLocales()
{
    uint32 oldMSTime = getMSTime();

    _itemLocaleStore.clear();                                 // need for reload case

    //                                               0   1       2     3
    QueryResult result = WorldDatabase.Query("SELECT ID, locale, Name, Description FROM item_template_locale");
    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();

        uint32 id               = fields[0].GetUInt32();
        std::string localeName  = fields[1].GetString();

        LocaleConstant locale = GetLocaleByName(localeName);
        if (!sWorld->getBoolConfig(CONFIG_LOAD_LOCALES) || locale == LOCALE_enUS)
            continue;

        ItemLocale& data = _itemLocaleStore[id];
        AddLocaleString(fields[2].GetString(), locale, data.Name);
        AddLocaleString(fields[3].GetString(), locale, data.Description);
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u Item locale strings in %u ms", uint32(_itemLocaleStore.size()), GetMSTimeDiffToNow(oldMSTime));
}

void FillItemDamageFields(float* minDamage, float* maxDamage, float* dps, uint32 itemLevel, uint32 itemClass, uint32 itemSubClass, uint32 quality, uint32 delay, float statScalingFactor, uint32 inventoryType, uint32 flags2)
{
    *minDamage = *maxDamage = *dps = 0.0f;
    if (itemClass != ITEM_CLASS_WEAPON || quality > ITEM_QUALITY_ARTIFACT)
        return;

    DBCStorage<ItemDamageEntry>* store = NULL;
    // get the right store here
    if (inventoryType > 0xD + 13)
        return;

    switch (inventoryType)
    {
        case INVTYPE_AMMO:
            store = &sItemDamageAmmoStore;
            break;
        case INVTYPE_2HWEAPON:
            if (flags2 & ITEM_FLAGS_EXTRA_CASTER_WEAPON)
                store = &sItemDamageTwoHandCasterStore;
            else
                store = &sItemDamageTwoHandStore;
            break;
        case INVTYPE_RANGED:
        case INVTYPE_THROWN:
        case INVTYPE_RANGEDRIGHT:
            switch (itemSubClass)
            {
                case ITEM_SUBCLASS_WEAPON_WAND:
                    store = &sItemDamageWandStore;
                    break;
                case ITEM_SUBCLASS_WEAPON_THROWN:
                    store = &sItemDamageThrownStore;
                    break;
                case ITEM_SUBCLASS_WEAPON_BOW:
                case ITEM_SUBCLASS_WEAPON_GUN:
                case ITEM_SUBCLASS_WEAPON_CROSSBOW:
                    store = &sItemDamageRangedStore;
                    break;
                default:
                    return;
            }
            break;
        case INVTYPE_WEAPON:
        case INVTYPE_WEAPONMAINHAND:
        case INVTYPE_WEAPONOFFHAND:
            if (flags2 & ITEM_FLAGS_EXTRA_CASTER_WEAPON)
                store = &sItemDamageOneHandCasterStore;
            else
                store = &sItemDamageOneHandStore;
            break;
        default:
            return;
    }

    if (!store)
        return;

    ItemDamageEntry const* damageInfo = store->LookupEntry(itemLevel);
    if (!damageInfo)
        return;

    *dps = damageInfo->DPS[quality];
    float avgDamage = *dps * delay * 0.001f;
    *minDamage = (statScalingFactor * -0.5f + 1.0f) * avgDamage;
    *maxDamage = floor(float(avgDamage* (statScalingFactor * 0.5f + 1.0f) + 0.5f));
}

uint32 FillItemArmor(uint32 itemlevel, uint32 itemClass, uint32 itemSubclass, uint32 quality, uint32 inventoryType)
{
    if (quality > ITEM_QUALITY_ARTIFACT)
        return 0;

    if (itemClass != ITEM_CLASS_ARMOR)
        return 0;

    // all items but shields...and buklers? I dunno, 11 bucklers in dbc.
    if (itemSubclass != ITEM_SUBCLASS_ARMOR_SHIELD && itemSubclass != ITEM_SUBCLASS_ARMOR_COSMETIC)
    {
        ItemArmorQualityEntry const* armorQuality = sItemArmorQualityStore.LookupEntry(itemlevel);
        ItemArmorTotalEntry const* armorTotal = sItemArmorTotalStore.LookupEntry(itemlevel);
        if (!armorQuality || !armorTotal)
            return 0;

        if (inventoryType == INVTYPE_ROBE)
            inventoryType = INVTYPE_CHEST;

        ArmorLocationEntry const* location = sArmorLocationStore.LookupEntry(inventoryType);
        if (!location)
            return 0;

        if (itemSubclass < ITEM_SUBCLASS_ARMOR_CLOTH)
            return 0;

        ASSERT(itemSubclass <= ITEM_SUBCLASS_ARMOR_PLATE);
        return uint32(armorQuality->Value[quality] * armorTotal->Value[itemSubclass - 1] * location->Value[itemSubclass - 1] + 0.5f);
    }

    // shields
    ItemArmorShieldEntry const* shield = sItemArmorShieldStore.LookupEntry(itemlevel);
    if (!shield)
        return 0;

    return uint32(shield->Value[quality] + 0.5f);
}

uint32 FillMaxDurability(uint32 itemClass, uint32 itemSubClass, uint32 inventoryType, uint32 quality, uint32 itemLevel)
{
    if (itemClass != ITEM_CLASS_ARMOR && itemClass != ITEM_CLASS_WEAPON)
        return 0;

    static float const qualityMultipliers[MAX_ITEM_QUALITY] =
    {
        1.0f, 1.0f, 1.0f, 1.17f, 1.37f, 1.68f, 0.0f, 0.0f
    };

    static float const armorMultipliers[MAX_INVTYPE] =
    {
        0.00f, // INVTYPE_NON_EQUIP
        0.59f, // INVTYPE_HEAD
        0.00f, // INVTYPE_NECK
        0.59f, // INVTYPE_SHOULDERS
        0.00f, // INVTYPE_BODY
        1.00f, // INVTYPE_CHEST
        0.35f, // INVTYPE_WAIST
        0.75f, // INVTYPE_LEGS
        0.49f, // INVTYPE_FEET
        0.35f, // INVTYPE_WRISTS
        0.35f, // INVTYPE_HANDS
        0.00f, // INVTYPE_FINGER
        0.00f, // INVTYPE_TRINKET
        0.00f, // INVTYPE_WEAPON
        1.00f, // INVTYPE_SHIELD
        0.00f, // INVTYPE_RANGED
        0.00f, // INVTYPE_CLOAK
        0.00f, // INVTYPE_2HWEAPON
        0.00f, // INVTYPE_BAG
        0.00f, // INVTYPE_TABARD
        1.00f, // INVTYPE_ROBE
        0.00f, // INVTYPE_WEAPONMAINHAND
        0.00f, // INVTYPE_WEAPONOFFHAND
        0.00f, // INVTYPE_HOLDABLE
        0.00f, // INVTYPE_AMMO
        0.00f, // INVTYPE_THROWN
        0.00f, // INVTYPE_RANGEDRIGHT
        0.00f, // INVTYPE_QUIVER
        0.00f, // INVTYPE_RELIC
    };

    static float const weaponMultipliers[MAX_ITEM_SUBCLASS_WEAPON] =
    {
        0.89f, // ITEM_SUBCLASS_WEAPON_AXE
        1.03f, // ITEM_SUBCLASS_WEAPON_AXE2
        0.77f, // ITEM_SUBCLASS_WEAPON_BOW
        0.77f, // ITEM_SUBCLASS_WEAPON_GUN
        0.89f, // ITEM_SUBCLASS_WEAPON_MACE
        1.03f, // ITEM_SUBCLASS_WEAPON_MACE2
        1.03f, // ITEM_SUBCLASS_WEAPON_POLEARM
        0.89f, // ITEM_SUBCLASS_WEAPON_SWORD
        1.03f, // ITEM_SUBCLASS_WEAPON_SWORD2
        0.00f, // ITEM_SUBCLASS_WEAPON_Obsolete
        1.03f, // ITEM_SUBCLASS_WEAPON_STAFF
        0.00f, // ITEM_SUBCLASS_WEAPON_EXOTIC
        0.00f, // ITEM_SUBCLASS_WEAPON_EXOTIC2
        0.64f, // ITEM_SUBCLASS_WEAPON_FIST_WEAPON
        0.00f, // ITEM_SUBCLASS_WEAPON_MISCELLANEOUS
        0.64f, // ITEM_SUBCLASS_WEAPON_DAGGER
        0.64f, // ITEM_SUBCLASS_WEAPON_THROWN
        0.00f, // ITEM_SUBCLASS_WEAPON_SPEAR
        0.77f, // ITEM_SUBCLASS_WEAPON_CROSSBOW
        0.64f, // ITEM_SUBCLASS_WEAPON_WAND
        0.64f, // ITEM_SUBCLASS_WEAPON_FISHING_POLE
    };

    float levelPenalty = 1.0f;
    if (itemLevel <= 28)
        levelPenalty = 0.966f - float(28u - itemLevel) / 54.0f;

    if (itemClass == ITEM_CLASS_ARMOR)
    {
        if (inventoryType > INVTYPE_ROBE)
            return 0;

        return 5 * uint32(23.0f * qualityMultipliers[quality] * armorMultipliers[inventoryType] * levelPenalty + 0.5f);
    }

    return 5 * uint32(17.0f * qualityMultipliers[quality] * weaponMultipliers[itemSubClass] * levelPenalty + 0.5f);
};

void FillDisenchantFields(uint32* disenchantID, uint32* requiredDisenchantSkill, ItemTemplate const& itemTemplate)
{
    *disenchantID = 0;
    *(int32*)requiredDisenchantSkill = -1;
    if ((itemTemplate.Flags & (ITEM_PROTO_FLAG_CONJURED | ITEM_PROTO_FLAG_UNK6)) ||
        itemTemplate.Bonding == BIND_QUEST || itemTemplate.Area || itemTemplate.Map ||
        itemTemplate.Stackable > 1 ||
        itemTemplate.Quality < ITEM_QUALITY_UNCOMMON || itemTemplate.Quality > ITEM_QUALITY_EPIC ||
        !(itemTemplate.Class == ITEM_CLASS_ARMOR || itemTemplate.Class == ITEM_CLASS_WEAPON) ||
        !(Item::GetSpecialPrice(&itemTemplate) || sItemCurrencyCostStore.LookupEntry(itemTemplate.ItemId)))
        return;

    for (uint32 i = 0; i < sItemDisenchantLootStore.GetNumRows(); ++i)
    {
        ItemDisenchantLootEntry const* disenchant = sItemDisenchantLootStore.LookupEntry(i);
        if (!disenchant)
            continue;

        if (disenchant->ItemClass == itemTemplate.Class &&
            disenchant->ItemQuality == itemTemplate.Quality &&
            disenchant->MinItemLevel <= itemTemplate.ItemLevel &&
            disenchant->MaxItemLevel >= itemTemplate.ItemLevel)
        {
            if (disenchant->Id == 60 || disenchant->Id == 61)   // epic item disenchant ilvl range 66-99 (classic)
            {
                if (itemTemplate.RequiredLevel > 60 || itemTemplate.RequiredSkillRank > 300)
                    continue;                                   // skip to epic item disenchant ilvl range 90-199 (TBC)
            }
            else if (disenchant->Id == 66 || disenchant->Id == 67)  // epic item disenchant ilvl range 90-199 (TBC)
            {
                if (itemTemplate.RequiredLevel <= 60 || (itemTemplate.RequiredSkill && itemTemplate.RequiredSkillRank <= 300))
                    continue;
            }

            *disenchantID = disenchant->Id;
            *requiredDisenchantSkill = disenchant->RequiredDisenchantSkill;
            return;
        }
    }
}

void ObjectMgr::LoadItemTemplates()
{
    uint32 oldMSTime = getMSTime();
    uint32 sparseCount = 0;
    uint32 dbCount = 0;

    for (uint32 itemId = 0; itemId < sItemSparseStore.GetNumRows(); ++itemId)
    {
        ItemSparseEntry const* sparse = sItemSparseStore.LookupEntry(itemId);
        ItemEntry const* db2Data = sItemStore.LookupEntry(itemId);
        if (!sparse || !db2Data)
            continue;

        ItemTemplate& itemTemplate = _itemTemplateStore[itemId];

        itemTemplate.ItemId = itemId;
        itemTemplate.Class = db2Data->Class;
        itemTemplate.SubClass = db2Data->SubClass;
        itemTemplate.SoundOverrideSubclass = db2Data->SoundOverrideSubclass;
        itemTemplate.Name1 = sparse->Name[GetDBCLocaleIndex()];
        itemTemplate.DisplayInfoID = db2Data->DisplayId;
        itemTemplate.Quality = sparse->Quality;
        itemTemplate.Flags = sparse->Flags;
        itemTemplate.Flags2 = sparse->Flags2;
        itemTemplate.Flags3 = sparse->Flags3;
        itemTemplate.Unk430_1 = sparse->Unk430_1;
        itemTemplate.Unk430_2 = sparse->Unk430_2;
        itemTemplate.BuyCount = std::max(sparse->BuyCount, 1u);
        itemTemplate.BuyPrice = sparse->BuyPrice;
        itemTemplate.SellPrice = sparse->SellPrice;
        itemTemplate.InventoryType = db2Data->InventoryType;
        itemTemplate.AllowableClass = sparse->AllowableClass;
        itemTemplate.AllowableRace = sparse->AllowableRace;
        itemTemplate.ItemLevel = sparse->ItemLevel;
        itemTemplate.RequiredLevel = sparse->RequiredLevel;
        itemTemplate.RequiredSkill = sparse->RequiredSkill;
        itemTemplate.RequiredSkillRank = sparse->RequiredSkillRank;
        itemTemplate.RequiredSpell = sparse->RequiredSpell;
        itemTemplate.RequiredHonorRank = sparse->RequiredHonorRank;
        itemTemplate.RequiredCityRank = sparse->RequiredCityRank;
        itemTemplate.RequiredReputationFaction = sparse->RequiredReputationFaction;
        itemTemplate.RequiredReputationRank = sparse->RequiredReputationRank;
        itemTemplate.MaxCount = sparse->MaxCount;
        itemTemplate.Stackable = sparse->Stackable;
        itemTemplate.ContainerSlots = sparse->ContainerSlots;

        for (uint32 i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
        {
            itemTemplate.ItemStat[i].ItemStatType = sparse->ItemStatType[i];
            itemTemplate.ItemStat[i].ItemStatValue = sparse->ItemStatValue[i];
            itemTemplate.ItemStat[i].ItemStatAllocation = sparse->ItemStatAllocation[i];
            itemTemplate.ItemStat[i].ItemStatSocketCostMultiplier = sparse->ItemStatSocketCostMultiplier[i];
        }

        itemTemplate.ScalingStatDistribution = sparse->ScalingStatDistribution;

        // cache item damage
        FillItemDamageFields(&itemTemplate.DamageMin, &itemTemplate.DamageMax, &itemTemplate.DPS, sparse->ItemLevel,
                             db2Data->Class, db2Data->SubClass, sparse->Quality, sparse->Delay, sparse->StatScalingFactor,
                             sparse->InventoryType, sparse->Flags2);

        itemTemplate.DamageType = sparse->DamageType;
        itemTemplate.Armor = FillItemArmor(sparse->ItemLevel, db2Data->Class, db2Data->SubClass, sparse->Quality, sparse->InventoryType);
        itemTemplate.Delay = sparse->Delay;
        itemTemplate.RangedModRange = sparse->RangedModRange;

        for (uint32 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
        {
            itemTemplate.Spells[i].SpellId = sparse->SpellId[i];
            itemTemplate.Spells[i].SpellTrigger = sparse->SpellTrigger[i];
            itemTemplate.Spells[i].SpellCharges = sparse->SpellCharges[i];
            itemTemplate.Spells[i].SpellCooldown = sparse->SpellCooldown[i];
            itemTemplate.Spells[i].SpellCategory = sparse->SpellCategory[i];
            itemTemplate.Spells[i].SpellCategoryCooldown = sparse->SpellCategoryCooldown[i];
        }

        itemTemplate.SpellPPMRate = 0.0f;
        itemTemplate.Bonding = sparse->Bonding;
        itemTemplate.Description = sparse->Description[sWorld->GetDefaultDbcLocale()];
        itemTemplate.PageText = sparse->PageText;
        itemTemplate.LanguageID = sparse->LanguageID;
        itemTemplate.PageMaterial = sparse->PageMaterial;
        itemTemplate.StartQuest = sparse->StartQuest;
        itemTemplate.LockID = sparse->LockID;
        itemTemplate.Material = sparse->Material;
        itemTemplate.Sheath = sparse->Sheath;
        itemTemplate.RandomProperty = sparse->RandomProperty;
        itemTemplate.RandomSuffix = sparse->RandomSuffix;
        itemTemplate.ItemSet = sparse->ItemSet;
        itemTemplate.MaxDurability = FillMaxDurability(db2Data->Class, db2Data->SubClass, sparse->InventoryType, sparse->Quality, sparse->ItemLevel);
        itemTemplate.Area = sparse->Area;
        itemTemplate.Map = sparse->Map;
        itemTemplate.BagFamily = sparse->BagFamily;
        itemTemplate.TotemCategory = sparse->TotemCategory;

        for (uint32 i = 0; i < MAX_ITEM_PROTO_SOCKETS; ++i)
        {
            itemTemplate.Socket[i].Color = sparse->Color[i];
            itemTemplate.Socket[i].Content = sparse->Content[i];
        }

        itemTemplate.socketBonus = sparse->SocketBonus;
        itemTemplate.GemProperties = sparse->GemProperties;
        FillDisenchantFields(&itemTemplate.DisenchantID, &itemTemplate.RequiredDisenchantSkill, itemTemplate);

        itemTemplate.ArmorDamageModifier = sparse->ArmorDamageModifier;
        itemTemplate.Duration = sparse->Duration;
        itemTemplate.ItemLimitCategory = sparse->ItemLimitCategory;
        itemTemplate.HolidayId = sparse->HolidayId;
        itemTemplate.StatScalingFactor = sparse->StatScalingFactor;
        itemTemplate.CurrencySubstitutionId = sparse->CurrencySubstitutionId;
        itemTemplate.CurrencySubstitutionCount = sparse->CurrencySubstitutionCount;
        itemTemplate.ScriptId = 0;
        itemTemplate.FoodType = 0;
        itemTemplate.MinMoneyLoot = 0;
        itemTemplate.MaxMoneyLoot = 0;
        ++sparseCount;
    }

    // Load missing items from item_template AND overwrite data from Item-sparse.db2 (item_template is supposed to contain Item-sparse.adb data)
    //                                               0      1      2         3                      4     5          6        7      8           9       10        11        12        13        14
    QueryResult result = WorldDatabase.Query("SELECT entry, Class, SubClass, SoundOverrideSubclass, Name, DisplayId, Quality, Flags, FlagsExtra, Flags3, Unk430_1, Unk430_2, BuyCount, BuyPrice, SellPrice, "
    //                                        15             16              17             18         19             20             21                 22
                                             "InventoryType, AllowableClass, AllowableRace, ItemLevel, RequiredLevel, RequiredSkill, RequiredSkillRank, RequiredSpell, "
    //                                        23                 24                25                         26                      27        28         29
                                             "RequiredHonorRank, RequiredCityRank, RequiredReputationFaction, RequiredReputationRank, MaxCount, Stackable, ContainerSlots, "
    //                                        30          31           32           33           34          35           36           37
                                             "stat_type1, stat_value1, stat_unk1_1, stat_unk2_1, stat_type2, stat_value2, stat_unk1_2, stat_unk2_2, "
    //                                        38          39           40           41           42          43           44           45
                                             "stat_type3, stat_value3, stat_unk1_3, stat_unk2_3, stat_type4, stat_value4, stat_unk1_4, stat_unk2_4, "
    //                                        46          47           48           49           50          51           52           53
                                             "stat_type5, stat_value5, stat_unk1_5, stat_unk2_5, stat_type6, stat_value6, stat_unk1_6, stat_unk2_6, "
    //                                        54          55           56           57           58          59           60           61
                                             "stat_type7, stat_value7, stat_unk1_7, stat_unk2_7, stat_type8, stat_value8, stat_unk1_8, stat_unk2_8, "
    //                                        62          63           64           65           66           67            68            69
                                             "stat_type9, stat_value9, stat_unk1_9, stat_unk2_9, stat_type10, stat_value10, stat_unk1_10, stat_unk2_10, "
    //                                        70                       71          72     73
                                             "ScalingStatDistribution, DamageType, Delay, RangedModRange, "
    //                                        74         75              76              77               78               79
                                             "spellid_1, spelltrigger_1, spellcharges_1, spellcooldown_1, spellcategory_1, spellcategorycooldown_1, "
    //                                        80         81              82              83               84               85
                                             "spellid_2, spelltrigger_2, spellcharges_2, spellcooldown_2, spellcategory_2, spellcategorycooldown_2, "
    //                                        86         87              88              89               90               91
                                             "spellid_3, spelltrigger_3, spellcharges_3, spellcooldown_3, spellcategory_3, spellcategorycooldown_3, "
    //                                        92         93              94              95               96               97
                                             "spellid_4, spelltrigger_4, spellcharges_4, spellcooldown_4, spellcategory_4, spellcategorycooldown_4, "
    //                                        98         99              100             101              102              103
                                             "spellid_5, spelltrigger_5, spellcharges_5, spellcooldown_5, spellcategory_5, spellcategorycooldown_5, "
    //                                        104      105          106       107         108           109         110     111
                                             "Bonding, Description, PageText, LanguageID, PageMaterial, StartQuest, LockID, Material, "
    //                                        112     113             114           115      116   117  118        119
                                             "Sheath, RandomProperty, RandomSuffix, ItemSet, Area, Map, BagFamily, TotemCategory, "
    //                                        120            121              122            123              124            125              126
                                             "SocketColor_1, SocketContent_1, SocketColor_2, SocketContent_2, SocketColor_3, SocketContent_3, SocketBonus, "
    //                                        127            128                  129       130                131        132
                                             "GemProperties, ArmorDamageModifier, Duration, ItemLimitCategory, HolidayId, StatScalingFactor, "
    //                                        133                     134
                                             "CurrencySubstitutionId, CurrencySubstitutionCount "
                                             "FROM item_template");

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 itemId = fields[0].GetUInt32();
            if (_itemTemplateStore.find(itemId) != _itemTemplateStore.end())
                --sparseCount;

            ItemTemplate& itemTemplate = _itemTemplateStore[itemId];

            itemTemplate.ItemId                    = itemId;
            itemTemplate.Class                     = uint32(fields[1].GetUInt8());
            itemTemplate.SubClass                  = uint32(fields[2].GetUInt8());
            itemTemplate.SoundOverrideSubclass     = fields[3].GetInt32();
            itemTemplate.Name1                     = fields[4].GetString();
            itemTemplate.DisplayInfoID             = fields[5].GetUInt32();
            itemTemplate.Quality                   = uint32(fields[6].GetUInt8());
            itemTemplate.Flags                     = fields[7].GetUInt32();
            itemTemplate.Flags2                    = fields[8].GetUInt32();
            itemTemplate.Flags3                    = fields[9].GetUInt32();
            itemTemplate.Unk430_1                  = fields[10].GetFloat();
            itemTemplate.Unk430_2                  = fields[11].GetFloat();
            itemTemplate.BuyCount                  = uint32(fields[12].GetUInt8());
            itemTemplate.BuyPrice                  = int32(fields[13].GetInt64());
            itemTemplate.SellPrice                 = fields[14].GetUInt32();
            itemTemplate.InventoryType             = uint32(fields[15].GetUInt8());
            itemTemplate.AllowableClass            = fields[16].GetInt32();
            itemTemplate.AllowableRace             = fields[17].GetInt32();
            itemTemplate.ItemLevel                 = uint32(fields[18].GetUInt16());
            itemTemplate.RequiredLevel             = uint32(fields[19].GetUInt8());
            itemTemplate.RequiredSkill             = uint32(fields[20].GetUInt16());
            itemTemplate.RequiredSkillRank         = uint32(fields[21].GetUInt16());
            itemTemplate.RequiredSpell             = fields[22].GetUInt32();
            itemTemplate.RequiredHonorRank         = fields[23].GetUInt32();
            itemTemplate.RequiredCityRank          = fields[24].GetUInt32();
            itemTemplate.RequiredReputationFaction = uint32(fields[25].GetUInt16());
            itemTemplate.RequiredReputationRank    = uint32(fields[26].GetUInt16());
            itemTemplate.MaxCount                  = fields[27].GetInt32();
            itemTemplate.Stackable                 = fields[28].GetInt32();
            itemTemplate.ContainerSlots            = uint32(fields[29].GetUInt8());

            for (uint32 i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
            {
                itemTemplate.ItemStat[i].ItemStatType  = uint32(fields[30 + i * 4 + 0].GetUInt8());
                itemTemplate.ItemStat[i].ItemStatValue = int32(fields[30 + i * 4 + 1].GetInt16());
                itemTemplate.ItemStat[i].ItemStatAllocation  = fields[30 + i * 4 + 2].GetInt32();
                itemTemplate.ItemStat[i].ItemStatSocketCostMultiplier  = fields[30 + i * 4 + 3].GetFloat();
            }

            itemTemplate.ScalingStatDistribution = uint32(fields[70].GetUInt16());

            // cache item damage
            FillItemDamageFields(&itemTemplate.DamageMin, &itemTemplate.DamageMax, &itemTemplate.DPS, itemTemplate.ItemLevel,
                                 itemTemplate.Class, itemTemplate.SubClass, itemTemplate.Quality, fields[72].GetUInt16(),
                                 fields[132].GetFloat(), itemTemplate.InventoryType, itemTemplate.Flags2);

            itemTemplate.DamageType                = fields[71].GetUInt8();
            itemTemplate.Armor                     = FillItemArmor(itemTemplate.ItemLevel, itemTemplate.Class,
                                                                   itemTemplate.SubClass, itemTemplate.Quality,
                                                                   itemTemplate.InventoryType);

            itemTemplate.Delay                     = fields[72].GetUInt16();
            itemTemplate.RangedModRange            = fields[73].GetFloat();

            for (uint32 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
            {
                itemTemplate.Spells[i].SpellId               = fields[74 + 6 * i + 0].GetInt32();
                itemTemplate.Spells[i].SpellTrigger          = uint32(fields[74 + 6 * i + 1].GetUInt8());
                itemTemplate.Spells[i].SpellCharges          = int32(fields[74 + 6 * i + 2].GetInt16());
                itemTemplate.Spells[i].SpellCooldown         = fields[74 + 6 * i + 3].GetInt32();
                itemTemplate.Spells[i].SpellCategory         = uint32(fields[74 + 6 * i + 4].GetUInt16());
                itemTemplate.Spells[i].SpellCategoryCooldown = fields[74 + 6 * i + 5].GetInt32();
            }

            itemTemplate.SpellPPMRate   = 0.0f;
            itemTemplate.Bonding        = uint32(fields[104].GetUInt8());
            itemTemplate.Description    = fields[105].GetString();
            itemTemplate.PageText       = fields[106].GetUInt32();
            itemTemplate.LanguageID     = uint32(fields[107].GetUInt8());
            itemTemplate.PageMaterial   = uint32(fields[108].GetUInt8());
            itemTemplate.StartQuest     = fields[109].GetUInt32();
            itemTemplate.LockID         = fields[110].GetUInt32();
            itemTemplate.Material       = int32(fields[111].GetInt8());
            itemTemplate.Sheath         = uint32(fields[112].GetUInt8());
            itemTemplate.RandomProperty = fields[113].GetUInt32();
            itemTemplate.RandomSuffix   = fields[114].GetInt32();
            itemTemplate.ItemSet        = fields[115].GetUInt32();
            itemTemplate.MaxDurability  = FillMaxDurability(itemTemplate.Class, itemTemplate.SubClass,
                itemTemplate.InventoryType, itemTemplate.Quality, itemTemplate.ItemLevel);

            itemTemplate.Area           = fields[116].GetUInt32();
            itemTemplate.Map            = uint32(fields[117].GetUInt16());
            itemTemplate.BagFamily      = fields[118].GetUInt32();
            itemTemplate.TotemCategory  = fields[119].GetUInt32();

            for (uint32 i = 0; i < MAX_ITEM_PROTO_SOCKETS; ++i)
            {
                itemTemplate.Socket[i].Color   = uint32(fields[120 + i*2].GetUInt8());
                itemTemplate.Socket[i].Content = fields[120 + i * 2 + 1].GetUInt32();
            }

            itemTemplate.socketBonus         = fields[126].GetUInt32();
            itemTemplate.GemProperties       = fields[127].GetUInt32();
            FillDisenchantFields(&itemTemplate.DisenchantID, &itemTemplate.RequiredDisenchantSkill, itemTemplate);

            itemTemplate.ArmorDamageModifier       = fields[128].GetFloat();
            itemTemplate.Duration                  = fields[129].GetUInt32();
            itemTemplate.ItemLimitCategory         = uint32(fields[130].GetInt16());
            itemTemplate.HolidayId                 = fields[131].GetUInt32();
            itemTemplate.StatScalingFactor         = fields[132].GetFloat();
            itemTemplate.CurrencySubstitutionId    = fields[133].GetInt32();
            itemTemplate.CurrencySubstitutionCount = fields[134].GetInt32();
            itemTemplate.ScriptId                  = 0;
            itemTemplate.FoodType                  = 0;
            itemTemplate.MinMoneyLoot              = 0;
            itemTemplate.MaxMoneyLoot              = 0;
            ++dbCount;
        } while (result->NextRow());
    }

    // Check if item templates for DBC referenced character start outfit are present
    std::set<uint32> notFoundOutfit;
    for (uint32 i = 1; i < sCharStartOutfitStore.GetNumRows(); ++i)
    {
        CharStartOutfitEntry const* entry = sCharStartOutfitStore.LookupEntry(i);
        if (!entry)
            continue;

        for (int j = 0; j < MAX_OUTFIT_ITEMS; ++j)
        {
            if (entry->ItemId[j] <= 0)
                continue;

            uint32 item_id = entry->ItemId[j];

            if (!GetItemTemplate(item_id))
                notFoundOutfit.insert(item_id);
        }
    }

    sSpellMgr->LoadItemSpellsCorrections();
    db2::LoadBattlePetCorrections();

    for (std::set<uint32>::const_iterator itr = notFoundOutfit.begin(); itr != notFoundOutfit.end(); ++itr)
        TC_LOG_ERROR("sql.sql", "Item (Entry: %u) does not exist in `item_template` but is referenced in `CharStartOutfit.dbc`", *itr);

    TC_LOG_INFO("server.loading", ">> Loaded %u item templates from Item-sparse.db2 and %u from database in %u ms", sparseCount, dbCount, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadSpellAreaTriggerTemplates()
{
    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    _areaTriggerTemplateStore.clear();
    QueryResult result = WorldDatabase.Query("SELECT Entry, Flags, CollisionType, Radius, ScaleX, ScaleY, ScriptName FROM spell_areatrigger_template");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            AreaTriggerTemplate& atTemplate = _areaTriggerTemplateStore[fields[0].GetUInt32()];
            atTemplate.Entry = fields[0].GetUInt32();
            atTemplate.Flags = fields[1].GetUInt32();
            atTemplate.CollisionType = fields[2].GetUInt32();
            atTemplate.Radius = fields[3].GetFloat();
            atTemplate.ScaleX = fields[4].GetFloat();
            atTemplate.ScaleY = fields[5].GetFloat();
            atTemplate.ScriptId = GetScriptId(fields[6].GetString());
            ++count;
        } while (result->NextRow());
    }
    TC_LOG_INFO("server.loading", ">> Loaded %u area trigger templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadItemTemplateAddon()
{
    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    QueryResult result = WorldDatabase.Query("SELECT Id, FlagsCu, FoodType, MinMoneyLoot, MaxMoneyLoot, SpellPPMChance FROM item_template_addon");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 itemId = fields[0].GetUInt32();
            if (!GetItemTemplate(itemId))
            {
                TC_LOG_ERROR("sql.sql", "Item %u specified in `item_template_addon` does not exist, skipped.", itemId);
                continue;
            }

            uint32 minMoneyLoot = fields[3].GetUInt32();
            uint32 maxMoneyLoot = fields[4].GetUInt32();
            if (minMoneyLoot > maxMoneyLoot)
            {
                TC_LOG_ERROR("sql.sql", "Minimum money loot specified in `item_template_addon` for item %u was greater than maximum amount, swapping.", itemId);
                std::swap(minMoneyLoot, maxMoneyLoot);
            }
            ItemTemplate& itemTemplate = _itemTemplateStore[itemId];
            itemTemplate.FlagsCu = fields[1].GetUInt32();
            itemTemplate.FoodType = fields[2].GetUInt8();
            itemTemplate.MinMoneyLoot = minMoneyLoot;
            itemTemplate.MaxMoneyLoot = maxMoneyLoot;
            itemTemplate.SpellPPMRate = fields[5].GetFloat();
            ++count;
        } while (result->NextRow());
    }
    TC_LOG_INFO("server.loading", ">> Loaded %u item addon templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadItemScriptNames()
{
    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    QueryResult result = WorldDatabase.Query("SELECT Id, ScriptName FROM item_script_names");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 itemId = fields[0].GetUInt32();
            if (!GetItemTemplate(itemId))
            {
                TC_LOG_ERROR("sql.sql", "Item %u specified in `item_script_names` does not exist, skipped.", itemId);
                continue;
            }

            _itemTemplateStore[itemId].ScriptId = GetScriptId(fields[1].GetString());
            ++count;
        } while (result->NextRow());
    }

    TC_LOG_INFO("server.loading", ">> Loaded %u item script names in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}
ItemTemplate const* ObjectMgr::GetItemTemplate(uint32 entry)
{
    ItemTemplateContainer::const_iterator itr = _itemTemplateStore.find(entry);
    if (itr != _itemTemplateStore.end())
        return &(itr->second);
    return NULL;
}

void ObjectMgr::LoadVehicleTemplateAccessories()
{
    uint32 oldMSTime = getMSTime();

    _vehicleTemplateAccessoryStore.clear();                           // needed for reload case

    uint32 count = 0;

    //                                                  0             1              2          3           4             5
    QueryResult result = WorldDatabase.Query("SELECT `entry`, `accessory_entry`, `seat_id`, `minion`, `summontype`, `summontimer` FROM `vehicle_template_accessory`");

    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 vehicle template accessories. DB table `vehicle_template_accessory` is empty.");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 uiEntry      = fields[0].GetUInt32();
        uint32 uiAccessory  = fields[1].GetUInt32();
        int8   uiSeat       = int8(fields[2].GetInt8());
        bool   bMinion      = fields[3].GetBool();
        uint8  uiSummonType = fields[4].GetUInt8();
        uint32 uiSummonTimer= fields[5].GetUInt32();

        if (!sObjectMgr->GetCreatureTemplate(uiEntry))
        {
            TC_LOG_ERROR("sql.sql", "Table `vehicle_template_accessory`: creature template entry %u does not exist.", uiEntry);
            continue;
        }

        if (!sObjectMgr->GetCreatureTemplate(uiAccessory))
        {
            TC_LOG_ERROR("sql.sql", "Table `vehicle_template_accessory`: Accessory %u does not exist.", uiAccessory);
            continue;
        }

        if (_spellClickInfoStore.find(uiEntry) == _spellClickInfoStore.end())
        {
            TC_LOG_ERROR("sql.sql", "Table `vehicle_template_accessory`: creature template entry %u has no data in npc_spellclick_spells", uiEntry);
            continue;
        }

        _vehicleTemplateAccessoryStore[uiEntry].push_back(VehicleAccessory(uiAccessory, uiSeat, bMinion, uiSummonType, uiSummonTimer));

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u Vehicle Template Accessories in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadVehicleAccessories()
{
    uint32 oldMSTime = getMSTime();

    _vehicleAccessoryStore.clear();                           // needed for reload case

    uint32 count = 0;

    //                                                  0             1             2          3           4             5
    QueryResult result = WorldDatabase.Query("SELECT `guid`, `accessory_entry`, `seat_id`, `minion`, `summontype`, `summontimer` FROM `vehicle_accessory`");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 Vehicle Accessories in %u ms", GetMSTimeDiffToNow(oldMSTime));
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 uiGUID       = fields[0].GetUInt32();
        uint32 uiAccessory  = fields[1].GetUInt32();
        int8   uiSeat       = int8(fields[2].GetInt16());
        bool   bMinion      = fields[3].GetBool();
        uint8  uiSummonType = fields[4].GetUInt8();
        uint32 uiSummonTimer= fields[5].GetUInt32();

        if (!sObjectMgr->GetCreatureTemplate(uiAccessory))
        {
            TC_LOG_ERROR("sql.sql", "Table `vehicle_accessory`: Accessory %u does not exist.", uiAccessory);
            continue;
        }

        _vehicleAccessoryStore[uiGUID].push_back(VehicleAccessory(uiAccessory, uiSeat, bMinion, uiSummonType, uiSummonTimer));

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u Vehicle Accessories in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadPetLevelInfo()
{
    uint32 oldMSTime = getMSTime();

    //                                                 0               1      2   3     4    5    6    7     8    9
    QueryResult result = WorldDatabase.Query("SELECT creature_entry, level, hp, mana, str, agi, sta, inte, spi, armor FROM pet_levelstats");

    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 level pet stats definitions. DB table `pet_levelstats` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 creature_id = fields[0].GetUInt32();
        if (!sObjectMgr->GetCreatureTemplate(creature_id))
        {
            TC_LOG_ERROR("sql.sql", "Wrong creature id %u in `pet_levelstats` table, ignoring.", creature_id);
            continue;
        }

        uint32 current_level = fields[1].GetUInt8();
        if (current_level > sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
        {
            if (current_level > STRONG_MAX_LEVEL)        // hardcoded level maximum
                TC_LOG_ERROR("sql.sql", "Wrong (> %u) level %u in `pet_levelstats` table, ignoring.", STRONG_MAX_LEVEL, current_level);
            else
            {
                TC_LOG_INFO("misc", "Unused (> MaxPlayerLevel in worldserver.conf) level %u in `pet_levelstats` table, ignoring.", current_level);
                ++count;                                // make result loading percent "expected" correct in case disabled detail mode for example.
            }
            continue;
        }
        else if (current_level < 1)
        {
            TC_LOG_ERROR("sql.sql", "Wrong (<1) level %u in `pet_levelstats` table, ignoring.", current_level);
            continue;
        }

        PetLevelInfo*& pInfoMapEntry = _petInfoStore[creature_id];

        if (pInfoMapEntry == NULL)
            pInfoMapEntry = new PetLevelInfo[sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)];

        // data for level 1 stored in [0] array element, ...
        PetLevelInfo* pLevelInfo = &pInfoMapEntry[current_level-1];

        pLevelInfo->health = fields[2].GetUInt16();
        pLevelInfo->mana   = fields[3].GetUInt16();
        pLevelInfo->armor  = fields[9].GetUInt32();

        for (int i = 0; i < MAX_STATS; i++)
        {
            pLevelInfo->stats[i] = fields[i+4].GetUInt16();
        }

        ++count;
    }
    while (result->NextRow());

    // Fill gaps and check integrity
    for (PetLevelInfoContainer::iterator itr = _petInfoStore.begin(); itr != _petInfoStore.end(); ++itr)
    {
        PetLevelInfo* pInfo = itr->second;

        // fatal error if no level 1 data
        if (!pInfo || pInfo[0].health == 0)
        {
            TC_LOG_ERROR("sql.sql", "Creature %u does not have pet stats data for Level 1!", itr->first);
            exit(1);
        }

        // fill level gaps
        for (uint8 level = 1; level < sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL); ++level)
        {
            if (pInfo[level].health == 0)
            {
                TC_LOG_ERROR("sql.sql", "Creature %u has no data for Level %i pet stats data, using data of Level %i.", itr->first, level+1, level);
                pInfo[level] = pInfo[level-1];
            }
        }
    }

    TC_LOG_INFO("server.loading", ">> Loaded %u level pet stats definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

PetLevelInfo const* ObjectMgr::GetPetLevelInfo(uint32 creature_id, uint8 level) const
{
    if (level > sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
        level = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);

    PetLevelInfoContainer::const_iterator itr = _petInfoStore.find(creature_id);
    if (itr == _petInfoStore.end())
        return NULL;

    return &itr->second[level-1];                           // data for level 1 stored in [0] array element, ...
}

void ObjectMgr::PlayerCreateInfoAddItemHelper(uint32 race_, uint32 class_, uint32 itemId, int32 count)
{
    if (!_playerInfo[race_][class_])
        return;

    if (count > 0)
        _playerInfo[race_][class_]->item.push_back(PlayerCreateInfoItem(itemId, count));
    else
    {
        if (count < -1)
            TC_LOG_ERROR("sql.sql", "Invalid count %i specified on item %u be removed from original player create info (use -1)!", count, itemId);

        for (uint32 gender = 0; gender < GENDER_NONE; ++gender)
        {
            if (CharStartOutfitEntry const* entry = GetCharStartOutfitEntry(race_, class_, gender))
            {
                bool found = false;
                for (uint8 x = 0; x < MAX_OUTFIT_ITEMS; ++x)
                {
                    if (entry->ItemId[x] > 0 && uint32(entry->ItemId[x]) == itemId)
                    {
                        found = true;
                        const_cast<CharStartOutfitEntry*>(entry)->ItemId[x] = 0;
                        break;
                    }
                }

                if (!found)
                    TC_LOG_ERROR("sql.sql", "Item %u specified to be removed from original create info not found in dbc!", itemId);
            }
        }
    }
}

void ObjectMgr::LoadPlayerInfo()
{
    // Load playercreate
    {
        uint32 oldMSTime = getMSTime();
        //                                                0     1      2    3        4          5           6
        QueryResult result = WorldDatabase.Query("SELECT race, class, map, zone, position_x, position_y, position_z, orientation FROM playercreateinfo");

        if (!result)
        {
            TC_LOG_ERROR("server.loading", ">> Loaded 0 player create definitions. DB table `playercreateinfo` is empty.");
            exit(1);
        }
        else
        {
            uint32 count = 0;

            do
            {
                Field* fields = result->Fetch();

                uint32 current_race  = fields[0].GetUInt8();
                uint32 current_class = fields[1].GetUInt8();
                uint32 mapId         = fields[2].GetUInt16();
                uint32 areaId        = fields[3].GetUInt32(); // zone
                float  positionX     = fields[4].GetFloat();
                float  positionY     = fields[5].GetFloat();
                float  positionZ     = fields[6].GetFloat();
                float  orientation   = fields[7].GetFloat();

                if (current_race >= MAX_RACES)
                {
                    TC_LOG_ERROR("sql.sql", "Wrong race %u in `playercreateinfo` table, ignoring.", current_race);
                    continue;
                }

                ChrRacesEntry const* rEntry = sChrRacesStore.LookupEntry(current_race);
                if (!rEntry)
                {
                    TC_LOG_ERROR("sql.sql", "Wrong race %u in `playercreateinfo` table, ignoring.", current_race);
                    continue;
                }

                if (current_class >= MAX_CLASSES)
                {
                    TC_LOG_ERROR("sql.sql", "Wrong class %u in `playercreateinfo` table, ignoring.", current_class);
                    continue;
                }

                if (!sChrClassesStore.LookupEntry(current_class))
                {
                    TC_LOG_ERROR("sql.sql", "Wrong class %u in `playercreateinfo` table, ignoring.", current_class);
                    continue;
                }

                // accept DB data only for valid position (and non instanceable)
                if (!MapManager::IsValidMapCoord(mapId, positionX, positionY, positionZ, orientation))
                {
                    TC_LOG_ERROR("sql.sql", "Wrong home position for class %u race %u pair in `playercreateinfo` table, ignoring.", current_class, current_race);
                    continue;
                }

                if (sMapStore.LookupEntry(mapId)->Instanceable())
                {
                    TC_LOG_ERROR("sql.sql", "Home position in instanceable map for class %u race %u pair in `playercreateinfo` table, ignoring.", current_class, current_race);
                    continue;
                }

                PlayerInfo* info = new PlayerInfo();
                info->mapId = mapId;
                info->areaId = areaId;
                info->positionX = positionX;
                info->positionY = positionY;
                info->positionZ = positionZ;
                info->orientation = orientation;
                info->displayId_m = rEntry->model_m;
                info->displayId_f = rEntry->model_f;
                _playerInfo[current_race][current_class] = info;

                ++count;
            }
            while (result->NextRow());

            TC_LOG_INFO("server.loading", ">> Loaded %u player create definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
        }
    }

    // Load playercreate items
    TC_LOG_INFO("server.loading", "Loading Player Create Items Data...");
    {
        uint32 oldMSTime = getMSTime();
        //                                                0     1      2       3
        QueryResult result = WorldDatabase.Query("SELECT race, class, itemid, amount FROM playercreateinfo_item");

        if (!result)
        {
            TC_LOG_INFO("server.loading", ">> Loaded 0 custom player create items. DB table `playercreateinfo_item` is empty.");
        }
        else
        {
            uint32 count = 0;

            do
            {
                Field* fields = result->Fetch();

                uint32 current_race = fields[0].GetUInt8();
                if (current_race >= MAX_RACES)
                {
                    TC_LOG_ERROR("sql.sql", "Wrong race %u in `playercreateinfo_item` table, ignoring.", current_race);
                    continue;
                }

                uint32 current_class = fields[1].GetUInt8();
                if (current_class >= MAX_CLASSES)
                {
                    TC_LOG_ERROR("sql.sql", "Wrong class %u in `playercreateinfo_item` table, ignoring.", current_class);
                    continue;
                }

                uint32 item_id = fields[2].GetUInt32();

                if (!GetItemTemplate(item_id))
                {
                    TC_LOG_ERROR("sql.sql", "Item id %u (race %u class %u) in `playercreateinfo_item` table but not listed in `item_template`, ignoring.", item_id, current_race, current_class);
                    continue;
                }

                int32 amount   = fields[3].GetInt8();

                if (!amount)
                {
                    TC_LOG_ERROR("sql.sql", "Item id %u (class %u race %u) have amount == 0 in `playercreateinfo_item` table, ignoring.", item_id, current_race, current_class);
                    continue;
                }

                if (!current_race || !current_class)
                {
                    uint32 min_race = current_race ? current_race : 1;
                    uint32 max_race = current_race ? current_race + 1 : MAX_RACES;
                    uint32 min_class = current_class ? current_class : 1;
                    uint32 max_class = current_class ? current_class + 1 : MAX_CLASSES;
                    for (uint32 r = min_race; r < max_race; ++r)
                        for (uint32 c = min_class; c < max_class; ++c)
                            PlayerCreateInfoAddItemHelper(r, c, item_id, amount);
                }
                else
                    PlayerCreateInfoAddItemHelper(current_race, current_class, item_id, amount);

                ++count;
            }
            while (result->NextRow());

            TC_LOG_INFO("server.loading", ">> Loaded %u custom player create items in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
        }
    }

    // Load playercreate skills
    TC_LOG_INFO("server.loading", "Loading Player Create Skill Data...");
    {
        uint32 oldMSTime = getMSTime();

        QueryResult result = WorldDatabase.PQuery("SELECT raceMask, classMask, skill, `rank` FROM playercreateinfo_skills");

        if (!result)
        {
            TC_LOG_INFO("server.loading", ">> Loaded 0 player create skills. DB table `playercreateinfo_skills` is empty.");
        }
        else
        {
            uint32 count = 0;

            do
            {
                Field* fields = result->Fetch();
                uint32 raceMask = fields[0].GetUInt32();
                uint32 classMask = fields[1].GetUInt32();
                // PlayerCreateInfoSkill skill;
                // skill.SkillId = fields[2].GetUInt16();
                // skill.Rank = fields[3].GetUInt16();
                uint32 rank = fields[3].GetUInt16();
                uint32 skill = fields[2].GetUInt16();

                if (rank >= MAX_SKILL_STEP)
                {
                    TC_LOG_ERROR("sql.sql", "Skill rank value %hu set for skill %u raceMask %u classMask %u is too high, max allowed value is %d", rank, skill, raceMask, classMask, MAX_SKILL_STEP);
                    continue;
                }

                if (raceMask != 0 && !(raceMask & RACEMASK_ALL_PLAYABLE))
                {
                    TC_LOG_ERROR("sql.sql", "Wrong race mask %u in `playercreateinfo_skills` table, ignoring.", raceMask);
                    continue;
                }

                if (classMask != 0 && !(classMask & CLASSMASK_ALL_PLAYABLE))
                {
                    TC_LOG_ERROR("sql.sql", "Wrong class mask %u in `playercreateinfo_skills` table, ignoring.", classMask);
                    continue;
                }

                if (!sSkillLineStore.LookupEntry(skill))
                {
                    TC_LOG_ERROR("sql.sql", "Wrong skill id %u in `playercreateinfo_skills` table, ignoring.", skill);
                    continue;
                }

                for (uint32 raceIndex = RACE_HUMAN; raceIndex < MAX_RACES; ++raceIndex)
                {
                    if (raceMask == 0 || ((1 << (raceIndex - 1)) & raceMask))
                    {
                        for (uint32 classIndex = CLASS_WARRIOR; classIndex < MAX_CLASSES; ++classIndex)
                        {
                            if (classMask == 0 || ((1 << (classIndex - 1)) & classMask))
                            {
                                if (!GetSkillRaceClassInfo(skill, raceIndex, classIndex))
                                    continue;

                                if (auto& info = _playerInfo[raceIndex][classIndex])
                                {
                                    info->skills.push_back(skill);
                                    ++count;
                                }
                            }
                        }
                    }
                }
            } while (result->NextRow());

            TC_LOG_INFO("server.loading", ">> Loaded %u player create skills in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
        }
    }

    // Load playercreate spells
    TC_LOG_INFO("server.loading", "Loading Player Create Spell Data...");
    {
        uint32 oldMSTime = getMSTime();
        uint32 count = 0;

        for (uint32 i = 0; i < sSkillRaceClassInfoStore.GetNumRows(); ++i)
        {
            auto entry = sSkillRaceClassInfoStore.LookupEntry(i);
            if (!entry || entry->Availability != 1)
                continue;
            for (uint32 raceIndex = RACE_HUMAN; raceIndex < MAX_RACES; ++raceIndex)
            {
                if ((1 << (raceIndex - 1)) & entry->RaceMask)
                {
                    for (uint32 classIndex = CLASS_WARRIOR; classIndex < MAX_CLASSES; ++classIndex)
                    {
                        if ((1 << (classIndex - 1)) & entry->ClassMask)
                        {
                            if (PlayerInfo* info = _playerInfo[raceIndex][classIndex])
                            {
                                info->skills.push_back(entry->Id);
                                ++count;
                            }
                        }
                    }
                }
            }
        }
        TC_LOG_INFO("server.loading", ">> Loaded %u player create spells in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    }

    // Load playercreate actions
    TC_LOG_INFO("server.loading", "Loading Player Create Action Data...");
    {
        uint32 oldMSTime = getMSTime();

        //                                                0     1      2       3       4
        QueryResult result = WorldDatabase.Query("SELECT race, class, button, action, type FROM playercreateinfo_action");

        if (!result)
        {
            TC_LOG_ERROR("server.loading", ">> Loaded 0 player create actions. DB table `playercreateinfo_action` is empty.");
        }
        else
        {
            uint32 count = 0;

            do
            {
                Field* fields = result->Fetch();

                uint32 current_race = fields[0].GetUInt8();
                if (current_race >= MAX_RACES)
                {
                    TC_LOG_ERROR("sql.sql", "Wrong race %u in `playercreateinfo_action` table, ignoring.", current_race);
                    continue;
                }

                uint32 current_class = fields[1].GetUInt8();
                if (current_class >= MAX_CLASSES)
                {
                    TC_LOG_ERROR("sql.sql", "Wrong class %u in `playercreateinfo_action` table, ignoring.", current_class);
                    continue;
                }

                if (PlayerInfo* info = _playerInfo[current_race][current_class])
                    info->action.push_back(PlayerCreateInfoAction(fields[2].GetUInt16(), fields[3].GetUInt32(), fields[4].GetUInt16()));

                ++count;
            }
            while (result->NextRow());

            TC_LOG_INFO("server.loading", ">> Loaded %u player create actions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
        }
    }

    // Load playercreate spells
    // TC_LOG_INFO("server.loading", "Loading Player Create Spell Data...");
    // {
    //     uint32 oldMSTime = getMSTime();

    //     QueryResult result = WorldDatabase.PQuery("SELECT racemask, classmask, Spell FROM playercreateinfo_spell_custom");

    //     if (!result)
    //     {
    //         TC_LOG_INFO("server.loading", ">> Loaded 0 player create spells. DB table `playercreateinfo_spell_custom` is empty.");
    //     }
    //     else
    //     {
    //         uint32 count = 0;

    //         do
    //         {
    //             Field* fields = result->Fetch();
    //             uint32 raceMask = fields[0].GetUInt32();
    //             uint32 classMask = fields[1].GetUInt32();
    //             uint32 spellId = fields[2].GetUInt32();

    //             if (raceMask != 0 && !(raceMask & RACEMASK_ALL_PLAYABLE))
    //             {
    //                 TC_LOG_ERROR("sql.sql", "Wrong race mask %u in `playercreateinfo_spell_custom` table, ignoring.", raceMask);
    //                 continue;
    //             }

    //             if (classMask != 0 && !(classMask & CLASSMASK_ALL_PLAYABLE))
    //             {
    //                 TC_LOG_ERROR("sql.sql", "Wrong class mask %u in `playercreateinfo_spell_custom` table, ignoring.", classMask);
    //                 continue;
    //             }

    //             for (uint32 raceIndex = RACE_HUMAN; raceIndex < MAX_RACES; ++raceIndex)
    //             {
    //                 if (raceMask == 0 || ((1 << (raceIndex - 1)) & raceMask))
    //                 {
    //                     for (uint32 classIndex = CLASS_WARRIOR; classIndex < MAX_CLASSES; ++classIndex)
    //                     {
    //                         if (classMask == 0 || ((1 << (classIndex - 1)) & classMask))
    //                         {
    //                             if (auto& info = _playerInfo[raceIndex][classIndex])
    //                             {
    //                                 info->customSpells.push_back(spellId);
    //                                 ++count;
    //                             }
    //                             // We need something better here, the check is not accounting for spells used by multiple races/classes but not all of them.
    //                             // Either split the masks per class, or per race, which kind of kills the point yet.
    //                             // else if (raceMask != 0 && classMask != 0)
    //                             //     TC_LOG_ERROR("sql.sql", "Racemask/classmask (%u/%u) combination was found containing an invalid race/class combination (%u/%u) in `%s` (Spell %u), ignoring.", raceMask, classMask, raceIndex, classIndex, tableName.c_str(), spellId);
    //                         }
    //                     }
    //                 }
    //             }
    //         }
    //         while (result->NextRow());

    //         TC_LOG_INFO("server.loading", ">> Loaded %u custom player create spells in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    //     }
    // }

    TC_LOG_INFO("server.loading", "Loading Player Create Cast Spell Data...");
    {
        uint32 oldMSTime = getMSTime();

        QueryResult result = WorldDatabase.PQuery("SELECT raceMask, classMask, spell FROM playercreateinfo_cast_spell");

        if (!result)
            TC_LOG_INFO("server.loading", ">> Loaded 0 player create cast spells. DB table `playercreateinfo_cast_spell` is empty.");
        else
        {
            uint32 count = 0;

            do
            {
                Field* fields = result->Fetch();
                uint64 raceMask = fields[0].GetUInt64();
                uint32 classMask = fields[1].GetUInt32();
                uint32 spellId = fields[2].GetUInt32();

                if (raceMask != 0 && !(raceMask & RACEMASK_ALL_PLAYABLE))
                {
                    TC_LOG_ERROR("sql.sql", "Wrong race mask %u in `playercreateinfo_cast_spell` table, ignoring.", raceMask);
                    continue;
                }

                if (classMask != 0 && !(classMask & CLASSMASK_ALL_PLAYABLE))
                {
                    TC_LOG_ERROR("sql.sql", "Wrong class mask %u in `playercreateinfo_cast_spell` table, ignoring.", classMask);
                    continue;
                }

                for (uint32 raceIndex = RACE_HUMAN; raceIndex < MAX_RACES; ++raceIndex)
                {
                    if (raceMask == 0 || ((1 << (raceIndex - 1)) & raceMask))
                    {
                        for (uint32 classIndex = CLASS_WARRIOR; classIndex < MAX_CLASSES; ++classIndex)
                        {
                            if (classMask == 0 || ((1 << (classIndex - 1)) & classMask))
                            {
                                if (PlayerInfo* info = _playerInfo[raceIndex][classIndex])
                                {
                                    info->castSpells.push_back(spellId);
                                    ++count;
                                }
                            }
                        }
                    }
                }
            }  while (result->NextRow());
            
            TC_LOG_INFO("server.loading", ">> Loaded %u player create cast spells in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
        }
    }

    // Loading levels data (class/race dependent)
    TC_LOG_INFO("server.loading", "Loading Player Create Level Stats Data...");
    {
        uint32 oldMSTime = getMSTime();

        //                                                 0     1      2      3    4    5    6    7
        QueryResult result  = WorldDatabase.Query("SELECT race, class, level, str, agi, sta, inte, spi FROM player_levelstats");

        if (!result)
        {
            TC_LOG_ERROR("server.loading", ">> Loaded 0 level stats definitions. DB table `player_levelstats` is empty.");
            exit(1);
        }

        uint32 count = 0;

        do
        {
            Field* fields = result->Fetch();

            uint32 current_race = fields[0].GetUInt8();
            if (current_race >= MAX_RACES)
            {
                TC_LOG_ERROR("sql.sql", "Wrong race %u in `player_levelstats` table, ignoring.", current_race);
                continue;
            }

            uint32 current_class = fields[1].GetUInt8();
            if (current_class >= MAX_CLASSES)
            {
                TC_LOG_ERROR("sql.sql", "Wrong class %u in `player_levelstats` table, ignoring.", current_class);
                continue;
            }

            uint32 current_level = fields[2].GetUInt8();
            if (current_level > sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
            {
                if (current_level > STRONG_MAX_LEVEL)        // hardcoded level maximum
                    TC_LOG_ERROR("sql.sql", "Wrong (> %u) level %u in `player_levelstats` table, ignoring.", STRONG_MAX_LEVEL, current_level);
                else
                {
                    TC_LOG_INFO("misc", "Unused (> MaxPlayerLevel in worldserver.conf) level %u in `player_levelstats` table, ignoring.", current_level);
                    ++count;                                // make result loading percent "expected" correct in case disabled detail mode for example.
                }
                continue;
            }

            if (PlayerInfo* info = _playerInfo[current_race][current_class])
            {
                if (!info->levelInfo)
                    info->levelInfo = new PlayerLevelInfo[sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)];

                PlayerLevelInfo& levelInfo = info->levelInfo[current_level-1];
                for (int i = 0; i < MAX_STATS; i++)
                    levelInfo.stats[i] = fields[i+3].GetUInt8();
            }

            ++count;
        }
        while (result->NextRow());

        // Fill gaps and check integrity
        for (int race = 0; race < MAX_RACES; ++race)
        {
            // skip non existed races
            if (!sChrRacesStore.LookupEntry(race))
                continue;

            for (int class_ = 0; class_ < MAX_CLASSES; ++class_)
            {
                // skip non existed classes
                if (!sChrClassesStore.LookupEntry(class_))
                    continue;

                PlayerInfo* info = _playerInfo[race][class_];
                if (!info)
                    continue;

                // skip expansion races if not playing with expansion
                if (sWorld->getIntConfig(CONFIG_EXPANSION) < 1 && (race == RACE_BLOODELF || race == RACE_DRAENEI))
                    continue;

                // skip expansion classes if not playing with expansion
                if (sWorld->getIntConfig(CONFIG_EXPANSION) < 2 && class_ == CLASS_DEATH_KNIGHT)
                    continue;

                // skip expansion classes / races if not playing with expansion
                if (sWorld->getIntConfig(CONFIG_EXPANSION) < 4 && (class_ == CLASS_MONK || race == RACE_PANDAREN_NEUTRAL || race == RACE_PANDAREN_ALLIANCE || race == RACE_PANDAREN_HORDE))
                    continue;

                // fatal error if no level 1 data
                if (!info->levelInfo || info->levelInfo[0].stats[0] == 0)
                {
                    TC_LOG_ERROR("sql.sql", "Race %i Class %i Level 1 does not have stats data!", race, class_);
                    exit(1);
                }

                // fill level gaps
                for (uint8 level = 1; level < sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL); ++level)
                {
                    if (info->levelInfo[level].stats[0] == 0)
                    {
                        TC_LOG_ERROR("sql.sql", "Race %i Class %i Level %i does not have stats data. Using stats data of level %i.", race, class_, level+1, level);
                        info->levelInfo[level] = info->levelInfo[level-1];
                    }
                }
            }
        }

        TC_LOG_INFO("server.loading", ">> Loaded %u level stats definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    }

    // Loading xp per level data
    TC_LOG_INFO("server.loading", "Loading Player Create XP Data...");
    {
        uint32 oldMSTime = getMSTime();

        _playerXPperLevel.resize(sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL));
        for (uint8 level = 0; level < sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL); ++level)
            _playerXPperLevel[level] = 0;

        //                                                 0    1
        QueryResult result  = WorldDatabase.Query("SELECT lvl, xp_for_next_level FROM player_xp_for_level");

        if (!result)
        {
            TC_LOG_ERROR("server.loading", ">> Loaded 0 xp for level definitions. DB table `player_xp_for_level` is empty.");
            exit(1);
        }

        uint32 count = 0;

        do
        {
            Field* fields = result->Fetch();

            uint32 current_level = fields[0].GetUInt8();
            uint32 current_xp    = fields[1].GetUInt32();

            if (current_level >= sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
            {
                if (current_level > STRONG_MAX_LEVEL)        // hardcoded level maximum
                    TC_LOG_ERROR("sql.sql", "Wrong (> %u) level %u in `player_xp_for_level` table, ignoring.", STRONG_MAX_LEVEL, current_level);
                else
                {
                    TC_LOG_INFO("misc", "Unused (> MaxPlayerLevel in worldserver.conf) level %u in `player_xp_for_level` table, ignoring.", current_level);
                    ++count;                                // make result loading percent "expected" correct in case disabled detail mode for example.
                }
                continue;
            }
            //PlayerXPperLevel
            _playerXPperLevel[current_level] = current_xp;
            ++count;
        }
        while (result->NextRow());

        // fill level gaps
        for (uint8 level = 1; level < sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL); ++level)
        {
            if (_playerXPperLevel[level] == 0)
            {
                TC_LOG_ERROR("sql.sql", "Level %i does not have XP for level data. Using data of level [%i] + 100.", level+1, level);
                _playerXPperLevel[level] = _playerXPperLevel[level-1]+100;
            }
        }

        TC_LOG_INFO("server.loading", ">> Loaded %u xp for level definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    }
}

void ObjectMgr::GetPlayerClassLevelInfo(uint32 class_, uint8 level, uint32& baseHP, uint32& baseMana) const
{
    if (level < 1 || class_ >= MAX_CLASSES)
        return;

    if (level > sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
        level = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);

    GtOCTBaseHPByClassEntry const* hp = sGtOCTBaseHPByClassStore.LookupEntry((class_-1) * GT_MAX_LEVEL + level-1);
    GtOCTBaseMPByClassEntry const* mp = sGtOCTBaseMPByClassStore.LookupEntry((class_-1) * GT_MAX_LEVEL + level-1);

    if (!hp || !mp)
    {
        TC_LOG_ERROR("misc", "Tried to get non-existant Class-Level combination data for base hp/mp. Class %u Level %u", class_, level);
        return;
    }

    baseHP = uint32(hp->ratio);
    baseMana = uint32(mp->ratio);
}

void ObjectMgr::GetPlayerLevelInfo(uint32 race, uint32 class_, uint8 level, PlayerLevelInfo* info) const
{
    if (level < 1 || race >= MAX_RACES || class_ >= MAX_CLASSES)
        return;

    PlayerInfo const* pInfo = _playerInfo[race][class_];
    if (!pInfo)
        return;

    if (level <= sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
        *info = pInfo->levelInfo[level-1];
    else
        BuildPlayerLevelInfo(race, class_, level, info);
}

void ObjectMgr::BuildPlayerLevelInfo(uint8 race, uint8 _class, uint8 level, PlayerLevelInfo* info) const
{
    // base data (last known level)
    *info = _playerInfo[race][_class]->levelInfo[sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)-1];

    // if conversion from uint32 to uint8 causes unexpected behaviour, change lvl to uint32
    for (uint8 lvl = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)-1; lvl < level; ++lvl)
    {
        switch (_class)
        {
            case CLASS_WARRIOR:
                info->stats[STAT_STRENGTH]  += (lvl > 23 ? 2: (lvl > 1  ? 1: 0));
                info->stats[STAT_STAMINA]   += (lvl > 23 ? 2: (lvl > 1  ? 1: 0));
                info->stats[STAT_AGILITY]   += (lvl > 36 ? 1: (lvl > 6 && (lvl%2) ? 1: 0));
                info->stats[STAT_INTELLECT] += (lvl > 9 && !(lvl%2) ? 1: 0);
                info->stats[STAT_SPIRIT]    += (lvl > 9 && !(lvl%2) ? 1: 0);
                break;
            case CLASS_PALADIN:
                info->stats[STAT_STRENGTH]  += (lvl > 3  ? 1: 0);
                info->stats[STAT_STAMINA]   += (lvl > 33 ? 2: (lvl > 1 ? 1: 0));
                info->stats[STAT_AGILITY]   += (lvl > 38 ? 1: (lvl > 7 && !(lvl%2) ? 1: 0));
                info->stats[STAT_INTELLECT] += (lvl > 6 && (lvl%2) ? 1: 0);
                info->stats[STAT_SPIRIT]    += (lvl > 7 ? 1: 0);
                break;
            case CLASS_HUNTER:
                info->stats[STAT_STRENGTH]  += (lvl > 4  ? 1: 0);
                info->stats[STAT_STAMINA]   += (lvl > 4  ? 1: 0);
                info->stats[STAT_AGILITY]   += (lvl > 33 ? 2: (lvl > 1 ? 1: 0));
                info->stats[STAT_INTELLECT] += (lvl > 8 && (lvl%2) ? 1: 0);
                info->stats[STAT_SPIRIT]    += (lvl > 38 ? 1: (lvl > 9 && !(lvl%2) ? 1: 0));
                break;
            case CLASS_ROGUE:
                info->stats[STAT_STRENGTH]  += (lvl > 5  ? 1: 0);
                info->stats[STAT_STAMINA]   += (lvl > 4  ? 1: 0);
                info->stats[STAT_AGILITY]   += (lvl > 16 ? 2: (lvl > 1 ? 1: 0));
                info->stats[STAT_INTELLECT] += (lvl > 8 && !(lvl%2) ? 1: 0);
                info->stats[STAT_SPIRIT]    += (lvl > 38 ? 1: (lvl > 9 && !(lvl%2) ? 1: 0));
                break;
            case CLASS_PRIEST:
                info->stats[STAT_STRENGTH]  += (lvl > 9 && !(lvl%2) ? 1: 0);
                info->stats[STAT_STAMINA]   += (lvl > 5  ? 1: 0);
                info->stats[STAT_AGILITY]   += (lvl > 38 ? 1: (lvl > 8 && (lvl%2) ? 1: 0));
                info->stats[STAT_INTELLECT] += (lvl > 22 ? 2: (lvl > 1 ? 1: 0));
                info->stats[STAT_SPIRIT]    += (lvl > 3  ? 1: 0);
                break;
            case CLASS_SHAMAN:
                info->stats[STAT_STRENGTH]  += (lvl > 34 ? 1: (lvl > 6 && (lvl%2) ? 1: 0));
                info->stats[STAT_STAMINA]   += (lvl > 4 ? 1: 0);
                info->stats[STAT_AGILITY]   += (lvl > 7 && !(lvl%2) ? 1: 0);
                info->stats[STAT_INTELLECT] += (lvl > 5 ? 1: 0);
                info->stats[STAT_SPIRIT]    += (lvl > 4 ? 1: 0);
                break;
            case CLASS_MAGE:
                info->stats[STAT_STRENGTH]  += (lvl > 9 && !(lvl%2) ? 1: 0);
                info->stats[STAT_STAMINA]   += (lvl > 5  ? 1: 0);
                info->stats[STAT_AGILITY]   += (lvl > 9 && !(lvl%2) ? 1: 0);
                info->stats[STAT_INTELLECT] += (lvl > 24 ? 2: (lvl > 1 ? 1: 0));
                info->stats[STAT_SPIRIT]    += (lvl > 33 ? 2: (lvl > 2 ? 1: 0));
                break;
            case CLASS_WARLOCK:
                info->stats[STAT_STRENGTH]  += (lvl > 9 && !(lvl%2) ? 1: 0);
                info->stats[STAT_STAMINA]   += (lvl > 38 ? 2: (lvl > 3 ? 1: 0));
                info->stats[STAT_AGILITY]   += (lvl > 9 && !(lvl%2) ? 1: 0);
                info->stats[STAT_INTELLECT] += (lvl > 33 ? 2: (lvl > 2 ? 1: 0));
                info->stats[STAT_SPIRIT]    += (lvl > 38 ? 2: (lvl > 3 ? 1: 0));
                break;
            case CLASS_MONK:
                info->stats[STAT_STRENGTH]  += (lvl > 38 ? 2 : (lvl > 6 && (lvl % 2) ? 1 : 0));
                info->stats[STAT_STAMINA]   += (lvl > 32 ? 2 : (lvl > 4 ? 1 : 0));
                info->stats[STAT_AGILITY]   += (lvl > 38 ? 2 : (lvl > 8 && (lvl % 2) ? 1 : 0));
                info->stats[STAT_INTELLECT] += (lvl > 38 ? 3 : (lvl > 4 ? 1 : 0));
                info->stats[STAT_SPIRIT]    += (lvl > 38 ? 3 : (lvl > 5 ? 1 : 0));
                break;
            case CLASS_DRUID:
                info->stats[STAT_STRENGTH]  += (lvl > 38 ? 2: (lvl > 6 && (lvl%2) ? 1: 0));
                info->stats[STAT_STAMINA]   += (lvl > 32 ? 2: (lvl > 4 ? 1: 0));
                info->stats[STAT_AGILITY]   += (lvl > 38 ? 2: (lvl > 8 && (lvl%2) ? 1: 0));
                info->stats[STAT_INTELLECT] += (lvl > 38 ? 3: (lvl > 4 ? 1: 0));
                info->stats[STAT_SPIRIT]    += (lvl > 38 ? 3: (lvl > 5 ? 1: 0));
        }
    }
}

void ObjectMgr::LoadQuests()
{
    uint32 oldMSTime = getMSTime();

    // For reload case
    for (QuestMap::const_iterator itr = _questTemplates.begin(); itr != _questTemplates.end(); ++itr)
        delete itr->second;

    _questTemplates.clear();
    _questObjectives.clear();

    _exclusiveQuestGroups.clear();

    QueryResult result = WorldDatabase.Query("SELECT "
         //0  1          2           3         4               5            6            7                  8                9
         "ID, QuestType, QuestLevel, MinLevel, QuestPackageID, QuestSortID, QuestInfoID, SuggestedGroupNum, RewardNextQuest, RewardXPDifficulty, "
         //10          11                12                  13           14           15               16
         "RewardMoney, RewardBonusMoney, RewardDisplaySpell, RewardSpell, RewardHonor, RewardKillHonor, StartItem, "
         //17    18
         "Flags, FlagsEx, "
         //19          20             21         22                 23           24             25         26
         "RewardItem1, RewardAmount1, ItemDrop1, ItemDropQuantity1, RewardItem2, RewardAmount2, ItemDrop2, ItemDropQuantity2, "
         //27          28             29         30                 31           32             33         34
         "RewardItem3, RewardAmount3, ItemDrop3, ItemDropQuantity3, RewardItem4, RewardAmount4, ItemDrop4, ItemDropQuantity4, "
         //35                  36                         37                   38
         "RewardChoiceItemID1, RewardChoiceItemQuantity1, RewardChoiceItemID2, RewardChoiceItemQuantity2, "
         //39                  40                         41                   42
         "RewardChoiceItemID3, RewardChoiceItemQuantity3, RewardChoiceItemID4, RewardChoiceItemQuantity4, "
         //43                  44                         45                   46
         "RewardChoiceItemID5, RewardChoiceItemQuantity5, RewardChoiceItemID6, RewardChoiceItemQuantity6, "
         //47           48    49    50           51           52                 53                 54
         "POIContinent, POIx, POIy, POIPriority, RewardTitle, RewardArenaPoints, RewardSkillLineID, RewardNumSkillUps, "
         //55            56
         "PortraitGiver, PortraitTurnIn, "
         //57               58                   59                      60                61                   62
         "RewardFactionID1, RewardFactionValue1, RewardFactionOverride1, RewardFactionID2, RewardFactionValue2, RewardFactionOverride2, "
         //63               64                   65                      66                67                   68
         "RewardFactionID3, RewardFactionValue3, RewardFactionOverride3, RewardFactionID4, RewardFactionValue4, RewardFactionOverride4, "
         //69               70                   71                      72
         "RewardFactionID5, RewardFactionValue5, RewardFactionOverride5, RewardFactionFlags, "
         //73                74                  75                 76                  77                 78                  79                 80
         "RewardCurrencyID1, RewardCurrencyQty1, RewardCurrencyID2, RewardCurrencyQty2, RewardCurrencyID3, RewardCurrencyQty3, RewardCurrencyID4, RewardCurrencyQty4, "
         //81                 82                  83           84           85
         "AcceptedSoundKitID, CompleteSoundKitID, AreaGroupID, TimeAllowed, AllowableRaces, "
         //86       87              88                89               90                 91                 92                  93                  94
         "LogTitle, LogDescription, QuestDescription, AreaDescription, PortraitGiverText, PortraitGiverName, PortraitTurnInText, PortraitTurnInName, QuestCompletionLog "
         "FROM quest_template");

    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 quests definitions. DB table `quest_template` is empty.");
        return;
    }

    // create multimap previous quest for each existed quest
    // some quests can have many previous maps set by NextQuestId in previous quest
    // for example set of race quests can lead to single not race specific quest
    do
    {
        Field* fields = result->Fetch();

        Quest* newQuest = new Quest(fields);
        _questTemplates[newQuest->GetQuestId()] = newQuest;
    } while (result->NextRow());

    struct QuestLoaderHelper
    {
        typedef void(Quest::* QuestLoaderFunction)(Field* fields);

        char const* QueryFields;
        char const* TableName;
        char const* QueryExtra;
        char const* TableDesc;
        QuestLoaderFunction LoaderFunction;
    };

    // QuestID needs to be fields[0]
    QuestLoaderHelper const QuestLoaderHelpers[] =
    {
        // 0   1       2       3       4       5            6            7            8
        { "ID, Emote1, Emote2, Emote3, Emote4, EmoteDelay1, EmoteDelay2, EmoteDelay3, EmoteDelay4",                                                                       "quest_details",        "",                                       "details",             &Quest::LoadQuestDetails       },

        // 0   1                2                  3
        { "ID, EmoteOnComplete, EmoteOnIncomplete, CompletionText",                                                                                                       "quest_request_items",  "",                                       "request items",       &Quest::LoadQuestRequestItems  },

        // 0   1       2       3       4       5            6            7            8            9
        { "ID, Emote1, Emote2, Emote3, Emote4, EmoteDelay1, EmoteDelay2, EmoteDelay3, EmoteDelay4, RewardText",                                                           "quest_offer_reward",   "",                                       "reward emotes",       &Quest::LoadQuestOfferReward   },

        // 0   1         2                 3              4            5            6               7                     8
        { "ID, MaxLevel, AllowableClasses, SourceSpellID, PrevQuestID, NextQuestID, ExclusiveGroup, RewardMailTemplateID, RewardMailDelay,"
         // 9               10                   11                     12                     13                   14                   15                 16
         " RequiredSkillID, RequiredSkillPoints, RequiredMinRepFaction, RequiredMaxRepFaction, RequiredMinRepValue, RequiredMaxRepValue, ProvidedItemCount, SpecialFlags,"
         // 17
         " ScriptName",                                                                                                                                                    "quest_template_addon", "",                                       "template addons",     &Quest::LoadQuestTemplateAddon },

        // 0           1      2        3         4            5          6         7
        { "qo.QuestID, qo.ID, qo.Type, qo.Index, qo.ObjectID, qo.Amount, qo.Flags, qo.Description",                                                                        "quest_objective qo",  "ORDER BY qo.Index ASC",                   "quest objectives",    &Quest::LoadQuestObjective     },
    };

    for (QuestLoaderHelper const& loader : QuestLoaderHelpers)
    {
        result = WorldDatabase.PQuery("SELECT %s FROM %s %s", loader.QueryFields, loader.TableName, loader.QueryExtra);

        if (!result)
            TC_LOG_INFO("server.loading", ">> Loaded 0 quest %s. DB table `%s` is empty.", loader.TableDesc, loader.TableName);
        else
        {
            do
            {
                Field* fields = result->Fetch();
                uint32 questId = fields[0].GetUInt32();

                auto itr = _questTemplates.find(questId);
                if (itr != _questTemplates.end())
                    (itr->second->*loader.LoaderFunction)(fields);
                else
                    TC_LOG_ERROR("server.loading", "Table `%s` has data for quest %u but such quest does not exist", loader.TableName, questId);
            } while (result->NextRow());
        }
    }

    std::map<uint32, uint32> usedMailTemplates;

    // Post processing
    for (auto& questPair : _questTemplates)
    {
        // skip post-loading checks for disabled quests
        if (DisableMgr::IsDisabledFor(DISABLE_TYPE_QUEST, questPair.first, nullptr))
            continue;

        Quest* qinfo = questPair.second;

        // additional quest integrity checks (GO, creature_template and item_template must be loaded already)

        if (qinfo->GetQuestType() >= MAX_DB_ALLOWED_QUEST_TYPES)
            TC_LOG_ERROR("sql.sql", "Quest %u has `Method` = %u, expected values are 0, 1 or 2.", qinfo->GetQuestId(), qinfo->GetQuestType());

        if (qinfo->_specialFlags & ~QUEST_SPECIAL_FLAGS_DB_ALLOWED)
        {
            TC_LOG_ERROR("sql.sql", "Quest %u has `SpecialFlags` = %u > max allowed value. Correct `SpecialFlags` to value <= %u",
                qinfo->GetQuestId(), qinfo->_specialFlags, QUEST_SPECIAL_FLAGS_DB_ALLOWED);
            qinfo->_specialFlags &= QUEST_SPECIAL_FLAGS_DB_ALLOWED;
        }

        if (qinfo->_flags & QUEST_FLAGS_DAILY && qinfo->_flags & QUEST_FLAGS_WEEKLY)
        {
            TC_LOG_ERROR("sql.sql", "Weekly Quest %u is marked as daily quest in `Flags`, removed daily flag.", qinfo->GetQuestId());
            qinfo->_flags &= ~QUEST_FLAGS_DAILY;
        }

        if (qinfo->_flags & QUEST_FLAGS_DAILY)
        {
            if (!(qinfo->_specialFlags & QUEST_SPECIAL_FLAGS_REPEATABLE))
            {
                TC_LOG_ERROR("sql.sql", "Daily Quest %u not marked as repeatable in `SpecialFlags`, added.", qinfo->GetQuestId());
                qinfo->_specialFlags |= QUEST_SPECIAL_FLAGS_REPEATABLE;
            }
        }

        if (qinfo->_flags & QUEST_FLAGS_WEEKLY)
        {
            if (!(qinfo->_specialFlags & QUEST_SPECIAL_FLAGS_REPEATABLE))
            {
                TC_LOG_ERROR("sql.sql", "Weekly Quest %u not marked as repeatable in `SpecialFlags`, added.", qinfo->GetQuestId());
                qinfo->_specialFlags |= QUEST_SPECIAL_FLAGS_REPEATABLE;
            }
        }

        if (qinfo->_specialFlags & QUEST_SPECIAL_FLAGS_MONTHLY)
        {
            if (!(qinfo->_specialFlags & QUEST_SPECIAL_FLAGS_REPEATABLE))
            {
                TC_LOG_ERROR("sql.sql", "Monthly quest %u not marked as repeatable in `SpecialFlags`, added.", qinfo->GetQuestId());
                qinfo->_specialFlags |= QUEST_SPECIAL_FLAGS_REPEATABLE;
            }
        }

        if (qinfo->_flags & QUEST_FLAGS_TRACKING_EVENT)
        {
            // at auto-reward can be rewarded only RewardChoiceItemId[0]
            for (int j = 1; j < QUEST_REWARD_CHOICES_COUNT; ++j )
            {
                if (uint32 id = qinfo->RewardChoiceItemId[j])
                {
                    TC_LOG_ERROR("sql.sql", "Quest %u has `RewardChoiceItemId%d` = %u but item from `RewardChoiceItemId%d` can't be rewarded with quest flag QUEST_FLAGS_TRACKING_EVENT.",
                        qinfo->GetQuestId(), j+1, id, j+1);
                    // no changes, quest ignore this data
                }
            }
        }

        if (qinfo->_minLevel == uint32(-1) || qinfo->_minLevel > DEFAULT_MAX_LEVEL)
        {
            TC_LOG_ERROR("sql.sql", "Quest %u should be disabled because `MinLevel` = %i", qinfo->GetQuestId(), int32(qinfo->_minLevel));
            // no changes needed, sending -1 in SMSG_QUEST_QUERY_RESPONSE is valid
        }

        // client quest log visual (area case)
        if (qinfo->_questSortID > 0)
        {
            if (!sAreaTableStore.LookupEntry(qinfo->_questSortID))
            {
                TC_LOG_ERROR("sql.sql", "Quest %u has `QuestSortID` = %u (zone case) but zone with this id does not exist.",
                    qinfo->GetQuestId(), qinfo->_questSortID);
                // no changes, quest not dependent from this value but can have problems at client
            }
        }
        // client quest log visual (sort case)
        if (qinfo->_questSortID < 0)
        {
            QuestSortEntry const* qSort = sQuestSortStore.LookupEntry(-int32(qinfo->_questSortID));
            if (!qSort)
            {
                TC_LOG_ERROR("sql.sql", "Quest %u has `QuestSortID` = %i (sort case) but quest sort with this id does not exist.",
                    qinfo->GetQuestId(), qinfo->_questSortID);
                // no changes, quest not dependent from this value but can have problems at client (note some may be 0, we must allow this so no check)
            }
            //check for proper RequiredSkillId value (skill case)
            if (uint32 skill_id = SkillByQuestSort(-int32(qinfo->_questSortID)))
            {
                if (qinfo->_requiredSkillId != skill_id)
                {
                    TC_LOG_ERROR("sql.sql", "Quest %u has `QuestSortID` = %i but `RequiredSkillId` does not have a corresponding value (%d).",
                        qinfo->GetQuestId(), qinfo->_questSortID, skill_id);
                    //override, and force proper value here?
                }
            }
        }

        // RequiredClasses, can be 0/CLASSMASK_ALL_PLAYABLE to allow any class
        if (qinfo->_allowableClasses)
        {
            if (!(qinfo->_allowableClasses & CLASSMASK_ALL_PLAYABLE))
            {
                TC_LOG_ERROR("sql.sql", "Quest %u does not contain any playable classes in `AllowableClasses` (%u), value set to 0 (all classes).", qinfo->GetQuestId(), qinfo->_allowableClasses);
                    qinfo->_allowableClasses = 0;
            }
        }
        // RequiredRaces, can be 0/RACEMASK_ALL_PLAYABLE to allow any race
        if (qinfo->_allowableRaces)
        {
            if (!(qinfo->_allowableRaces & RACEMASK_ALL_PLAYABLE))
            {
                TC_LOG_ERROR("sql.sql", "Quest %u does not contain any playable races in `AllowableRaces` (%u), value set to 0 (all races).", qinfo->GetQuestId(), qinfo->_allowableRaces);
                qinfo->_allowableRaces = 0;
            }
        }
        // RequiredSkillId, can be 0
        if (qinfo->_requiredSkillId)
        {
            if (!sSkillLineStore.LookupEntry(qinfo->_requiredSkillId))
            {
                TC_LOG_ERROR("sql.sql", "Quest %u has `RequiredSkillId` = %u but this skill does not exist",
                    qinfo->GetQuestId(), qinfo->_requiredSkillId);
            }
        }

        if (qinfo->_requiredSkillPoints)
        {
            if (qinfo->_requiredSkillPoints > sWorld->GetConfigMaxSkillValue())
            {
                TC_LOG_ERROR("sql.sql", "Quest %u has `RequiredSkillPoints` = %u but max possible skill is %u, quest can't be done.",
                    qinfo->GetQuestId(), qinfo->_requiredSkillPoints, sWorld->GetConfigMaxSkillValue());
                // no changes, quest can't be done for this requirement
            }
        }
        // else Skill quests can have 0 skill level, this is ok

        if (qinfo->_requiredMinRepFaction && !sFactionStore.LookupEntry(qinfo->_requiredMinRepFaction))
        {
            TC_LOG_ERROR("sql.sql", "Quest %u has `RequiredMinRepFaction` = %u but faction template %u does not exist, quest can't be done.",
                qinfo->GetQuestId(), qinfo->_requiredMinRepFaction, qinfo->_requiredMinRepFaction);
            // no changes, quest can't be done for this requirement
        }

        if (qinfo->_requiredMaxRepFaction && !sFactionStore.LookupEntry(qinfo->_requiredMaxRepFaction))
        {
            TC_LOG_ERROR("sql.sql", "Quest %u has `RequiredMaxRepFaction` = %u but faction template %u does not exist, quest can't be done.",
                qinfo->GetQuestId(), qinfo->_requiredMaxRepFaction, qinfo->_requiredMaxRepFaction);
            // no changes, quest can't be done for this requirement
        }

        if (qinfo->_requiredMinRepValue && qinfo->_requiredMinRepValue > ReputationMgr::Reputation_Cap)
        {
            TC_LOG_ERROR("sql.sql", "Quest %u has `RequiredMinRepValue` = %d but max reputation is %u, quest can't be done.",
                qinfo->GetQuestId(), qinfo->_requiredMinRepValue, ReputationMgr::Reputation_Cap);
            // no changes, quest can't be done for this requirement
        }

        if (qinfo->_requiredMinRepValue && qinfo->_requiredMaxRepValue && qinfo->_requiredMaxRepValue <= qinfo->_requiredMinRepValue)
        {
            TC_LOG_ERROR("sql.sql", "Quest %u has `RequiredMaxRepValue` = %d and `RequiredMinRepValue` = %d, quest can't be done.",
                qinfo->GetQuestId(), qinfo->_requiredMaxRepValue, qinfo->_requiredMinRepValue);
            // no changes, quest can't be done for this requirement
        }

        if (!qinfo->_requiredMinRepFaction && qinfo->_requiredMinRepValue != 0)
        {
            TC_LOG_ERROR("sql.sql", "Quest %u has `RequiredMinRepValue` = %d but `RequiredMinRepFaction` is 0, value has no effect",
                qinfo->GetQuestId(), qinfo->_requiredMinRepValue);
            // warning
        }

        if (!qinfo->_requiredMaxRepFaction && qinfo->_requiredMaxRepValue != 0)
        {
            TC_LOG_ERROR("sql.sql", "Quest %u has `RequiredMaxRepValue` = %d but `RequiredMaxRepFaction` is 0, value has no effect",
                qinfo->GetQuestId(), qinfo->_requiredMaxRepValue);
            // warning
        }

        if (qinfo->_rewardTitleId && !sCharTitlesStore.LookupEntry(qinfo->_rewardTitleId))
        {
            TC_LOG_ERROR("sql.sql", "Quest %u has `RewardTitleId` = %u but CharTitle Id %u does not exist, quest can't be rewarded with title.",
                qinfo->GetQuestId(), qinfo->_rewardTitleId, qinfo->_rewardTitleId);
            qinfo->_rewardTitleId = 0;
            // quest can't reward this title
        }

        if (qinfo->_sourceItemId)
        {
            if (!sObjectMgr->GetItemTemplate(qinfo->_sourceItemId))
            {
                TC_LOG_ERROR("sql.sql", "Quest %u has `SourceItemId` = %u but item with entry %u does not exist, quest can't be done.",
                    qinfo->GetQuestId(), qinfo->_sourceItemId, qinfo->_sourceItemId);
                qinfo->_sourceItemId = 0;                       // quest can't be done for this requirement
            }
            else if (qinfo->_sourceItemIdCount == 0)
            {
                TC_LOG_ERROR("sql.sql", "Quest %u has `StartItem` = %u but `ProvidedItemCount` = 0, set to 1 but need fix in DB.",
                             qinfo->GetQuestId(), qinfo->_sourceItemId);
                qinfo->_sourceItemIdCount = 1;                    // update to 1 for allow quest work for backward compatibility with DB
            }
        }
        else if (qinfo->_sourceItemIdCount > 0)
        {
            TC_LOG_ERROR("sql.sql", "Quest %u has `SourceItemId` = 0 but `SourceItemIdCount` = %u, useless value.",
                         qinfo->GetQuestId(), qinfo->_sourceItemIdCount);
            qinfo->_sourceItemIdCount = 0;                          // no quest work changes in fact
        }

        if (qinfo->_sourceSpellID)
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(qinfo->_sourceSpellID);
            if (!spellInfo)
            {
                TC_LOG_ERROR("sql.sql", "Quest %u has `SourceSpellid` = %u but spell %u doesn't exist, quest can't be done.",
                    qinfo->GetQuestId(), qinfo->_sourceSpellID, qinfo->_sourceSpellID);
                qinfo->_sourceSpellID = 0;                        // quest can't be done for this requirement
            }
            else if (!SpellMgr::IsSpellValid(spellInfo))
            {
                TC_LOG_ERROR("sql.sql", "Quest %u has `SourceSpellid` = %u but spell %u is broken, quest can't be done.",
                    qinfo->GetQuestId(), qinfo->_sourceSpellID, qinfo->_sourceSpellID);
                qinfo->_sourceSpellID = 0;                        // quest can't be done for this requirement
            }
        }

        for (QuestObjective const& obj : qinfo->GetObjectives())
        {
            // Store objective for lookup by id
            _questObjectives[obj.ID] = &obj;

            // Check storage index for objectives which store data
            if (obj.StorageIndex < 0)
            {
                switch (obj.Type)
                {
                    case QUEST_OBJECTIVE_MONSTER:
                    case QUEST_OBJECTIVE_ITEM:
                    case QUEST_OBJECTIVE_GAMEOBJECT:
                    case QUEST_OBJECTIVE_TALKTO:
                    case QUEST_OBJECTIVE_PLAYERKILLS:
                    case QUEST_OBJECTIVE_AREATRIGGER:
                    case QUEST_OBJECTIVE_WINPETBATTLEAGAINSTNPC:
                        TC_LOG_ERROR("sql.sql", "Quest %u objective %u has invalid StorageIndex = %u for objective type %u", qinfo->GetQuestId(), obj.ID, obj.StorageIndex, obj.Type);
                        break;
                    default:
                        break;
                }
            }

            switch (obj.Type)
            {
                case QUEST_OBJECTIVE_ITEM:
                    if (!GetItemTemplate(obj.ObjectID))
                        TC_LOG_ERROR("sql.sql", "Quest %u objective %u has non existing item entry %u, quest can't be done.",
                                     qinfo->GetQuestId(), obj.ID, obj.ObjectID);
                    break;
                case QUEST_OBJECTIVE_MONSTER:
                    if (!GetCreatureTemplate(obj.ObjectID))
                        TC_LOG_ERROR("sql.sql", "Quest %u objective %u has non existing creature entry %u, quest can't be done.",
                                     qinfo->GetQuestId(), obj.ID, uint32(obj.ObjectID));
                    break;
                case QUEST_OBJECTIVE_GAMEOBJECT:
                    if (!GetGameObjectTemplate(obj.ObjectID))
                        TC_LOG_ERROR("sql.sql", "Quest %u objective %u has non existing gameobject entry %u, quest can't be done.",
                                     qinfo->GetQuestId(), obj.ID, uint32(obj.ObjectID));
                    break;
                case QUEST_OBJECTIVE_TALKTO:
                    if (!GetCreatureTemplate(obj.ObjectID))
                        TC_LOG_ERROR("sql.sql", "Quest %u objective %u has non existing creature entry %u, quest can't be done.",
                                     qinfo->GetQuestId(), obj.ID, uint32(obj.ObjectID));
                    break;
                case QUEST_OBJECTIVE_MIN_REPUTATION:
                case QUEST_OBJECTIVE_MAX_REPUTATION:
                    if (!sFactionStore.LookupEntry(obj.ObjectID))
                        TC_LOG_ERROR("sql.sql", "Quest %u objective %u has non existing faction id %u", qinfo->GetQuestId(), obj.ID, obj.ObjectID);
                    break;
                case QUEST_OBJECTIVE_PLAYERKILLS:
                    if (obj.Amount <= 0)
                        TC_LOG_ERROR("sql.sql", "Quest %u objective %u has invalid player kills count %u", qinfo->GetQuestId(), obj.ID, obj.Amount);
                    break;
                case QUEST_OBJECTIVE_CURRENCY:
                    if (!sCurrencyTypesStore.LookupEntry(obj.ObjectID))
                        TC_LOG_ERROR("sql.sql", "Quest %u objective %u has non existing currency %u", qinfo->GetQuestId(), obj.ID, obj.ObjectID);
                    if (obj.Amount <= 0)
                        TC_LOG_ERROR("sql.sql", "Quest %u objective %u has invalid currency amount %u", qinfo->GetQuestId(), obj.ID, obj.Amount);
                    break;
                case QUEST_OBJECTIVE_LEARNSPELL:
                    if (!sSpellMgr->GetSpellInfo(obj.ObjectID))
                        TC_LOG_ERROR("sql.sql", "Quest %u objective %u has non existing spell id %u", qinfo->GetQuestId(), obj.ID, obj.ObjectID);
                    break;
                case QUEST_OBJECTIVE_WINPETBATTLEAGAINSTNPC:
                    if (obj.ObjectID && !GetCreatureTemplate(obj.ObjectID))
                        TC_LOG_ERROR("sql.sql", "Quest %u objective %u has non existing creature entry %u, quest can't be done.",
                                     qinfo->GetQuestId(), obj.ID, uint32(obj.ObjectID));
                    break;
                case QUEST_OBJECTIVE_DEFEATBATTLEPET:
                    if (!sBattlePetSpeciesStore.LookupEntry(obj.ObjectID))
                        TC_LOG_ERROR("sql.sql", "Quest %u objective %u has non existing battlepet species id %u", qinfo->GetQuestId(), obj.ID, obj.ObjectID);
                    break;
                case QUEST_OBJECTIVE_AREATRIGGER:
                    if (!sAreaTriggerStore.LookupEntry(uint32(obj.ObjectID)) && obj.ObjectID != -1)
                        TC_LOG_ERROR("sql.sql", "Quest %u objective %u has non existing AreaTrigger.db2 id %u", qinfo->GetQuestId(), obj.ID, obj.ObjectID);
                    break;
                case QUEST_OBJECTIVE_MONEY:
                case QUEST_OBJECTIVE_WINPVPPETBATTLES:
                    break;
                default:
                    TC_LOG_ERROR("sql.sql", "Quest %u objective %u has unhandled type %u", qinfo->GetQuestId(), obj.ID, obj.Type);
                    break;
            }
        }

        for (uint8 j = 0; j < QUEST_ITEM_DROP_COUNT; ++j)
        {
            uint32 id = qinfo->ItemDrop[j];
            if (id)
            {
                if (!sObjectMgr->GetItemTemplate(id))
                {
                    TC_LOG_ERROR("sql.sql", "Quest %u has `ItemDrop%d` = %u but item with entry %u does not exist, quest can't be done.",
                        qinfo->GetQuestId(), j+1, id, id);
                    // no changes, quest can't be done for this requirement
                }
            }
            else
            {
                if (qinfo->ItemDropQuantity[j]>0)
                {
                    TC_LOG_ERROR("sql.sql", "Quest %u has `ItemDrop%d` = 0 but `ItemDropQuantity%d` = %u.",
                        qinfo->GetQuestId(), j+1, j+1, qinfo->ItemDropQuantity[j]);
                    // no changes, quest ignore this data
                }
            }
        }

        for (uint8 j = 0; j < QUEST_REWARD_CHOICES_COUNT; ++j)
        {
            uint32 id = qinfo->RewardChoiceItemId[j];
            if (id)
            {
                if (!sObjectMgr->GetItemTemplate(id))
                {
                    TC_LOG_ERROR("sql.sql", "Quest %u has `RewardChoiceItemId%d` = %u but item with entry %u does not exist, quest will not reward this item.",
                        qinfo->GetQuestId(), j+1, id, id);
                    qinfo->RewardChoiceItemId[j] = 0;          // no changes, quest will not reward this
                }

                if (!qinfo->RewardChoiceItemCount[j])
                {
                    TC_LOG_ERROR("sql.sql", "Quest %u has `RewardChoiceItemId%d` = %u but `RewardChoiceItemCount%d` = 0, quest can't be done.",
                        qinfo->GetQuestId(), j+1, id, j+1);
                    // no changes, quest can't be done
                }
            }
            else if (qinfo->RewardChoiceItemCount[j]>0)
            {
                TC_LOG_ERROR("sql.sql", "Quest %u has `RewardChoiceItemId%d` = 0 but `RewardChoiceItemCount%d` = %u.",
                    qinfo->GetQuestId(), j+1, j+1, qinfo->RewardChoiceItemCount[j]);
                // no changes, quest ignore this data
            }
        }

        for (uint8 j = 0; j < QUEST_REWARD_ITEM_COUNT; ++j)
        {
            uint32 id = qinfo->RewardItemId[j];
            if (id)
            {
                if (!sObjectMgr->GetItemTemplate(id))
                {
                    TC_LOG_ERROR("sql.sql", "Quest %u has `RewardItemId%d` = %u but item with entry %u does not exist, quest will not reward this item.",
                        qinfo->GetQuestId(), j+1, id, id);
                    qinfo->RewardItemId[j] = 0;                // no changes, quest will not reward this item
                }

                if (!qinfo->RewardItemCount[j])
                {
                    TC_LOG_ERROR("sql.sql", "Quest %u has `RewardItemId%d` = %u but `RewardItemCount%d` = 0, quest will not reward this item.",
                        qinfo->GetQuestId(), j+1, id, j+1);
                    // no changes
                }
            }
            else if (qinfo->RewardItemCount[j]>0)
            {
                TC_LOG_ERROR("sql.sql", "Quest %u has `RewardItemId%d` = 0 but `RewardItemCount%d` = %u.",
                    qinfo->GetQuestId(), j+1, j+1, qinfo->RewardItemCount[j]);
                // no changes, quest ignore this data
            }
        }

        for (uint8 j = 0; j < QUEST_REWARD_REPUTATIONS_COUNT; ++j)
        {
            if (qinfo->RewardFactionId[j])
            {
                if (abs(qinfo->RewardFactionValue[j]) > 9)
                {
                    TC_LOG_ERROR("sql.sql", "Quest %u has RewardFactionValueId%d = %i. That is outside the range of valid values (-9 to 9).", qinfo->GetQuestId(), j+1, qinfo->RewardFactionValue[j]);
                }
                if (!sFactionStore.LookupEntry(qinfo->RewardFactionId[j]))
                {
                    TC_LOG_ERROR("sql.sql", "Quest %u has `RewardFactionId%d` = %u but raw faction (faction.dbc) %u does not exist, quest will not reward reputation for this faction.", qinfo->GetQuestId(), j+1, qinfo->RewardFactionId[j], qinfo->RewardFactionId[j]);
                    qinfo->RewardFactionId[j] = 0;            // quest will not reward this
                }
            }

            else if (qinfo->RewardFactionOverride[j] != 0)
            {
                TC_LOG_ERROR("sql.sql", "Quest %u has `RewardFactionId%d` = 0 but `RewardFactionOverride%d` = %i.",
                    qinfo->GetQuestId(), j+1, j+1, qinfo->RewardFactionOverride[j]);
                // no changes, quest ignore this data
            }
        }

        if (qinfo->_rewardSpell > 0)
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(qinfo->_rewardSpell);

            if (!spellInfo)
            {
                TC_LOG_ERROR("sql.sql", "Quest %u has `RewardSpell` = %u but spell %u does not exist, spell removed as display reward.",
                    qinfo->GetQuestId(), qinfo->_rewardSpell, qinfo->_rewardSpell);
                qinfo->_rewardSpell = 0;                        // no spell reward will display for this quest
            }

            else if (!SpellMgr::IsSpellValid(spellInfo))
            {
                TC_LOG_ERROR("sql.sql", "Quest %u has `RewardSpell` = %u but spell %u is broken, quest will not have a spell reward.",
                    qinfo->GetQuestId(), qinfo->_rewardSpell, qinfo->_rewardSpell);
                qinfo->_rewardSpell = 0;                        // no spell reward will display for this quest
            }

            else if (GetTalentSpellCost(qinfo->_rewardSpell))
            {
                TC_LOG_ERROR("sql.sql", "Quest %u has `RewardSpell` = %u but spell %u is talent, quest will not have a spell reward.",
                    qinfo->GetQuestId(), qinfo->_rewardSpell, qinfo->_rewardSpell);
                qinfo->_rewardSpell = 0;                        // no spell reward will display for this quest
            }
        }

        if (qinfo->_rewardMailTemplateId)
        {
            if (!sMailTemplateStore.LookupEntry(qinfo->_rewardMailTemplateId))
            {
                TC_LOG_ERROR("sql.sql", "Quest %u has `RewardMailTemplateId` = %u but mail template  %u does not exist, quest will not have a mail reward.",
                    qinfo->GetQuestId(), qinfo->_rewardMailTemplateId, qinfo->_rewardMailTemplateId);
                qinfo->_rewardMailTemplateId = 0;               // no mail will send to player
                qinfo->_rewardMailDelay = 0;                // no mail will send to player
            }
            else if (usedMailTemplates.find(qinfo->_rewardMailTemplateId) != usedMailTemplates.end())
            {
                std::map<uint32, uint32>::const_iterator used_mt_itr = usedMailTemplates.find(qinfo->_rewardMailTemplateId);
                TC_LOG_ERROR("sql.sql", "Quest %u has `RewardMailTemplateId` = %u but mail template  %u already used for quest %u, quest will not have a mail reward.",
                    qinfo->GetQuestId(), qinfo->_rewardMailTemplateId, qinfo->_rewardMailTemplateId, used_mt_itr->second);
                qinfo->_rewardMailTemplateId = 0;               // no mail will send to player
                qinfo->_rewardMailDelay = 0;                // no mail will send to player
            }
            else
                usedMailTemplates[qinfo->_rewardMailTemplateId] = qinfo->GetQuestId();
        }

        if (uint32 nextQuestInChain = qinfo->_nextQuestInChain)
        {
            if (!_questTemplates.count(nextQuestInChain))
            {
                TC_LOG_ERROR("sql.sql", "Quest %u has `NextQuestInChain` = %u but quest %u does not exist, quest chain will not work.",
                    qinfo->GetQuestId(), qinfo->_nextQuestInChain, qinfo->_nextQuestInChain);
                qinfo->_nextQuestInChain = 0;
            }
        }

        for (uint8 j = 0; j < QUEST_REWARD_CURRENCY_COUNT; ++j)
        {
            if (qinfo->RewardCurrencyId[j])
            {
                if (qinfo->RewardCurrencyCount[j] == 0)
                {
                    TC_LOG_ERROR("sql.sql", "Quest %u has `RewardCurrencyId%d` = %u but `RewardCurrencyCount%d` = 0, quest can't be done.",
                        qinfo->GetQuestId(), j+1, qinfo->RewardCurrencyId[j], j+1);
                    // no changes, quest can't be done for this requirement
                }

                if (!sCurrencyTypesStore.LookupEntry(qinfo->RewardCurrencyId[j]))
                {
                    TC_LOG_ERROR("sql.sql", "Quest %u has `RewardCurrencyId%d` = %u but currency with entry %u does not exist, quest can't be done.",
                        qinfo->GetQuestId(), j+1, qinfo->RewardCurrencyId[j], qinfo->RewardCurrencyId[j]);
                    qinfo->RewardCurrencyCount[j] = 0;             // prevent incorrect work of quest
                }
            }
            else if (qinfo->RewardCurrencyCount[j] > 0)
            {
                TC_LOG_ERROR("sql.sql", "Quest %u has `RewardCurrencyId%d` = 0 but `RewardCurrencyCount%d` = %u, quest can't be done.",
                    qinfo->GetQuestId(), j+1, j+1, qinfo->RewardCurrencyCount[j]);
                qinfo->RewardCurrencyCount[j] = 0;                 // prevent incorrect work of quest
            }
        }

        if (qinfo->_soundAccept)
        {
            if (!sSoundEntriesStore.LookupEntry(qinfo->_soundAccept))
            {
                TC_LOG_ERROR("sql.sql", "Quest %u has `SoundAccept` = %u but sound %u does not exist, set to 0.",
                    qinfo->GetQuestId(), qinfo->_soundAccept, qinfo->_soundAccept);
                qinfo->_soundAccept = 0;                        // no sound will be played
            }
        }

        if (qinfo->_soundTurnIn)
        {
            if (!sSoundEntriesStore.LookupEntry(qinfo->_soundTurnIn))
            {
                TC_LOG_ERROR("sql.sql", "Quest %u has `SoundTurnIn` = %u but sound %u does not exist, set to 0.",
                    qinfo->GetQuestId(), qinfo->_soundTurnIn, qinfo->_soundTurnIn);
                qinfo->_soundTurnIn = 0;                        // no sound will be played
            }
        }

        if (qinfo->_rewardSkillId)
        {
            if (!sSkillLineStore.LookupEntry(qinfo->_rewardSkillId))
            {
                TC_LOG_ERROR("sql.sql", "Quest %u has `RewardSkillId` = %u but this skill does not exist",
                    qinfo->GetQuestId(), qinfo->_rewardSkillId);
            }
            if (!qinfo->_rewardSkillPoints)
            {
                TC_LOG_ERROR("sql.sql", "Quest %u has `RewardSkillId` = %u but `RewardSkillPoints` is 0",
                    qinfo->GetQuestId(), qinfo->_rewardSkillId);
            }
        }

        if (qinfo->_rewardSkillPoints)
        {
            if (qinfo->_rewardSkillPoints > sWorld->GetConfigMaxSkillValue())
            {
                TC_LOG_ERROR("sql.sql", "Quest %u has `RewardSkillPoints` = %u but max possible skill is %u, quest can't be done.",
                    qinfo->GetQuestId(), qinfo->_rewardSkillPoints, sWorld->GetConfigMaxSkillValue());
                // no changes, quest can't be done for this requirement
            }
            if (!qinfo->_rewardSkillId)
            {
                TC_LOG_ERROR("sql.sql", "Quest %u has `RewardSkillPoints` = %u but `RewardSkillId` is 0",
                    qinfo->GetQuestId(), qinfo->_rewardSkillPoints);
            }
        }

        // fill additional data stores
        if (uint32 prevQuestId = std::abs(qinfo->_prevQuestID))
        {
            auto prevQuestItr = _questTemplates.find(prevQuestId);
            if (prevQuestItr == _questTemplates.end())
                TC_LOG_ERROR("sql.sql", "Quest %d has PrevQuestId %i, but no such quest", qinfo->GetQuestId(), qinfo->GetPrevQuestId());
            else if (qinfo->_prevQuestID > 0)
                qinfo->DependentPreviousQuests.push_back(prevQuestId);
        }

        if (uint32 nextQuestId = qinfo->_nextQuestID)
        {
            auto nextQuestItr = _questTemplates.find(nextQuestId);
            if (nextQuestItr == _questTemplates.end())
                TC_LOG_ERROR("sql.sql", "Quest %d has NextQuestId %i, but no such quest", qinfo->GetQuestId(), qinfo->GetNextQuestId());
            else
                nextQuestItr->second->DependentPreviousQuests.push_back(qinfo->GetQuestId());
        }

        if (qinfo->_exclusiveGroup)
            _exclusiveQuestGroups.insert(std::pair<int32, uint32>(qinfo->_exclusiveGroup, qinfo->GetQuestId()));

        if (qinfo->_limitTime)
            qinfo->SetSpecialFlag(QUEST_SPECIAL_FLAGS_TIMED);
    }

    // check QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT for spell with SPELL_EFFECT_QUEST_COMPLETE
    for (uint32 i = 0; i < sSpellMgr->GetSpellInfoStoreSize(); ++i)
    {
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(i);
        if (!spellInfo)
            continue;

        for (uint8 j = 0; j < MAX_SPELL_EFFECTS; ++j)
        {
            if (spellInfo->Effects[j].Effect != SPELL_EFFECT_QUEST_COMPLETE)
                continue;

            uint32 quest_id = spellInfo->Effects[j].MiscValue;

            Quest const* quest = GetQuestTemplate(quest_id);

            // some quest referenced in spells not exist (outdated spells)
            if (!quest)
                continue;

            if (!quest->HasSpecialFlag(QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT))
            {
                TC_LOG_ERROR("sql.sql", "Spell (id: %u) have SPELL_EFFECT_QUEST_COMPLETE for quest %u, but quest not have flag QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT. Quest flags must be fixed, quest modified to enable objective.", spellInfo->Id, quest_id);

                // this will prevent quest completing without objective
                const_cast<Quest*>(quest)->SetSpecialFlag(QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT);
            }
        }
    }

    TC_LOG_INFO("server.loading", ">> Loaded %lu quests definitions in %u ms", (unsigned long)_questTemplates.size(), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadScripts(ScriptsType type)
{
    uint32 oldMSTime = getMSTime();

    ScriptMapMap* scripts = GetScriptsMapByType(type);
    if (!scripts)
        return;

    std::string tableName = GetScriptsTableNameByType(type);
    if (tableName.empty())
        return;

    if (sScriptMgr->IsScriptScheduled())                    // function cannot be called when scripts are in use.
        return;

    TC_LOG_INFO("server.loading", "Loading %s...", tableName.c_str());

    scripts->clear();                                       // need for reload support

    bool isSpellScriptTable = (type == SCRIPTS_SPELL);
    //                                                 0    1       2         3         4          5    6  7  8  9
    QueryResult result = WorldDatabase.PQuery("SELECT id, delay, command, datalong, datalong2, dataint, x, y, z, o%s FROM %s", isSpellScriptTable ? ", effIndex" : "", tableName.c_str());

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 script definitions. DB table `%s` is empty!", tableName.c_str());
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        ScriptInfo tmp;
        tmp.type      = type;
        tmp.id           = fields[0].GetUInt32();
        if (isSpellScriptTable)
            tmp.id      |= fields[10].GetUInt8() << 24;
        tmp.delay        = fields[1].GetUInt32();
        tmp.command      = ScriptCommands(fields[2].GetUInt32());
        tmp.Raw.nData[0] = fields[3].GetUInt32();
        tmp.Raw.nData[1] = fields[4].GetUInt32();
        tmp.Raw.nData[2] = fields[5].GetInt32();
        tmp.Raw.fData[0] = fields[6].GetFloat();
        tmp.Raw.fData[1] = fields[7].GetFloat();
        tmp.Raw.fData[2] = fields[8].GetFloat();
        tmp.Raw.fData[3] = fields[9].GetFloat();

        // generic command args check
        switch (tmp.command)
        {
            case SCRIPT_COMMAND_TALK:
            {
                if (tmp.Talk.ChatType > CHAT_TYPE_WHISPER && tmp.Talk.ChatType != CHAT_MSG_RAID_BOSS_WHISPER)
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` has invalid talk type (datalong = %u) in SCRIPT_COMMAND_TALK for script id %u",
                        tableName.c_str(), tmp.Talk.ChatType, tmp.id);
                    continue;
                }
                if (!tmp.Talk.TextID)
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` has invalid talk text id (dataint = %i) in SCRIPT_COMMAND_TALK for script id %u",
                        tableName.c_str(), tmp.Talk.TextID, tmp.id);
                    continue;
                }

                break;
            }

            case SCRIPT_COMMAND_EMOTE:
            {
                if (!sEmotesStore.LookupEntry(tmp.Emote.EmoteID))
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` has invalid emote id (datalong = %u) in SCRIPT_COMMAND_EMOTE for script id %u",
                        tableName.c_str(), tmp.Emote.EmoteID, tmp.id);
                    continue;
                }
                break;
            }

            case SCRIPT_COMMAND_TELEPORT_TO:
            {
                if (!sMapStore.LookupEntry(tmp.TeleportTo.MapID))
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` has invalid map (Id: %u) in SCRIPT_COMMAND_TELEPORT_TO for script id %u",
                        tableName.c_str(), tmp.TeleportTo.MapID, tmp.id);
                    continue;
                }

                if (!Trinity::IsValidMapCoord(tmp.TeleportTo.DestX, tmp.TeleportTo.DestY, tmp.TeleportTo.DestZ, tmp.TeleportTo.Orientation))
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` has invalid coordinates (X: %f Y: %f Z: %f O: %f) in SCRIPT_COMMAND_TELEPORT_TO for script id %u",
                        tableName.c_str(), tmp.TeleportTo.DestX, tmp.TeleportTo.DestY, tmp.TeleportTo.DestZ, tmp.TeleportTo.Orientation, tmp.id);
                    continue;
                }
                break;
            }

            case SCRIPT_COMMAND_QUEST_EXPLORED:
            {
                Quest const* quest = GetQuestTemplate(tmp.QuestExplored.QuestID);
                if (!quest)
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` has invalid quest (ID: %u) in SCRIPT_COMMAND_QUEST_EXPLORED in `datalong` for script id %u",
                        tableName.c_str(), tmp.QuestExplored.QuestID, tmp.id);
                    continue;
                }

                if (!quest->HasSpecialFlag(QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT))
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` has quest (ID: %u) in SCRIPT_COMMAND_QUEST_EXPLORED in `datalong` for script id %u, but quest not have flag QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT in quest flags. Script command or quest flags wrong. Quest modified to require objective.",
                        tableName.c_str(), tmp.QuestExplored.QuestID, tmp.id);

                    // this will prevent quest completing without objective
                    const_cast<Quest*>(quest)->SetSpecialFlag(QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT);

                    // continue; - quest objective requirement set and command can be allowed
                }

                if (float(tmp.QuestExplored.Distance) > DEFAULT_VISIBILITY_DISTANCE)
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` has too large distance (%u) for exploring objective complete in `datalong2` in SCRIPT_COMMAND_QUEST_EXPLORED in `datalong` for script id %u",
                        tableName.c_str(), tmp.QuestExplored.Distance, tmp.id);
                    continue;
                }

                if (tmp.QuestExplored.Distance && float(tmp.QuestExplored.Distance) > DEFAULT_VISIBILITY_DISTANCE)
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` has too large distance (%u) for exploring objective complete in `datalong2` in SCRIPT_COMMAND_QUEST_EXPLORED in `datalong` for script id %u, max distance is %f or 0 for disable distance check",
                        tableName.c_str(), tmp.QuestExplored.Distance, tmp.id, DEFAULT_VISIBILITY_DISTANCE);
                    continue;
                }

                if (tmp.QuestExplored.Distance && float(tmp.QuestExplored.Distance) < INTERACTION_DISTANCE)
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` has too small distance (%u) for exploring objective complete in `datalong2` in SCRIPT_COMMAND_QUEST_EXPLORED in `datalong` for script id %u, min distance is %f or 0 for disable distance check",
                        tableName.c_str(), tmp.QuestExplored.Distance, tmp.id, INTERACTION_DISTANCE);
                    continue;
                }

                break;
            }

            case SCRIPT_COMMAND_KILL_CREDIT:
            {
                if (!GetCreatureTemplate(tmp.KillCredit.CreatureEntry))
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` has invalid creature (Entry: %u) in SCRIPT_COMMAND_KILL_CREDIT for script id %u",
                        tableName.c_str(), tmp.KillCredit.CreatureEntry, tmp.id);
                    continue;
                }
                break;
            }

            case SCRIPT_COMMAND_RESPAWN_GAMEOBJECT:
            {
                GameObjectData const* data = GetGOData(tmp.RespawnGameobject.GOGuid);
                if (!data)
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` has invalid gameobject (GUID: %u) in SCRIPT_COMMAND_RESPAWN_GAMEOBJECT for script id %u",
                        tableName.c_str(), tmp.RespawnGameobject.GOGuid, tmp.id);
                    continue;
                }

                GameObjectTemplate const* info = GetGameObjectTemplate(data->id);
                if (!info)
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` has gameobject with invalid entry (GUID: %u Entry: %u) in SCRIPT_COMMAND_RESPAWN_GAMEOBJECT for script id %u",
                        tableName.c_str(), tmp.RespawnGameobject.GOGuid, data->id, tmp.id);
                    continue;
                }

                if (info->type == GAMEOBJECT_TYPE_FISHINGNODE ||
                    info->type == GAMEOBJECT_TYPE_FISHINGHOLE ||
                    info->type == GAMEOBJECT_TYPE_DOOR        ||
                    info->type == GAMEOBJECT_TYPE_BUTTON      ||
                    info->type == GAMEOBJECT_TYPE_TRAP)
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` have gameobject type (%u) unsupported by command SCRIPT_COMMAND_RESPAWN_GAMEOBJECT for script id %u",
                        tableName.c_str(), info->entry, tmp.id);
                    continue;
                }
                break;
            }

            case SCRIPT_COMMAND_TEMP_SUMMON_CREATURE:
            {
                if (!Trinity::IsValidMapCoord(tmp.TempSummonCreature.PosX, tmp.TempSummonCreature.PosY, tmp.TempSummonCreature.PosZ, tmp.TempSummonCreature.Orientation))
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` has invalid coordinates (X: %f Y: %f Z: %f O: %f) in SCRIPT_COMMAND_TEMP_SUMMON_CREATURE for script id %u",
                        tableName.c_str(), tmp.TempSummonCreature.PosX, tmp.TempSummonCreature.PosY, tmp.TempSummonCreature.PosZ, tmp.TempSummonCreature.Orientation, tmp.id);
                    continue;
                }

                if (!GetCreatureTemplate(tmp.TempSummonCreature.CreatureEntry))
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` has invalid creature (Entry: %u) in SCRIPT_COMMAND_TEMP_SUMMON_CREATURE for script id %u",
                        tableName.c_str(), tmp.TempSummonCreature.CreatureEntry, tmp.id);
                    continue;
                }
                break;
            }

            case SCRIPT_COMMAND_OPEN_DOOR:
            case SCRIPT_COMMAND_CLOSE_DOOR:
            {
                GameObjectData const* data = GetGOData(tmp.ToggleDoor.GOGuid);
                if (!data)
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` has invalid gameobject (GUID: %u) in %s for script id %u",
                        tableName.c_str(), tmp.ToggleDoor.GOGuid, GetScriptCommandName(tmp.command).c_str(), tmp.id);
                    continue;
                }

                GameObjectTemplate const* info = GetGameObjectTemplate(data->id);
                if (!info)
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` has gameobject with invalid entry (GUID: %u Entry: %u) in %s for script id %u",
                        tableName.c_str(), tmp.ToggleDoor.GOGuid, data->id, GetScriptCommandName(tmp.command).c_str(), tmp.id);
                    continue;
                }

                if (info->type != GAMEOBJECT_TYPE_DOOR)
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` has gameobject type (%u) non supported by command %s for script id %u",
                        tableName.c_str(), info->entry, GetScriptCommandName(tmp.command).c_str(), tmp.id);
                    continue;
                }

                break;
            }

            case SCRIPT_COMMAND_REMOVE_AURA:
            {
                if (!sSpellMgr->GetSpellInfo(tmp.RemoveAura.SpellID))
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` using non-existent spell (id: %u) in SCRIPT_COMMAND_REMOVE_AURA for script id %u",
                        tableName.c_str(), tmp.RemoveAura.SpellID, tmp.id);
                    continue;
                }
                if (tmp.RemoveAura.Flags & ~0x1)                    // 1 bits (0, 1)
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` using unknown flags in datalong2 (%u) in SCRIPT_COMMAND_REMOVE_AURA for script id %u",
                        tableName.c_str(), tmp.RemoveAura.Flags, tmp.id);
                    continue;
                }
                break;
            }

            case SCRIPT_COMMAND_CAST_SPELL:
            {
                if (!sSpellMgr->GetSpellInfo(tmp.CastSpell.SpellID))
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` using non-existent spell (id: %u) in SCRIPT_COMMAND_CAST_SPELL for script id %u",
                        tableName.c_str(), tmp.CastSpell.SpellID, tmp.id);
                    continue;
                }
                if (tmp.CastSpell.Flags > 4)                      // targeting type
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` using unknown target in datalong2 (%u) in SCRIPT_COMMAND_CAST_SPELL for script id %u",
                        tableName.c_str(), tmp.CastSpell.Flags, tmp.id);
                    continue;
                }
                if (tmp.CastSpell.Flags != 4 && tmp.CastSpell.CreatureEntry & ~0x1)                      // 1 bit (0, 1)
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` using unknown flags in dataint (%u) in SCRIPT_COMMAND_CAST_SPELL for script id %u",
                        tableName.c_str(), tmp.CastSpell.CreatureEntry, tmp.id);
                    continue;
                }
                else if (tmp.CastSpell.Flags == 4 && !GetCreatureTemplate(tmp.CastSpell.CreatureEntry))
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` using invalid creature entry in dataint (%u) in SCRIPT_COMMAND_CAST_SPELL for script id %u",
                        tableName.c_str(), tmp.CastSpell.CreatureEntry, tmp.id);
                    continue;
                }
                break;
            }

            case SCRIPT_COMMAND_CREATE_ITEM:
            {
                if (!GetItemTemplate(tmp.CreateItem.ItemEntry))
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` has nonexistent item (entry: %u) in SCRIPT_COMMAND_CREATE_ITEM for script id %u",
                        tableName.c_str(), tmp.CreateItem.ItemEntry, tmp.id);
                    continue;
                }
                if (!tmp.CreateItem.Amount)
                {
                    TC_LOG_ERROR("sql.sql", "Table `%s` SCRIPT_COMMAND_CREATE_ITEM but amount is %u for script id %u",
                        tableName.c_str(), tmp.CreateItem.Amount, tmp.id);
                    continue;
                }
                break;
            }
            default:
                break;
        }

        if (scripts->find(tmp.id) == scripts->end())
        {
            ScriptMap emptyMap;
            (*scripts)[tmp.id] = emptyMap;
        }
        (*scripts)[tmp.id].insert(std::pair<uint32, ScriptInfo>(tmp.delay, tmp));

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u script definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadSpellScripts()
{
    LoadScripts(SCRIPTS_SPELL);

    // check ids
    for (ScriptMapMap::const_iterator itr = sSpellScripts.begin(); itr != sSpellScripts.end(); ++itr)
    {
        uint32 spellId = uint32(itr->first) & 0x00FFFFFF;
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);

        if (!spellInfo)
        {
            TC_LOG_ERROR("sql.sql", "Table `spell_scripts` has not existing spell (Id: %u) as script id", spellId);
            continue;
        }

        uint8 i = (uint8)((uint32(itr->first) >> 24) & 0x000000FF);
        if (i >= MAX_SPELL_EFFECTS)
        {
            TC_LOG_ERROR("sql.sql", "Table `spell_scripts` has entry for spell (Id: %u) with wrong spell effect %u", spellId, i);
            continue;
        }
        //check for correct spellEffect
        if (!spellInfo->Effects[i].Effect || (spellInfo->Effects[i].Effect != SPELL_EFFECT_SCRIPT_EFFECT && spellInfo->Effects[i].Effect != SPELL_EFFECT_DUMMY))
            TC_LOG_ERROR("sql.sql", "Table `spell_scripts` - spell %u effect %u is not SPELL_EFFECT_SCRIPT_EFFECT or SPELL_EFFECT_DUMMY", spellId, i);
    }
}

void ObjectMgr::LoadEventScripts()
{
    LoadScripts(SCRIPTS_EVENT);

    std::set<uint32> evt_scripts;
    // Load all possible script entries from gameobjects
    GameObjectTemplateContainer const* gotc = sObjectMgr->GetGameObjectTemplates();
    for (GameObjectTemplateContainer::const_iterator itr = gotc->begin(); itr != gotc->end(); ++itr)
        if (uint32 eventId = itr->second.GetEventScriptId())
            evt_scripts.insert(eventId);

    // Load all possible script entries from spells
    for (uint32 i = 1; i < sSpellMgr->GetSpellInfoStoreSize(); ++i)
        if (SpellInfo const* spell = sSpellMgr->GetSpellInfo(i))
            for (uint8 j = 0; j < MAX_SPELL_EFFECTS; ++j)
                if (spell->Effects[j].Effect == SPELL_EFFECT_SEND_EVENT)
                    if (spell->Effects[j].MiscValue)
                        evt_scripts.insert(spell->Effects[j].MiscValue);

    for (size_t path_idx = 0; path_idx < sTaxiPathNodesByPath.size(); ++path_idx)
    {
        for (size_t node_idx = 0; node_idx < sTaxiPathNodesByPath[path_idx].size(); ++node_idx)
        {
            TaxiPathNodeEntry const& node = sTaxiPathNodesByPath[path_idx][node_idx];

            if (node.ArrivalEventID)
                evt_scripts.insert(node.ArrivalEventID);

            if (node.DepartureEventID)
                evt_scripts.insert(node.DepartureEventID);
        }
    }

    // Then check if all scripts are in above list of possible script entries
    for (ScriptMapMap::const_iterator itr = sEventScripts.begin(); itr != sEventScripts.end(); ++itr)
    {
        std::set<uint32>::const_iterator itr2 = evt_scripts.find(itr->first);
        if (itr2 == evt_scripts.end())
            TC_LOG_ERROR("sql.sql", "Table `event_scripts` has script (Id: %u) not referring to any gameobject_template type 10 data2 field, type 3 data6 field, type 13 data 2 field or any spell effect %u",
                itr->first, SPELL_EFFECT_SEND_EVENT);
    }
}

//Load WP Scripts
void ObjectMgr::LoadWaypointScripts()
{
    LoadScripts(SCRIPTS_WAYPOINT);

    std::set<uint32> actionSet;

    for (ScriptMapMap::const_iterator itr = sWaypointScripts.begin(); itr != sWaypointScripts.end(); ++itr)
        actionSet.insert(itr->first);

    WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_WAYPOINT_DATA_ACTION);
    PreparedQueryResult result = WorldDatabase.Query(stmt);

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 action = fields[0].GetUInt32();

            actionSet.erase(action);
        }
        while (result->NextRow());
    }

    for (std::set<uint32>::iterator itr = actionSet.begin(); itr != actionSet.end(); ++itr)
        TC_LOG_ERROR("sql.sql", "There is no waypoint which links to the waypoint script %u", *itr);
}

void ObjectMgr::LoadSpellScriptNames()
{
    uint32 oldMSTime = getMSTime();

    _spellScriptsStore.clear();                            // need for reload case

    QueryResult result = WorldDatabase.Query("SELECT spell_id, ScriptName FROM spell_script_names");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 spell script names. DB table `spell_script_names` is empty!");
        return;
    }

    uint32 count = 0;

    do
    {

        Field* fields = result->Fetch();

        int32 spellId                = fields[0].GetInt32();
        std::string const scriptName = fields[1].GetString();

        bool allRanks = false;
        if (spellId < 0)
        {
            allRanks = true;
            spellId = -spellId;
        }

        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
        if (!spellInfo)
        {
            TC_LOG_ERROR("sql.sql", "Scriptname: `%s` spell (Id: %d) does not exist.", scriptName.c_str(), fields[0].GetInt32());
            continue;
        }

        if (allRanks)
        {
            if (!spellInfo->IsRanked())
                TC_LOG_ERROR("sql.sql", "Scriptname: `%s` spell (Id: %d) has no ranks of spell.", scriptName.c_str(), fields[0].GetInt32());

            if (spellInfo->GetFirstRankSpell()->Id != uint32(spellId))
            {
                TC_LOG_ERROR("sql.sql", "Scriptname: `%s` spell (Id: %d) is not first rank of spell.", scriptName.c_str(), fields[0].GetInt32());
                continue;
            }
            while (spellInfo)
            {
                _spellScriptsStore.insert(SpellScriptsContainer::value_type(spellInfo->Id, GetScriptId(scriptName)));
                spellInfo = spellInfo->GetNextRankSpell();
            }
        }
        else
        {
            if (spellInfo->IsRanked())
                TC_LOG_ERROR("sql.sql", "Scriptname: `%s` spell (Id: %d) is ranked spell. Perhaps not all ranks are assigned to this script.", scriptName.c_str(), spellId);

            _spellScriptsStore.insert(SpellScriptsContainer::value_type(spellInfo->Id, GetScriptId(scriptName)));
        }

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u spell script names in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::ValidateSpellScripts()
{
    uint32 oldMSTime = getMSTime();

    if (_spellScriptsStore.empty())
    {
        TC_LOG_INFO("server.loading", ">> Validated 0 scripts.");
        return;
    }

    uint32 count = 0;

    for (SpellScriptsContainer::iterator itr = _spellScriptsStore.begin(); itr != _spellScriptsStore.end();)
    {
        SpellInfo const* spellEntry = sSpellMgr->GetSpellInfo(itr->first);
        std::vector<std::pair<SpellScriptLoader *, SpellScriptsContainer::iterator> > SpellScriptLoaders;
        sScriptMgr->CreateSpellScriptLoaders(itr->first, SpellScriptLoaders);
        itr = _spellScriptsStore.upper_bound(itr->first);

        for (std::vector<std::pair<SpellScriptLoader *, SpellScriptsContainer::iterator> >::iterator sitr = SpellScriptLoaders.begin(); sitr != SpellScriptLoaders.end(); ++sitr)
        {
            SpellScript* spellScript = sitr->first->GetSpellScript();
            AuraScript* auraScript = sitr->first->GetAuraScript();
            bool valid = true;
            if (!spellScript && !auraScript)
            {
                TC_LOG_ERROR("scripts", "Functions GetSpellScript() and GetAuraScript() of script `%s` do not return objects - script skipped",  GetScriptName(sitr->second->second).c_str());
                valid = false;
            }
            if (spellScript)
            {
                spellScript->_Init(&sitr->first->GetName(), spellEntry->Id);
                spellScript->_Register();
                if (!spellScript->_Validate(spellEntry))
                    valid = false;
                delete spellScript;
            }
            if (auraScript)
            {
                auraScript->_Init(&sitr->first->GetName(), spellEntry->Id);
                auraScript->_Register();
                if (!auraScript->_Validate(spellEntry))
                    valid = false;
                delete auraScript;
            }
            if (!valid)
            {
                _spellScriptsStore.erase(sitr->second);
            }
        }
        ++count;
    }

    TC_LOG_INFO("server.loading", ">> Validated %u scripts in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadPageTexts()
{
    uint32 oldMSTime = getMSTime();

    //                                                 0      1       2
    QueryResult result = WorldDatabase.Query("SELECT entry, text, next_page FROM page_text");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 page texts. DB table `page_text` is empty!");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        PageText& pageText = _pageTextStore[fields[0].GetUInt32()];

        pageText.Text     = fields[1].GetString();
        pageText.NextPage = fields[2].GetUInt32();

        ++count;
    }
    while (result->NextRow());

    for (PageTextContainer::const_iterator itr = _pageTextStore.begin(); itr != _pageTextStore.end(); ++itr)
    {
        if (itr->second.NextPage)
        {
            PageTextContainer::const_iterator itr2 = _pageTextStore.find(itr->second.NextPage);
            if (itr2 == _pageTextStore.end())
                TC_LOG_ERROR("sql.sql", "Page text (Id: %u) has not existing next page (Id: %u)", itr->first, itr->second.NextPage);

        }
    }

    TC_LOG_INFO("server.loading", ">> Loaded %u page texts in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

PageText const* ObjectMgr::GetPageText(uint32 pageEntry)
{
    PageTextContainer::const_iterator itr = _pageTextStore.find(pageEntry);
    if (itr != _pageTextStore.end())
        return &(itr->second);

    return NULL;
}

void ObjectMgr::LoadPageTextLocales()
{
    uint32 oldMSTime = getMSTime();

    _pageTextLocaleStore.clear();                             // need for reload case

    //                                               0   1        2
    QueryResult result = WorldDatabase.Query("SELECT ID, locale, `Text` FROM page_text_locale");

    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();

        uint32 id                   = fields[0].GetUInt32();
        std::string localeName      = fields[1].GetString();

        LocaleConstant locale = GetLocaleByName(localeName);
        if (!sWorld->getBoolConfig(CONFIG_LOAD_LOCALES) || locale == LOCALE_enUS)
            continue;

        PageTextLocale& data = _pageTextLocaleStore[id];
        AddLocaleString(fields[2].GetString(), locale, data.Text);
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u PageText locale strings in %u ms", uint32(_pageTextLocaleStore.size()), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadInstanceTemplate()
{
    uint32 oldMSTime = getMSTime();

    //                                                0     1       2        4
    QueryResult result = WorldDatabase.Query("SELECT map, parent, script, allowMount FROM instance_template");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 instance templates. DB table `page_text` is empty!");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint16 mapID = fields[0].GetUInt16();

        if (!MapManager::IsValidMAP(mapID, true))
        {
            TC_LOG_ERROR("sql.sql", "ObjectMgr::LoadInstanceTemplate: bad mapid %d for template!", mapID);
            continue;
        }

        InstanceTemplate instanceTemplate;

        instanceTemplate.AllowMount = fields[3].GetBool();
        instanceTemplate.Parent     = uint32(fields[1].GetUInt16());
        instanceTemplate.ScriptId   = sObjectMgr->GetScriptId(fields[2].GetString());

        _instanceTemplateStore[mapID] = instanceTemplate;

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u instance templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

InstanceTemplate const* ObjectMgr::GetInstanceTemplate(uint32 mapID)
{
    InstanceTemplateContainer::const_iterator itr = _instanceTemplateStore.find(uint16(mapID));
    if (itr != _instanceTemplateStore.end())
        return &(itr->second);

    return NULL;
}

void ObjectMgr::LoadInstanceEncounters()
{
    uint32 oldMSTime = getMSTime();

    //                                                 0         1            2            3                4
    QueryResult result = WorldDatabase.Query("SELECT entry, difficulty+0, creditType, creditEntry, lastEncounterDungeon FROM instance_encounters");
    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 instance encounters, table is empty!");
        return;
    }

    uint32 count = 0;
    std::map<uint32, DungeonEncounterEntry const*> dungeonLastBosses;
    do
    {
        Field* fields = result->Fetch();
        uint32 entry = fields[0].GetUInt32();
        uint8 difficulty = fields[1].GetUInt8();
        uint8 creditType = fields[2].GetUInt8();
        uint32 creditEntry = fields[3].GetUInt32();
        uint32 lastEncounterDungeon = fields[4].GetUInt16();
        DungeonEncounterEntry const* dungeonEncounter = sDungeonEncounterStore.LookupEntry(entry);
        if (!dungeonEncounter)
        {
            TC_LOG_ERROR("sql.sql", "Table `instance_encounters` has an invalid encounter id %u, skipped!", entry);
            continue;
        }

        if (lastEncounterDungeon && !sLFGMgr->GetLFGDungeonEntry(lastEncounterDungeon))
        {
            TC_LOG_ERROR("sql.sql", "Table `instance_encounters` has an encounter %u (%s) marked as final for invalid dungeon id %u, skipped!", entry, dungeonEncounter->encounterName.m_impl[0], lastEncounterDungeon);
            continue;
        }

        std::map<uint32, DungeonEncounterEntry const*>::const_iterator itr = dungeonLastBosses.find(lastEncounterDungeon);
        if (lastEncounterDungeon)
        {
            if (itr != dungeonLastBosses.end())
            {
                TC_LOG_ERROR("sql.sql", "Table `instance_encounters` specified encounter %u (%s) as last encounter but %u (%s) is already marked as one, skipped!", entry, dungeonEncounter->encounterName.m_impl[0], itr->second->id, itr->second->encounterName.m_impl[0]);
                continue;
            }

            dungeonLastBosses[lastEncounterDungeon] = dungeonEncounter;
        }

        switch (creditType)
        {
            case ENCOUNTER_CREDIT_KILL_CREATURE:
            {
                CreatureTemplate const* creatureInfo = GetCreatureTemplate(creditEntry);
                if (!creatureInfo)
                {
                    TC_LOG_ERROR("sql.sql", "Table `instance_encounters` has an invalid creature (entry %u) linked to the encounter %u (%s), skipped!", creditEntry, entry, dungeonEncounter->encounterName.m_impl[0]);
                    continue;
                }
                const_cast<CreatureTemplate*>(creatureInfo)->flags_extra |= CREATURE_FLAG_EXTRA_DUNGEON_BOSS;
                break;
            }
            case ENCOUNTER_CREDIT_CAST_SPELL:
                if (!sSpellMgr->GetSpellInfo(creditEntry))
                {
                    TC_LOG_ERROR("sql.sql", "Table `instance_encounters` has an invalid spell (entry %u) linked to the encounter %u (%s), skipped!", creditEntry, entry, dungeonEncounter->encounterName.m_impl[0]);
                    continue;
                }
                break;
            default:
                TC_LOG_ERROR("sql.sql", "Table `instance_encounters` has an invalid credit type (%u) for encounter %u (%s), skipped!", creditType, entry, dungeonEncounter->encounterName.m_impl[0]);
                continue;
        }

        if (!difficulty)
        {
            for (uint32 i = 0; i < MAX_DIFFICULTY; ++i)
            {
                if (GetMapDifficultyData(dungeonEncounter->mapId, Difficulty(i)))
                {
                    DungeonEncounterList& encounters = _dungeonEncounterStore[MAKE_PAIR32(dungeonEncounter->mapId, i)];
                    encounters.push_back(new DungeonEncounter(dungeonEncounter, EncounterCreditType(creditType), creditEntry, lastEncounterDungeon));
                }
            }
        }
        else
        {
            DungeonEncounterList& encounters = _dungeonEncounterStore[MAKE_PAIR32(dungeonEncounter->mapId, difficulty)];
            encounters.push_back(new DungeonEncounter(dungeonEncounter, EncounterCreditType(creditType), creditEntry, lastEncounterDungeon));
        }

        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u instance encounters in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

GossipText const* ObjectMgr::GetGossipText(uint32 Text_ID) const
{
    GossipTextContainer::const_iterator itr = _gossipTextStore.find(Text_ID);
    if (itr != _gossipTextStore.end())
        return &itr->second;
    return NULL;
}

void ObjectMgr::LoadGossipText()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT ID, "
        "text0_0, text0_1, BroadcastTextID0, lang0, Probability0, EmoteDelay0_0, Emote0_0, EmoteDelay0_1, Emote0_1, EmoteDelay0_2, Emote0_2, "
        "text1_0, text1_1, BroadcastTextID1, lang1, Probability1, EmoteDelay1_0, Emote1_0, EmoteDelay1_1, Emote1_1, EmoteDelay1_2, Emote1_2, "
        "text2_0, text2_1, BroadcastTextID2, lang2, Probability2, EmoteDelay2_0, Emote2_0, EmoteDelay2_1, Emote2_1, EmoteDelay2_2, Emote2_2, "
        "text3_0, text3_1, BroadcastTextID3, lang3, Probability3, EmoteDelay3_0, Emote3_0, EmoteDelay3_1, Emote3_1, EmoteDelay3_2, Emote3_2, "
        "text4_0, text4_1, BroadcastTextID4, lang4, Probability4, EmoteDelay4_0, Emote4_0, EmoteDelay4_1, Emote4_1, EmoteDelay4_2, Emote4_2, "
        "text5_0, text5_1, BroadcastTextID5, lang5, Probability5, EmoteDelay5_0, Emote5_0, EmoteDelay5_1, Emote5_1, EmoteDelay5_2, Emote5_2, "
        "text6_0, text6_1, BroadcastTextID6, lang6, Probability6, EmoteDelay6_0, Emote6_0, EmoteDelay6_1, Emote6_1, EmoteDelay6_2, Emote6_2, "
        "text7_0, text7_1, BroadcastTextID7, lang7, Probability7, EmoteDelay7_0, Emote7_0, EmoteDelay7_1, Emote7_1, EmoteDelay7_2, Emote7_2 "
        "FROM npc_text");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 npc texts, table is empty!");
        return;
    }

    _gossipTextStore.reserve(result->GetRowCount());

    uint32 count = 0;
    uint8 cic;

    do
    {
        ++count;
        cic = 0;

        Field* fields = result->Fetch();

        uint32 id = fields[cic++].GetUInt32();
        if (!id)
        {
            TC_LOG_ERROR("sql.sql", "Table `npc_text` has record with reserved id 0, ignore.");
            continue;
        }

        GossipText& gText = _gossipTextStore[id];

        for (uint8 i = 0; i < MAX_GOSSIP_TEXT_OPTIONS; ++i)
        {
            GossipTextOption& gOption = gText.Options[i];
            gOption.Text_0           = fields[cic++].GetString();
            gOption.Text_1           = fields[cic++].GetString();
            gOption.BroadcastTextID  = fields[cic++].GetUInt32();
            gOption.Language         = fields[cic++].GetUInt8();
            gOption.Probability      = fields[cic++].GetFloat();

            for (uint8 j = 0; j < MAX_GOSSIP_TEXT_EMOTES; ++j)
            {
                gOption.Emotes[j]._Delay = fields[cic++].GetUInt16();
                gOption.Emotes[j]._Emote = fields[cic++].GetUInt16();
            }

            // check broadcast_text correctness
            if (gOption.BroadcastTextID)
            {
                if (BroadcastText const* bcText = sObjectMgr->GetBroadcastText(gOption.BroadcastTextID))
                {
                    if (bcText->Text[DEFAULT_LOCALE] != gOption.Text_0)
                        TC_LOG_INFO("sql.sql", "Row %u in table `npc_text` has mismatch between text%u_0 and the corresponding Text in `broadcast_text` row %u", id, i, gOption.BroadcastTextID);
                    if (bcText->Text1[DEFAULT_LOCALE] != gOption.Text_1)
                        TC_LOG_INFO("sql.sql", "Row %u in table `npc_text` has mismatch between text%u_1 and the corresponding Text1 in `broadcast_text` row %u", id, i, gOption.BroadcastTextID);
                }
                else
                {
                    TC_LOG_ERROR("sql.sql", "GossipText (Id: %u) in table `npc_text` has non-existing or incompatible BroadcastTextID%u %u.", id, i, gOption.BroadcastTextID);
                    gOption.BroadcastTextID = 0;
                }


            }
        }

    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u npc texts in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadNpcTextLocales()
{
    uint32 oldMSTime = getMSTime();

    _npcTextLocaleStore.clear();                              // need for reload case

    QueryResult result = WorldDatabase.Query("SELECT ID, Locale, "
    //   2        3        4        5        6        7        8        9        10       11       12       13       14       15       16       17
        "Text0_0, Text0_1, Text1_0, Text1_1, Text2_0, Text2_1, Text3_0, Text3_1, Text4_0, Text4_1, Text5_0, Text5_1, Text6_0, Text6_1, Text7_0, Text7_1 "
        "FROM npc_text_locale");

    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();

        uint32 id               = fields[0].GetUInt32();
        std::string localeName  = fields[1].GetString();

        NpcTextLocale& data     = _npcTextLocaleStore[id];
        LocaleConstant locale   = GetLocaleByName(localeName);
        if (!sWorld->getBoolConfig(CONFIG_LOAD_LOCALES) || locale == LOCALE_enUS)
            continue;

        for (uint8 i = 0; i < MAX_GOSSIP_TEXT_OPTIONS; ++i)
        {
            AddLocaleString(fields[2 + i * 2].GetString(), locale, data.Text_0[i]);
            AddLocaleString(fields[3 + i * 2].GetString(), locale, data.Text_1[i]);
        }
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u NpcText locale strings in %u ms", uint32(_npcTextLocaleStore.size()), GetMSTimeDiffToNow(oldMSTime));
}

//not very fast function but it is called only once a day, or on starting-up
void ObjectMgr::ReturnOrDeleteOldMails(bool serverUp)
{
    uint32 oldMSTime = getMSTime();

    time_t curTime = time(NULL);
    tm lt;
    localtime_r(&curTime, &lt);
    uint64 basetime(curTime);
    TC_LOG_INFO("misc", "Returning mails current time: hour: %d, minute: %d, second: %d ", lt.tm_hour, lt.tm_min, lt.tm_sec);

    // Delete all old mails without item and without body immediately, if starting server
    if (!serverUp)
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_EMPTY_EXPIRED_MAIL);
        stmt->setUInt64(0, basetime);
        CharacterDatabase.Execute(stmt);
    }
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_EXPIRED_MAIL);
    stmt->setUInt64(0, basetime);
    PreparedQueryResult result = CharacterDatabase.Query(stmt);
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> No expired mails found.");
        return;                                             // any mails need to be returned or deleted
    }

    std::map<uint32 /*messageId*/, MailItemInfoVec> itemsCache;
    stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_EXPIRED_MAIL_ITEMS);
    stmt->setUInt32(0, (uint32)basetime);
    if (PreparedQueryResult items = CharacterDatabase.Query(stmt))
    {
        MailItemInfo item;
        do
        {
            Field* fields = items->Fetch();
            item.item_guid = fields[0].GetUInt32();
            item.item_template = fields[1].GetUInt32();
            uint32 mailId = fields[2].GetUInt32();
            itemsCache[mailId].push_back(item);
        } while (items->NextRow());
    }

    uint32 deletedCount = 0;
    uint32 returnedCount = 0;
    do
    {
        Field* fields = result->Fetch();
        Mail* m = new Mail;
        m->messageID      = fields[0].GetUInt32();
        m->messageType    = fields[1].GetUInt8();
        m->sender         = fields[2].GetUInt32();
        m->receiver       = fields[3].GetUInt32();
        bool has_items    = fields[4].GetBool();
        m->expire_time    = time_t(fields[5].GetUInt32());
        m->deliver_time   = 0;
        m->COD            = fields[6].GetUInt64();
        m->checked        = fields[7].GetUInt32();
        m->mailTemplateId = fields[8].GetInt16();

        Player* player = NULL;
        if (serverUp)
            player = ObjectAccessor::FindPlayer(ObjectGuid::Create<HighGuid::Player>(m->receiver));

        if (player && player->m_mailsLoaded)
        {                                                   // this code will run very improbably (the time is between 4 and 5 am, in game is online a player, who has old mail
            // his in mailbox and he has already listed his mails)
            delete m;
            continue;
        }

        // Delete or return mail
        if (has_items)
        {
            // read items from cache
            m->items.swap(itemsCache[m->messageID]);

            // if it is mail from non-player, or if it's already return mail, it shouldn't be returned, but deleted
            if (m->messageType != MAIL_NORMAL || (m->checked & (MAIL_CHECK_MASK_COD_PAYMENT | MAIL_CHECK_MASK_RETURNED)))
            {
                // mail open and then not returned
                for (MailItemInfoVec::iterator itr2 = m->items.begin(); itr2 != m->items.end(); ++itr2)
                {
                    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ITEM_INSTANCE);
                    stmt->setUInt32(0, itr2->item_guid);
                    CharacterDatabase.Execute(stmt);
                }
            }
            else
            {
                // Mail will be returned
                stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_MAIL_RETURNED);
                stmt->setUInt32(0, m->receiver);
                stmt->setUInt32(1, m->sender);
                stmt->setUInt32(2, basetime + 30 * DAY);
                stmt->setUInt32(3, basetime);
                stmt->setUInt8 (4, uint8(MAIL_CHECK_MASK_RETURNED));
                stmt->setUInt32(5, m->messageID);
                CharacterDatabase.Execute(stmt);
                for (MailItemInfoVec::iterator itr2 = m->items.begin(); itr2 != m->items.end(); ++itr2)
                {
                    // Update receiver in mail items for its proper delivery, and in instance_item for avoid lost item at sender delete
                    stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_MAIL_ITEM_RECEIVER);
                    stmt->setUInt32(0, m->sender);
                    stmt->setUInt32(1, itr2->item_guid);
                    CharacterDatabase.Execute(stmt);

                    stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ITEM_OWNER);
                    stmt->setUInt32(0, m->sender);
                    stmt->setUInt32(1, itr2->item_guid);
                    CharacterDatabase.Execute(stmt);
                }
                delete m;
                ++returnedCount;
                continue;
            }
        }

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_MAIL_BY_ID);
        stmt->setUInt32(0, m->messageID);
        CharacterDatabase.Execute(stmt);
        delete m;
        ++deletedCount;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Processed %u expired mails: %u deleted and %u returned in %u ms", deletedCount + returnedCount, deletedCount, returnedCount, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadQuestGiverAreaTriggers()
{
    uint32 oldMSTime = getMSTime();

    _questGiverAreaTriggerStore.clear();                           // need for reload case

    QueryResult result = WorldDatabase.Query("SELECT id, quest FROM areatrigger_queststarter");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 quest giver trigger points. DB table `areatrigger_queststarter` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        ++count;

        Field* fields = result->Fetch();

        uint32 trigger_ID = fields[0].GetUInt32();
        uint32 quest_ID = fields[1].GetUInt32();

        AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(trigger_ID);
        if (!atEntry)
        {
            TC_LOG_ERROR("sql.sql", "Area trigger (ID:%u) does not exist in `AreaTrigger.dbc`.", trigger_ID);
            continue;
        }

        Quest const* quest = GetQuestTemplate(quest_ID);

        if (!quest)
        {
            TC_LOG_ERROR("sql.sql", "Table `areatrigger_queststarter` has record (id: %u) for not existing quest %u", trigger_ID, quest_ID);
            continue;
        }

        if (!quest->HasFlag(QUEST_FLAGS_AUTO_ACCEPT))
        {
            TC_LOG_ERROR("sql.sql", "Table `areatrigger_queststarter` has record (id: %u) for quest %u, but quest does not have flag QUEST_FLAGS_AUTO_ACCEPT. Trigger or quest flags must be fixed, quest modified to be autoacceptable.", trigger_ID, quest_ID);

            // this will prevent quest completing without objective
            const_cast<Quest*>(quest)->_flags |= QUEST_FLAGS_AUTO_ACCEPT;

            // continue; - quest modified to required objective and trigger can be allowed.
        }

        _questGiverAreaTriggerStore[trigger_ID].insert(quest_ID);

    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u quest giver trigger points in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadQuestAreaTriggers()
{
    uint32 oldMSTime = getMSTime();

    _questAreaTriggerStore.clear();                           // need for reload case

    QueryResult result = WorldDatabase.Query("SELECT id, quest FROM areatrigger_involvedrelation");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 quest trigger points. DB table `areatrigger_involvedrelation` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        ++count;

        Field* fields = result->Fetch();

        uint32 trigger_ID = fields[0].GetUInt32();
        uint32 quest_ID   = fields[1].GetUInt32();

        AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(trigger_ID);
        if (!atEntry)
        {
            TC_LOG_ERROR("sql.sql", "Area trigger (ID:%u) does not exist in `AreaTrigger.dbc`.", trigger_ID);
            continue;
        }

        Quest const* quest = GetQuestTemplate(quest_ID);

        if (!quest)
        {
            TC_LOG_ERROR("sql.sql", "Table `areatrigger_involvedrelation` has record (id: %u) for not existing quest %u", trigger_ID, quest_ID);
            continue;
        }

        if (!quest->HasSpecialFlag(QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT))
        {
            TC_LOG_ERROR("sql.sql", "Table `areatrigger_involvedrelation` has record (id: %u) for quest %u, but quest does not have flag QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT. Trigger or quest flags must be fixed, quest modified to require objective.", trigger_ID, quest_ID);

            // this will prevent quest completing without objective
            const_cast<Quest*>(quest)->SetSpecialFlag(QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT);

            // continue; - quest modified to required objective and trigger can be allowed.
        }

        _questAreaTriggerStore[trigger_ID].insert(quest_ID);

    } while (result->NextRow());

    for (auto const& pair : _questObjectives)
    {
        QuestObjective const* objective = pair.second;
        if (objective->Type == QUEST_OBJECTIVE_AREATRIGGER)
            _questAreaTriggerStore[objective->ID].insert(objective->QuestID);
    }

    TC_LOG_INFO("server.loading", ">> Loaded %u quest trigger points in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadTavernAreaTriggers()
{
    uint32 oldMSTime = getMSTime();

    _tavernAreaTriggerStore.clear();                          // need for reload case

    QueryResult result = WorldDatabase.Query("SELECT id FROM areatrigger_tavern");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 tavern triggers. DB table `areatrigger_tavern` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        ++count;

        Field* fields = result->Fetch();

        uint32 Trigger_ID      = fields[0].GetUInt32();

        AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(Trigger_ID);
        if (!atEntry)
        {
            TC_LOG_ERROR("sql.sql", "Area trigger (ID:%u) does not exist in `AreaTrigger.dbc`.", Trigger_ID);
            continue;
        }

        _tavernAreaTriggerStore.insert(Trigger_ID);
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u tavern triggers in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadAreaTriggerScripts()
{
    uint32 oldMSTime = getMSTime();

    _areaTriggerScriptStore.clear();                            // need for reload case

    QueryResult result = WorldDatabase.Query("SELECT entry, ScriptName FROM areatrigger_scripts");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 areatrigger scripts. DB table `areatrigger_scripts` is empty.");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 triggerId      = fields[0].GetUInt32();
        char const* scriptName = fields[1].GetCString();

        AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(triggerId);

        if (!atEntry)
        {
            TC_LOG_ERROR("sql.sql", "AreaTrigger (ID: %u) does not exist in `AreaTrigger.dbc`.", triggerId);
            continue;
        }

        _areaTriggerScriptStore[triggerId] = GetScriptId(scriptName);
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded " SZFMTD " areatrigger scripts in %u ms", _areaTriggerScriptStore.size(), GetMSTimeDiffToNow(oldMSTime));
}

uint32 ObjectMgr::GetNearestTaxiNode(float x, float y, float z, uint32 mapid, uint32 team)
{
    bool found = false;
    float dist = 10000;
    uint32 id = 0;

    for (uint32 i = 1; i < sTaxiNodesStore.GetNumRows(); ++i)
    {
        TaxiNodesEntry const* node = sTaxiNodesStore.LookupEntry(i);

        if (!node || node->map_id != mapid || (!node->MountCreatureID[team == ALLIANCE ? 1 : 0] && node->MountCreatureID[0] != 32981)) // dk flight
            continue;

        uint8  field   = (uint8)((i - 1) / 8);
        uint32 submask = 1 << ((i-1) % 8);

        // skip not taxi network nodes
        if ((sTaxiNodesMask[field] & submask) == 0)
            continue;

        float dist2 = (node->x - x)*(node->x - x)+(node->y - y)*(node->y - y)+(node->z - z)*(node->z - z);
        if (found)
        {
            if (dist2 < dist)
            {
                dist = dist2;
                id = i;
            }
        }
        else
        {
            found = true;
            dist = dist2;
            id = i;
        }
    }

    if (id == 626) // Some shit in Exodar, hack it for now
        id = 94;

    if (id == 343) // Splintertree Post, Ashenvale
        id = 61;

    if (id == 345) // Zoram'gar Outpost, Ashenvale
        id = 58;

    if (id == 414) // Mudsprocket, Dustwallow Marsh
        id = 179;

    return id;
}

void ObjectMgr::GetTaxiPath(uint32 source, uint32 destination, uint32 &path, uint32 &cost)
{
    TaxiPathSetBySource::iterator src_i = sTaxiPathSetBySource.find(source);
    if (src_i == sTaxiPathSetBySource.end())
    {
        path = 0;
        cost = 0;
        return;
    }

    TaxiPathSetForSource& pathSet = src_i->second;

    TaxiPathSetForSource::iterator dest_i = pathSet.find(destination);
    if (dest_i == pathSet.end())
    {
        path = 0;
        cost = 0;
        return;
    }

    cost = dest_i->second.price;
    path = dest_i->second.ID;
}

uint32 ObjectMgr::GetTaxiMountDisplayId(uint32 id, uint32 team, bool allowed_alt_team /* = false */)
{
    CreatureModel mountModel;
    CreatureTemplate const* mount_info = nullptr;

    // select mount creature id
    TaxiNodesEntry const* node = sTaxiNodesStore.LookupEntry(id);
    if (node)
    {
        uint32 mount_entry = 0;
        if (team == ALLIANCE)
            mount_entry = node->MountCreatureID[1];
        else
            mount_entry = node->MountCreatureID[0];

        // Fix for Alliance not being able to use Acherus taxi
        // only one mount type for both sides
        if (mount_entry == 0 && allowed_alt_team)
        {
            // Simply reverse the selection. At least one team in theory should have a valid mount ID to choose.
            mount_entry = team == ALLIANCE ? node->MountCreatureID[0] : node->MountCreatureID[1];
        }

        CreatureTemplate const* mount_info = GetCreatureTemplate(mount_entry);
        if (mount_info)
        {
            CreatureModel const* model = mount_info->GetRandomValidModel();
            if (!model)
            {
                TC_LOG_ERROR("sql.sql", "No displayid found for the taxi mount with the entry %u! Can't load it!", mount_entry);
                return 0;
            }
            mountModel = *model;
        }
    }

    // minfo is not actually used but the mount_id was updated
    GetCreatureModelRandomGender(&mountModel, mount_info);

    return mountModel.CreatureDisplayID;
}

Quest const* ObjectMgr::GetQuestTemplate(uint32 quest_id) const
{
    QuestMap::const_iterator itr = _questTemplates.find(quest_id);
    return itr != _questTemplates.end() ? itr->second : NULL;
}

void ObjectMgr::LoadGraveyardZones()
{
    uint32 oldMSTime = getMSTime();

    GraveYardStore.clear();                                  // need for reload case

    //                                                0       1         2
    QueryResult result = WorldDatabase.Query("SELECT id, ghost_zone, faction FROM game_graveyard_zone");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 graveyard-zone links. DB table `game_graveyard_zone` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        ++count;

        Field* fields = result->Fetch();

        uint32 safeLocId = fields[0].GetUInt32();
        uint32 zoneId = fields[1].GetUInt32();
        uint32 team   = fields[2].GetUInt16();

        WorldSafeLocsEntry const* entry = sWorldSafeLocsStore.LookupEntry(safeLocId);
        if (!entry)
        {
            TC_LOG_ERROR("sql.sql", "Table `game_graveyard_zone` has a record for not existing graveyard (WorldSafeLocs.dbc id) %u, skipped.", safeLocId);
            continue;
        }

        AreaTableEntry const* areaEntry = sAreaTableStore.LookupEntry(zoneId);
        if (!areaEntry)
        {
            TC_LOG_ERROR("sql.sql", "Table `game_graveyard_zone` has a record for not existing zone id (%u), skipped.", zoneId);
            continue;
        }

        if (team != 0 && team != HORDE && team != ALLIANCE)
        {
            TC_LOG_ERROR("sql.sql", "Table `game_graveyard_zone` has a record for non player faction (%u), skipped.", team);
            continue;
        }

        if (!AddGraveYardLink(safeLocId, zoneId, team, false))
            TC_LOG_ERROR("sql.sql", "Table `game_graveyard_zone` has a duplicate record for Graveyard (ID: %u) and Zone (ID: %u), skipped.", safeLocId, zoneId);
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u graveyard-zone links in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

WorldSafeLocsEntry const* ObjectMgr::GetDefaultGraveYard(uint32 team)
{
    enum DefaultGraveyard
    {
        HORDE_GRAVEYARD    = 10, // Crossroads
        ALLIANCE_GRAVEYARD = 4   // Westfall
    };

    if (team == HORDE)
        return sWorldSafeLocsStore.LookupEntry(HORDE_GRAVEYARD);
    else if (team == ALLIANCE)
        return sWorldSafeLocsStore.LookupEntry(ALLIANCE_GRAVEYARD);
    else return NULL;
}

WorldSafeLocsEntry const* ObjectMgr::GetClosestGraveYard(WorldLocation const& location, uint32 team, WorldObject* conditionObject)
{
    float x, y, z;
    location.GetPosition(x, y, z);
    uint32 MapId = location.GetMapId();

    // search for zone associated closest graveyard
    uint32 areaId, zoneId;
    sMapMgr->GetZoneAndAreaId(PHASEMASK_NORMAL, zoneId, areaId, MapId, x, y, z);

    if (!zoneId)
    {
        if (z > -500)
        {
            TC_LOG_ERROR("misc", "ZoneId not found for map %u coords (%f, %f, %f)", MapId, x, y, z);
            return GetDefaultGraveYard(team);
        }
    }

    // Simulate std. algorithm:
    //   found some graveyard associated to (ghost_zone, ghost_map)
    //
    //   if mapId == graveyard.mapId (ghost in plain zone or city or battleground) and search graveyard at same map
    //     then check faction
    //   if mapId != graveyard.mapId (ghost in instance) and search any graveyard associated
    //     then check faction
    GraveYardMapBounds range = GraveYardStore.equal_range(zoneId);
    MapEntry const* map = sMapStore.LookupEntry(MapId);

    ConditionSourceInfo conditionSource(conditionObject);

    // not need to check validity of map object; MapId _MUST_ be valid here
    if (range.first == range.second && !map->IsBattlegroundOrArena())
    {
        TC_LOG_ERROR("sql.sql", "Table `game_graveyard_zone` incomplete: Zone %u Team %u does not have a linked graveyard.", zoneId, team);
        return GetDefaultGraveYard(team);
    }

    // at corpse map
    bool foundNear = false;
    float distNear = 10000;
    WorldSafeLocsEntry const* entryNear = NULL;

    // at entrance map for corpse map
    bool foundEntr = false;
    float distEntr = 10000;
    WorldSafeLocsEntry const* entryEntr = NULL;

    // some where other
    WorldSafeLocsEntry const* entryFar = NULL;

    MapEntry const* mapEntry = sMapStore.LookupEntry(MapId);

    for (; range.first != range.second; ++range.first)
    {
        GraveYardData const& data = range.first->second;

        WorldSafeLocsEntry const* entry = sWorldSafeLocsStore.LookupEntry(data.safeLocId);
        if (!entry)
        {
            TC_LOG_ERROR("sql.sql", "Table `game_graveyard_zone` has record for not existing graveyard (WorldSafeLocs.dbc id) %u, skipped.", data.safeLocId);
            continue;
        }

        // skip enemy faction graveyard
        // team == 0 case can be at call from .neargrave
        if (data.team != 0 && team != 0 && data.team != team)
            continue;

        // skip graveyards in The Lost Isles used for previous phases (TODO: move it to conditions)
        if ((data.safeLocId == 1704 || data.safeLocId == 1705) && zoneId == 4720 && areaId == 4924)
            continue;

        // not in skyfire,maybe hack
        // if (conditionObject && !sConditionMgr->IsObjectMeetToConditions(conditionSource, sConditionMgr->GetConditionsForNotGroupedEntry(CONDITION_SOURCE_TYPE_GRAVEYARD, data.safeLocId)))
        // if (conditionObject && !sConditionMgr->IsObjectMeetToConditions(conditionSource, IsObjectMeetingNotGroupedConditions(CONDITION_SOURCE_TYPE_GRAVEYARD, data.safeLocId, ConditionSourceInfo& sourceInfo)))
        //     continue;

        // find now nearest graveyard at other map
        if (MapId != entry->map_id)
        {
            // if find graveyard at different map from where entrance placed (or no entrance data), use any first
            if (!mapEntry
                || mapEntry->entrance_map < 0
                || uint32(mapEntry->entrance_map) != entry->map_id
                || (mapEntry->entrance_x == 0 && mapEntry->entrance_y == 0))
            {
                // not have any corrdinates for check distance anyway
                entryFar = entry;
                continue;
            }

            // at entrance map calculate distance (2D);
            float dist2 = (entry->x - mapEntry->entrance_x)*(entry->x - mapEntry->entrance_x)
                +(entry->y - mapEntry->entrance_y)*(entry->y - mapEntry->entrance_y);
            if (foundEntr)
            {
                if (dist2 < distEntr)
                {
                    distEntr = dist2;
                    entryEntr = entry;
                }
            }
            else
            {
                foundEntr = true;
                distEntr = dist2;
                entryEntr = entry;
            }
        }
        // find now nearest graveyard at same map
        else
        {
            float dist2 = (entry->x - x)*(entry->x - x)+(entry->y - y)*(entry->y - y)+(entry->z - z)*(entry->z - z);
            if (foundNear)
            {
                if (dist2 < distNear)
                {
                    distNear = dist2;
                    entryNear = entry;
                }
            }
            else
            {
                foundNear = true;
                distNear = dist2;
                entryNear = entry;
            }
        }
    }

    if (entryNear)
        return entryNear;

    if (entryEntr)
        return entryEntr;

    return entryFar;
}

GraveYardData const* ObjectMgr::FindGraveYardData(uint32 id, uint32 zoneId)
{
    GraveYardMapBounds range = GraveYardStore.equal_range(zoneId);
    for (; range.first != range.second; ++range.first)
    {
        GraveYardData const& data = range.first->second;
        if (data.safeLocId == id)
            return &data;
    }
    return nullptr;
}

AreaTriggerStruct const* ObjectMgr::GetAreaTrigger(uint32 trigger) const
{
    AreaTriggerContainer::const_iterator itr = _areaTriggerStore.find(trigger);
    if (itr != _areaTriggerStore.end())
        return &itr->second;
    return nullptr;
}

AccessRequirement const* ObjectMgr::GetAccessRequirement(uint32 mapid, Difficulty difficulty) const
{
    AccessRequirementContainer::const_iterator itr = _accessRequirementStore.find(MAKE_PAIR32(mapid, difficulty));
    if (itr != _accessRequirementStore.end())
        return itr->second;
    return nullptr;    
}

bool ObjectMgr::AddGraveYardLink(uint32 id, uint32 zoneId, uint32 team, bool persist /*= true*/)
{
    if (FindGraveYardData(id, zoneId))
        return false;

    // add link to loaded data
    GraveYardData data;
    data.safeLocId = id;
    data.team = team;

    GraveYardStore.insert(GraveYardContainer::value_type(zoneId, data));

    // add link to DB
    if (persist)
    {
        WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_INS_GRAVEYARD_ZONE);

        stmt->setUInt32(0, id);
        stmt->setUInt32(1, zoneId);
        stmt->setUInt16(2, uint16(team));

        WorldDatabase.Execute(stmt);
    }

    return true;
}

void ObjectMgr::RemoveGraveYardLink(uint32 id, uint32 zoneId, uint32 team, bool persist /*= false*/)
{
    GraveYardMapBoundsNonConst range = GraveYardStore.equal_range(zoneId);
    if (range.first == range.second)
    {
        //TC_LOG_ERROR("sql.sql", "Table `game_graveyard_zone` incomplete: Zone %u Team %u does not have a linked graveyard.", zoneId, team);
        return;
    }

    bool found = false;


    for (; range.first != range.second; ++range.first)
    {
        GraveYardData & data = range.first->second;

        // skip not matching safezone id
        if (data.safeLocId != id)
            continue;

        // skip enemy faction graveyard at same map (normal area, city, or battleground)
        // team == 0 case can be at call from .neargrave
        if (data.team != 0 && team != 0 && data.team != team)
            continue;

        found = true;
        break;
    }

    // no match, return
    if (!found)
        return;

    // remove from links
    GraveYardStore.erase(range.first);

    // remove link from DB
    if (persist)
    {
        WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_GRAVEYARD_ZONE);

        stmt->setUInt32(0, id);
        stmt->setUInt32(1, zoneId);
        stmt->setUInt16(2, uint16(team));

        WorldDatabase.Execute(stmt);
    }
}

void ObjectMgr::LoadAreaTriggerTeleports()
{
    uint32 oldMSTime = getMSTime();

    _areaTriggerStore.clear();                                  // need for reload case

    //                                                        0            1                  2                  3                  4                   5
    QueryResult result = WorldDatabase.Query("SELECT id,  target_map, target_position_x, target_position_y, target_position_z, target_orientation FROM areatrigger_teleport");
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 area trigger teleport definitions. DB table `areatrigger_teleport` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        ++count;

        uint32 Trigger_ID = fields[0].GetUInt32();

        AreaTriggerStruct at;

        at.target_mapId             = fields[1].GetUInt16();
        at.target_X                 = fields[2].GetFloat();
        at.target_Y                 = fields[3].GetFloat();
        at.target_Z                 = fields[4].GetFloat();
        at.target_Orientation       = fields[5].GetFloat();

        AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(Trigger_ID);
        if (!atEntry)
        {
            TC_LOG_ERROR("sql.sql", "Area trigger (ID:%u) does not exist in `AreaTrigger.dbc`.", Trigger_ID);
            continue;
        }

        MapEntry const* mapEntry = sMapStore.LookupEntry(at.target_mapId);
        if (!mapEntry)
        {
            TC_LOG_ERROR("sql.sql", "Area trigger (ID:%u) target map (ID: %u) does not exist in `Map.dbc`.", Trigger_ID, at.target_mapId);
            continue;
        }

        if (at.target_X == 0 && at.target_Y == 0 && at.target_Z == 0)
        {
            TC_LOG_ERROR("sql.sql", "Area trigger (ID:%u) target coordinates not provided.", Trigger_ID);
            continue;
        }

        _areaTriggerStore[Trigger_ID] = at;

    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u area trigger teleport definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadAccessRequirements()
{
    uint32 oldMSTime = getMSTime();

    if (!_accessRequirementStore.empty())
    {
        for (AccessRequirementContainer::iterator itr = _accessRequirementStore.begin(); itr != _accessRequirementStore.end(); ++itr)
            delete itr->second;

        _accessRequirementStore.clear();                                  // need for reload case
    }

    //                                                 0        1           2          3          4      5         6             7                 8                  9
    QueryResult result = WorldDatabase.Query("SELECT mapid, difficulty+0, level_min, level_max, item, item2, quest_done_A, quest_done_H, completed_achievement, quest_failed_text FROM access_requirement");
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 access requirement definitions. DB table `access_requirement` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        ++count;

        uint32 mapid = fields[0].GetUInt32();
        uint8 difficulty = fields[1].GetUInt8();
        uint32 requirement_ID = MAKE_PAIR32(mapid, difficulty);

        AccessRequirement* ar = new AccessRequirement();

        ar->levelMin        = fields[2].GetUInt8();
        ar->levelMax        = fields[3].GetUInt8();
        ar->item            = fields[4].GetUInt32();
        ar->item2           = fields[5].GetUInt32();
        ar->quest_A         = fields[6].GetUInt32();
        ar->quest_H         = fields[7].GetUInt32();
        ar->achievement     = fields[8].GetUInt32();
        ar->questFailedText = fields[9].GetString();

        if (ar->item)
        {
            ItemTemplate const* pProto = GetItemTemplate(ar->item);
            if (!pProto)
            {
                TC_LOG_ERROR("misc", "Key item %u does not exist for map %u difficulty %u, removing key requirement.", ar->item, mapid, difficulty);
                ar->item = 0;
            }
        }

        if (ar->item2)
        {
            ItemTemplate const* pProto = GetItemTemplate(ar->item2);
            if (!pProto)
            {
                TC_LOG_ERROR("misc", "Second item %u does not exist for map %u difficulty %u, removing key requirement.", ar->item2, mapid, difficulty);
                ar->item2 = 0;
            }
        }

        if (ar->quest_A)
        {
            if (!GetQuestTemplate(ar->quest_A))
            {
                TC_LOG_ERROR("sql.sql", "Required Alliance Quest %u not exist for map %u difficulty %u, remove quest done requirement.", ar->quest_A, mapid, difficulty);
                ar->quest_A = 0;
            }
        }

        if (ar->quest_H)
        {
            if (!GetQuestTemplate(ar->quest_H))
            {
                TC_LOG_ERROR("sql.sql", "Required Horde Quest %u not exist for map %u difficulty %u, remove quest done requirement.", ar->quest_H, mapid, difficulty);
                ar->quest_H = 0;
            }
        }

        if (ar->achievement)
        {
            if (!sAchievementMgr->GetAchievement(ar->achievement))
            {
                TC_LOG_ERROR("sql.sql", "Required Achievement %u not exist for map %u difficulty %u, remove quest done requirement.", ar->achievement, mapid, difficulty);
                ar->achievement = 0;
            }
        }

        _accessRequirementStore[requirement_ID] = ar;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u access requirement definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

/*
 * Searches for the areatrigger which teleports players out of the given map with instance_template.parent field support
 */
AreaTriggerStruct const* ObjectMgr::GetGoBackTrigger(uint32 Map) const
{
    bool useParentDbValue = false;
    uint32 parentId = 0;
    const MapEntry* mapEntry = sMapStore.LookupEntry(Map);
    if (!mapEntry || mapEntry->entrance_map < 0)
        return NULL;

    if (mapEntry->IsDungeon())
    {
        const InstanceTemplate* iTemplate = sObjectMgr->GetInstanceTemplate(Map);

        if (!iTemplate)
            return NULL;

        parentId = iTemplate->Parent;
        useParentDbValue = true;
    }

    uint32 entrance_map = uint32(mapEntry->entrance_map);
    for (AreaTriggerContainer::const_iterator itr = _areaTriggerStore.begin(); itr != _areaTriggerStore.end(); ++itr)
        if ((!useParentDbValue && itr->second.target_mapId == entrance_map) || (useParentDbValue && itr->second.target_mapId == parentId))
        {
            AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(itr->first);
            if (atEntry && atEntry->ContinentID == Map)
                return &itr->second;
        }
    return NULL;
}

/**
 * Searches for the areatrigger which teleports players to the given map
 */
AreaTriggerStruct const* ObjectMgr::GetMapEntranceTrigger(uint32 Map) const
{
    for (AreaTriggerContainer::const_iterator itr = _areaTriggerStore.begin(); itr != _areaTriggerStore.end(); ++itr)
    {
        if (itr->second.target_mapId == Map)
        {
            AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(itr->first);
            if (atEntry)
                return &itr->second;
        }
    }
    return NULL;
}

void ObjectMgr::SetHighestGuids()
{
    QueryResult result = CharacterDatabase.Query("SELECT MAX(guid) FROM characters");
    if (result)
        GetGuidSequenceGenerator<HighGuid::Player>().Set((*result)[0].GetUInt32() + 1);

    result = WorldDatabase.Query("SELECT MAX(guid) FROM creature");
    if (result)
        _creatureSpawnId = (*result)[0].GetUInt32() + 1;

    result = CharacterDatabase.Query("SELECT MAX(guid) FROM item_instance");
    if (result)
        GetGuidSequenceGenerator<HighGuid::Item>().Set((*result)[0].GetUInt32() + 1);

    // Cleanup other tables from not existed guids ( >= _hiItemGuid)
    uint32 hiItemGuid = GetGuidSequenceGenerator<HighGuid::Item>().GetNextAfterMaxUsed();
    CharacterDatabase.PExecute("DELETE FROM character_inventory WHERE item >= '%u'", hiItemGuid);      // One-time query
    CharacterDatabase.PExecute("DELETE FROM mail_items WHERE item_guid >= '%u'", hiItemGuid);          // One-time query
    CharacterDatabase.PExecute("DELETE FROM auctionhouse WHERE itemguid >= '%u'", hiItemGuid);         // One-time query
    CharacterDatabase.PExecute("DELETE FROM guild_bank_item WHERE item_guid >= '%u'", hiItemGuid);     // One-time query

    result = WorldDatabase.Query("SELECT MAX(guid) FROM gameobject");
    if (result)
        _gameObjectSpawnId = (*result)[0].GetUInt32() + 1;

    result = WorldDatabase.Query("SELECT MAX(guid) FROM transports");
    if (result)
        GetGuidSequenceGenerator<HighGuid::Mo_Transport>().Set((*result)[0].GetUInt32() + 1);

    result = CharacterDatabase.Query("SELECT MAX(id) FROM auctionhouse");
    if (result)
        _auctionId = (*result)[0].GetUInt32() + 1;

    result = CharacterDatabase.Query("SELECT MAX(id) FROM mail");
    if (result)
        _mailId = (*result)[0].GetUInt32() + 1;

    result = LoginDatabase.PQuery("SELECT MAX(id) FROM account_muted WHERE realmid = '%u'", realm.Id.Realm);
    if (result)
        _muteId = (*result)[0].GetUInt32() + 1;

//    result = CharacterDatabase.Query("SELECT MAX(corpseGuid) FROM corpse");
//    if (result)
//        _hiCorpseGuid = (*result)[0].GetUInt32()+1;

    result = CharacterDatabase.Query("SELECT MAX(setguid) FROM character_equipmentsets");
    if (result)
        _equipmentSetGuid = (*result)[0].GetUInt64()+1;

    result = CharacterDatabase.Query("SELECT MAX(guildId) FROM guild");
    if (result)
        sGuildMgr->SetNextGuildId((*result)[0].GetUInt32()+1);

    result = CharacterDatabase.Query("SELECT MAX(guid) FROM `groups`");
    if (result)
        sGroupMgr->SetGroupDbStoreSize((*result)[0].GetUInt32()+1);

    result = CharacterDatabase.Query("SELECT MAX(itemId) from character_void_storage");
    if (result)
        _voidItemId = (*result)[0].GetUInt64()+1;

    result = CharacterDatabase.Query("SELECT MAX(id) FROM account_battle_pet");
    if (result)
        _battlePetId = (*result)[0].GetUInt64() + 1;
}

uint32 ObjectMgr::GenerateAuctionID()
{
    if (_auctionId >= 0xFFFFFFFE)
    {
        TC_LOG_ERROR("misc", "Auctions ids overflow!! Can't continue, shutting down server. ");
        World::StopNow(ERROR_EXIT_CODE);
    }
    return _auctionId++;
}

uint64 ObjectMgr::GenerateEquipmentSetGuid()
{
    if (_equipmentSetGuid >= uint64(0xFFFFFFFFFFFFFFFELL))
    {
        TC_LOG_ERROR("misc", "EquipmentSet guid overflow!! Can't continue, shutting down server. ");
        World::StopNow(ERROR_EXIT_CODE);
    }
    return _equipmentSetGuid++;
}

uint32 ObjectMgr::GenerateMailID()
{
    if (_mailId >= 0xFFFFFFFE)
    {
        TC_LOG_ERROR("misc", "Mail ids overflow!! Can't continue, shutting down server. ");
        World::StopNow(ERROR_EXIT_CODE);
    }
    return _mailId++;
}

uint32 ObjectMgr::GenerateMuteID()
{
    if (_muteId >= 0xFFFFFFFE)
    {
        TC_LOG_ERROR("misc", "Mute ids overflow!! Can't continue, shutting down server. ");
        World::StopNow(ERROR_EXIT_CODE);
    }
    return _muteId++;
}

uint64 ObjectMgr::GenerateBattlePetId()
{
    if (_battlePetId >= uint64(0xFFFFFFFFFFFFFFFELL))
    {
        TC_LOG_ERROR("misc", "BattlePetId Id overflow!! Can't continue, shutting down server. ");
        World::StopNow(ERROR_EXIT_CODE);
    }
    return ++_battlePetId;
}

void ObjectMgr::LoadGameObjectLocales()
{
    uint32 oldMSTime = getMSTime();

    _gameObjectLocaleStore.clear(); // need for reload case

    //                                               0      1       2     3
    QueryResult result = WorldDatabase.Query("SELECT entry, locale, name, castBarCaption FROM gameobject_template_locale");
    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();

        uint32 id                   = fields[0].GetUInt32();
        std::string localeName      = fields[1].GetString();

        LocaleConstant locale = GetLocaleByName(localeName);
        if (!sWorld->getBoolConfig(CONFIG_LOAD_LOCALES) || locale == LOCALE_enUS)
            continue;

        GameObjectLocale& data = _gameObjectLocaleStore[id];
        AddLocaleString(fields[2].GetString(), locale, data.Name);
        AddLocaleString(fields[3].GetString(), locale, data.CastBarCaption);
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u gameobject_template_locale strings in %u ms", uint32(_gameObjectLocaleStore.size()), GetMSTimeDiffToNow(oldMSTime));
}

inline void CheckGOLockId(GameObjectTemplate const* goInfo, uint32 dataN, uint32 N)
{
    if (sLockStore.LookupEntry(dataN))
        return;

    TC_LOG_ERROR("sql.sql", "Gameobject (Entry: %u GoType: %u) have data%d=%u but lock (Id: %u) not found.",
        goInfo->entry, goInfo->type, N, goInfo->door.lockId, goInfo->door.lockId);
}

inline void CheckGOLinkedTrapId(GameObjectTemplate const* goInfo, uint32 dataN, uint32 N)
{
    if (GameObjectTemplate const* trapInfo = sObjectMgr->GetGameObjectTemplate(dataN))
    {
        if (trapInfo->type != GAMEOBJECT_TYPE_TRAP)
            TC_LOG_ERROR("sql.sql", "Gameobject (Entry: %u GoType: %u) have data%d=%u but GO (Entry %u) have not GAMEOBJECT_TYPE_TRAP (%u) type.",
            goInfo->entry, goInfo->type, N, dataN, dataN, GAMEOBJECT_TYPE_TRAP);
    }
}

inline void CheckGOSpellId(GameObjectTemplate const* goInfo, uint32 dataN, uint32 N)
{
    if (sSpellMgr->GetSpellInfo(dataN))
        return;

    TC_LOG_ERROR("sql.sql", "Gameobject (Entry: %u GoType: %u) have data%d=%u but Spell (Entry %u) not exist.",
        goInfo->entry, goInfo->type, N, dataN, dataN);
}

inline void CheckAndFixGOChairHeightId(GameObjectTemplate const* goInfo, uint32& dataN, uint32 N)
{
    if (dataN <= (UNIT_STAND_STATE_SIT_HIGH_CHAIR-UNIT_STAND_STATE_SIT_LOW_CHAIR))
        return;

    TC_LOG_ERROR("sql.sql", "Gameobject (Entry: %u GoType: %u) have data%d=%u but correct chair height in range 0..%i.",
        goInfo->entry, goInfo->type, N, dataN, UNIT_STAND_STATE_SIT_HIGH_CHAIR-UNIT_STAND_STATE_SIT_LOW_CHAIR);

    // prevent client and server unexpected work
    dataN = 0;
}

inline void CheckGONoDamageImmuneId(GameObjectTemplate* goTemplate, uint32 dataN, uint32 N)
{
    // 0/1 correct values
    if (dataN <= 1)
        return;

    TC_LOG_ERROR("sql.sql", "Gameobject (Entry: %u GoType: %u) have data%d=%u but expected boolean (0/1) noDamageImmune field value.", goTemplate->entry, goTemplate->type, N, dataN);
}

inline void CheckGOConsumable(GameObjectTemplate const* goInfo, uint32 dataN, uint32 N)
{
    // 0/1 correct values
    if (dataN <= 1)
        return;

    TC_LOG_ERROR("sql.sql", "Gameobject (Entry: %u GoType: %u) have data%d=%u but expected boolean (0/1) consumable field value.",
        goInfo->entry, goInfo->type, N, dataN);
}

void ObjectMgr::LoadGameObjectTemplate()
{
    uint32 oldMSTime = getMSTime();

    //                                                 0      1      2        3       4             5          6     7       8            9        10
    QueryResult result = WorldDatabase.Query("SELECT entry, type, displayId, name, IconName, castBarCaption, unk1, size, questItem1, questItem2, questItem3, "
    //                                            11          12          13       14     15     16     17     18     19     20     21     22     23      24      25      26
                                             "questItem4, questItem5, questItem6, data0, data1, data2, data3, data4, data5, data6, data7, data8, data9, data10, data11, data12, "
    //                                          27      28      29      30      31      32      33      34      35     36      37      38      39      40      41      42
                                             "data13, data14, data15, data16, data17, data18, data19, data20, data21, data22, data23, data24, data25, data26, data27, data28, "
    //                                          43      44      45       46       47        48
                                             "data29, data30, data31, unkInt32, AIName, ScriptName "
                                             "FROM gameobject_template");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 gameobject definitions. DB table `gameobject_template` is empty.");
        return;
    }

    _gameObjectTemplateStore.rehash(result->GetRowCount());
    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        GameObjectTemplate& got = _gameObjectTemplateStore[entry];

        got.entry          = entry;
        got.type           = uint32(fields[1].GetUInt8());
        got.displayId      = fields[2].GetUInt32();
        got.name           = fields[3].GetString();
        got.IconName       = fields[4].GetString();
        got.castBarCaption = fields[5].GetString();
        got.unk1           = fields[6].GetString();
        got.size           = fields[7].GetFloat();

        for (uint8 i = 0; i < MAX_GAMEOBJECT_QUEST_ITEMS; ++i)
            got.questItems[i] = fields[8 + i].GetUInt32();

        for (uint8 i = 0; i < MAX_GAMEOBJECT_DATA; ++i)
            got.raw.data[i] = fields[14 + i].GetUInt32();

        got.unkInt32 = fields[46].GetInt32();
        got.AIName = fields[47].GetString();
        got.ScriptId = GetScriptId(fields[48].GetString());

        // Checks

        switch (got.type)
        {
            case GAMEOBJECT_TYPE_DOOR:                      //0
            {
                if (got.door.lockId)
                    CheckGOLockId(&got, got.door.lockId, 1);
                CheckGONoDamageImmuneId(&got, got.door.noDamageImmune, 3);
                break;
            }
            case GAMEOBJECT_TYPE_BUTTON:                    //1
            {
                if (got.button.lockId)
                    CheckGOLockId(&got, got.button.lockId, 1);
                CheckGONoDamageImmuneId(&got, got.button.noDamageImmune, 4);
                break;
            }
            case GAMEOBJECT_TYPE_QUESTGIVER:                //2
            {
                if (got.questgiver.lockId)
                    CheckGOLockId(&got, got.questgiver.lockId, 0);
                CheckGONoDamageImmuneId(&got, got.questgiver.noDamageImmune, 5);
                break;
            }
            case GAMEOBJECT_TYPE_CHEST:                     //3
            {
                if (got.chest.lockId)
                    CheckGOLockId(&got, got.chest.lockId, 0);

                CheckGOConsumable(&got, got.chest.consumable, 3);

                if (got.chest.linkedTrapId)              // linked trap
                    CheckGOLinkedTrapId(&got, got.chest.linkedTrapId, 7);
                break;
            }
            case GAMEOBJECT_TYPE_TRAP:                      //6
            {
                if (got.trap.lockId)
                    CheckGOLockId(&got, got.trap.lockId, 0);
                break;
            }
            case GAMEOBJECT_TYPE_CHAIR:                     //7
                CheckAndFixGOChairHeightId(&got, got.chair.height, 1);
                break;
            case GAMEOBJECT_TYPE_SPELL_FOCUS:               //8
            {
                if (got.spellFocus.focusId)
                {
                    if (!sSpellFocusObjectStore.LookupEntry(got.spellFocus.focusId))
                        TC_LOG_ERROR("sql.sql", "GameObject (Entry: %u GoType: %u) have data0=%u but SpellFocus (Id: %u) not exist.",
                        entry, got.type, got.spellFocus.focusId, got.spellFocus.focusId);
                }

                if (got.spellFocus.linkedTrapId)        // linked trap
                    CheckGOLinkedTrapId(&got, got.spellFocus.linkedTrapId, 2);
                break;
            }
            case GAMEOBJECT_TYPE_GOOBER:                    //10
            {
                if (got.goober.lockId)
                    CheckGOLockId(&got, got.goober.lockId, 0);

                CheckGOConsumable(&got, got.goober.consumable, 3);

                if (got.goober.pageId)                  // pageId
                {
                    if (!GetPageText(got.goober.pageId))
                        TC_LOG_ERROR("sql.sql", "GameObject (Entry: %u GoType: %u) have data7=%u but PageText (Entry %u) not exist.",
                        entry, got.type, got.goober.pageId, got.goober.pageId);
                }
                CheckGONoDamageImmuneId(&got, got.goober.noDamageImmune, 11);
                if (got.goober.linkedTrapId)            // linked trap
                    CheckGOLinkedTrapId(&got, got.goober.linkedTrapId, 12);
                break;
            }
            case GAMEOBJECT_TYPE_AREADAMAGE:                //12
            {
                if (got.areadamage.lockId)
                    CheckGOLockId(&got, got.areadamage.lockId, 0);
                break;
            }
            case GAMEOBJECT_TYPE_CAMERA:                    //13
            {
                if (got.camera.lockId)
                    CheckGOLockId(&got, got.camera.lockId, 0);
                break;
            }
            case GAMEOBJECT_TYPE_MO_TRANSPORT:              //15
            {
                if (got.moTransport.taxiPathId)
                {
                    if (got.moTransport.taxiPathId >= sTaxiPathNodesByPath.size() || sTaxiPathNodesByPath[got.moTransport.taxiPathId].empty())
                        TC_LOG_ERROR("sql.sql", "GameObject (Entry: %u GoType: %u) have data0=%u but TaxiPath (Id: %u) not exist.",
                        entry, got.type, got.moTransport.taxiPathId, got.moTransport.taxiPathId);
                }
                if (uint32 transportMap = got.moTransport.mapID)
                    _transportMaps.insert(transportMap);
                break;
            }
            case GAMEOBJECT_TYPE_SUMMONING_RITUAL:          //18
                break;
            case GAMEOBJECT_TYPE_SPELLCASTER:               //22
            {
                // always must have spell
                CheckGOSpellId(&got, got.spellcaster.spellId, 0);
                break;
            }
            case GAMEOBJECT_TYPE_FLAGSTAND:                 //24
            {
                if (got.flagstand.lockId)
                    CheckGOLockId(&got, got.flagstand.lockId, 0);
                CheckGONoDamageImmuneId(&got, got.flagstand.noDamageImmune, 5);
                break;
            }
            case GAMEOBJECT_TYPE_FISHINGHOLE:               //25
            {
                if (got.fishinghole.lockId)
                    CheckGOLockId(&got, got.fishinghole.lockId, 4);
                break;
            }
            case GAMEOBJECT_TYPE_FLAGDROP:                  //26
            {
                if (got.flagdrop.lockId)
                    CheckGOLockId(&got, got.flagdrop.lockId, 0);
                CheckGONoDamageImmuneId(&got, got.flagdrop.noDamageImmune, 3);
                break;
            }
            case GAMEOBJECT_TYPE_BARBER_CHAIR:              //32
                CheckAndFixGOChairHeightId(&got, got.barberChair.chairheight, 0);
                break;
        }

       ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u game object templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadGameObjectTemplateAddons()
{
    uint32 oldMSTime = getMSTime();

    //                                                0       1       2      3        4           5
    QueryResult result = WorldDatabase.Query("SELECT entry, faction, flags, mingold, maxgold, maxDuplicates FROM gameobject_template_addon");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 gameobject template addon definitions. DB table `gameobject_template_addon` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        GameObjectTemplate const* got = sObjectMgr->GetGameObjectTemplate(entry);
        if (!got)
        {
            TC_LOG_ERROR("sql.sql", "GameObject template (Entry: %u) does not exist but has a record in `gameobject_template_addon`", entry);
            continue;
        }

        GameObjectTemplateAddon& gameObjectAddon = _gameObjectTemplateAddonStore[entry];
        gameObjectAddon.faction = uint32(fields[1].GetUInt16());
        gameObjectAddon.flags   = fields[2].GetUInt32();
        gameObjectAddon.mingold = fields[3].GetUInt32();
        gameObjectAddon.maxgold = fields[4].GetUInt32();
        gameObjectAddon.maxDuplicates = fields[5].GetUInt8();

        // checks
        if (gameObjectAddon.faction && !sFactionTemplateStore.LookupEntry(gameObjectAddon.faction))
            TC_LOG_ERROR("sql.sql", "GameObject (Entry: %u) has invalid faction (%u) defined in `gameobject_template_addon`.", entry, gameObjectAddon.faction);

        if (gameObjectAddon.maxgold > 0)
        {
            switch (got->type)
            {
                case GAMEOBJECT_TYPE_CHEST:
                case GAMEOBJECT_TYPE_FISHINGHOLE:
                    break;
                default:
                    TC_LOG_ERROR("sql.sql", "GameObject (Entry %u GoType: %u) cannot be looted but has maxgold set in `gameobject_template_addon`.", entry, got->type);
                    break;
            }
        }

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u game object template addons in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadExplorationBaseXP()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT level, basexp FROM exploration_basexp");

    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 BaseXP definitions. DB table `exploration_basexp` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        uint8 level  = fields[0].GetUInt8();
        uint32 basexp = fields[1].GetInt32();
        _baseXPTable[level] = basexp;
        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u BaseXP definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

uint32 ObjectMgr::GetBaseXP(uint8 level)
{
    return _baseXPTable[level] ? _baseXPTable[level] : 0;
}

uint32 ObjectMgr::GetXPForLevel(uint8 level) const
{
    if (level < _playerXPperLevel.size())
        return _playerXPperLevel[level];
    return 0;
}

void ObjectMgr::LoadPetNames()
{
    uint32 oldMSTime = getMSTime();
    //                                                0     1      2
    QueryResult result = WorldDatabase.Query("SELECT word, entry, half FROM pet_name_generation");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 pet name parts. DB table `pet_name_generation` is empty!");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        std::string word = fields[0].GetString();
        uint32 entry     = fields[1].GetUInt32();
        bool   half      = fields[2].GetBool();
        if (half)
            _petHalfName1[entry].push_back(word);
        else
            _petHalfName0[entry].push_back(word);
        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u pet name parts in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadPetNumber()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = CharacterDatabase.Query("SELECT MAX(id) FROM character_pet");
    if (result)
    {
        Field* fields = result->Fetch();
        _hiPetNumber = fields[0].GetUInt32()+1;
    }

    TC_LOG_INFO("server.loading", ">> Loaded the max pet number: %d in %u ms", _hiPetNumber-1, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadGuildChallengeRewardInfo()
{
    // No need to keep this in DB
    const uint32 GuildChallengeWeeklyMaximum[] = { 0, 7, 1, 3, 15, 3 };
    const uint32 GuildChallengeGoldReward[] = { 0, 125, 500, 250, 125, 250 };
    const uint32 GuildChallengeMaxLevelGoldReward[] = { 0, 250, 1000, 500, 250, 500 };
    const uint32 GuildChallengeXPReward[] = { 0, 300000, 3000000, 1500000, 50000, 1000000 };

    _challengeRewardData.resize(CHALLENGE_MAX);

    for (int32 i = CHALLENGE_NONE; i < CHALLENGE_MAX; ++i)
    {
        auto& data = _challengeRewardData[i];
        data.ChallengeCount = GuildChallengeWeeklyMaximum[i];
        data.Experience = GuildChallengeXPReward[i];
        data.MaxLevelGold = GuildChallengeMaxLevelGoldReward[i];
        data.Gold = GuildChallengeGoldReward[i];
    }
}

void ObjectMgr::LoadPetScaling()
{
    _petScaling.clear();

    uint32 count = 0;
    uint32 starttime = getMSTime();
    //                                           0        1           2           3            4          5             6
    auto result = WorldDatabase.PQuery("SELECT entry, ap_per_spd, ap_per_ap, spd_per_spd, spd_per_ap, hp_per_hp, base_damage_coeff FROM pet_scaling");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 entry = fields[0].GetUInt32();
            if (!GetCreatureTemplate(entry))
            {
                TC_LOG_ERROR("sql.sql", "Table pet_scaling has entry for the non existing creature: %i", entry);
                continue;
            }
            PetScaling& scaling = _petScaling[entry];
            scaling.ApPerSpd = fields[1].GetFloat();
            scaling.ApPerAp = fields[2].GetFloat();
            scaling.SpdPerSpd = fields[3].GetFloat();
            scaling.SpdPerAp = fields[4].GetFloat();
            scaling.HpPerHp = fields[5].GetFloat();
            scaling.BaseDamageCoeff = fields[6].GetFloat();
            ++count;
        } while (result->NextRow());
    }
    TC_LOG_INFO("server.loading", "Loaded %u pet scaing in %u ms", count, GetMSTimeDiffToNow(starttime));
}

std::string ObjectMgr::GeneratePetName(uint32 entry)
{
    StringVector& list0 = _petHalfName0[entry];
    StringVector& list1 = _petHalfName1[entry];

    if (list0.empty() || list1.empty())
    {
        CreatureTemplate const* cinfo = GetCreatureTemplate(entry);
        if (!cinfo)
            return std::string();

        char const* petname = GetPetName(cinfo->family, sWorld->GetDefaultDbcLocale());
        if (petname)
            return std::string(petname);
        else
            return cinfo->Name;
    }

    return *(list0.begin()+urand(0, list0.size()-1)) + *(list1.begin()+urand(0, list1.size()-1));
}

uint32 ObjectMgr::GeneratePetNumber()
{
    if (_hiPetNumber >= 0xFFFFFFFE)
    {
        TC_LOG_ERROR("misc", "_hiPetNumber Id overflow!! Can't continue, shutting down server. ");
        World::StopNow(ERROR_EXIT_CODE);
    }
    return ++_hiPetNumber;
}

uint64 ObjectMgr::GenerateVoidStorageItemId()
{
    if (_voidItemId >= uint64(0xFFFFFFFFFFFFFFFELL))
    {
        TC_LOG_ERROR("misc", "VoidStorageItem Id overflow!! Can't continue, shutting down server. ");
        World::StopNow(ERROR_EXIT_CODE);
    }
    return ++_voidItemId;
}

uint64 ObjectMgr::GenerateCreatureSpawnId()
{
    if (_creatureSpawnId >= uint64(0xFFFFFFFFFFFFFFFELL))
    {
        TC_LOG_ERROR("misc", "Creature spawn id overflow!! Can't continue, shutting down server. Search on forum for TCE00007 for more info.");
        World::StopNow(ERROR_EXIT_CODE);
    }
    return _creatureSpawnId++;
}

uint64 ObjectMgr::GenerateGameObjectSpawnId()
{
    if (_gameObjectSpawnId >= uint64(0xFFFFFFFFFFFFFFFELL))
    {
        TC_LOG_ERROR("misc", "Game object spawn id overflow!! Can't continue, shutting down server. Search on forum for TCE00007 for more info. ");
        World::StopNow(ERROR_EXIT_CODE);
    }
    return _gameObjectSpawnId++;
}

void ObjectMgr::LoadReputationRewardRate()
{
    uint32 oldMSTime = getMSTime();

    _repRewardRateStore.clear();                             // for reload case

    uint32 count = 0; //                                0          1             2                  3                  4                 5                      6             7
    QueryResult result = WorldDatabase.Query("SELECT faction, quest_rate, quest_daily_rate, quest_weekly_rate, quest_monthly_rate, quest_repeatable_rate, creature_rate, spell_rate FROM reputation_reward_rate");
    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded `reputation_reward_rate`, table is empty!");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 factionId            = fields[0].GetUInt32();

        RepRewardRate repRate;

        repRate.questRate           = fields[1].GetFloat();
        repRate.questDailyRate      = fields[2].GetFloat();
        repRate.questWeeklyRate     = fields[3].GetFloat();
        repRate.questMonthlyRate    = fields[4].GetFloat();
        repRate.questRepeatableRate = fields[5].GetFloat();
        repRate.creatureRate        = fields[6].GetFloat();
        repRate.spellRate           = fields[7].GetFloat();

        FactionEntry const* factionEntry = sFactionStore.LookupEntry(factionId);
        if (!factionEntry)
        {
            TC_LOG_ERROR("sql.sql", "Faction (faction.dbc) %u does not exist but is used in `reputation_reward_rate`", factionId);
            continue;
        }

        if (repRate.questRate < 0.0f)
        {
            TC_LOG_ERROR("sql.sql", "Table reputation_reward_rate has quest_rate with invalid rate %f, skipping data for faction %u", repRate.questRate, factionId);
            continue;
        }

        if (repRate.questDailyRate < 0.0f)
        {
            TC_LOG_ERROR("sql.sql", "Table reputation_reward_rate has quest_daily_rate with invalid rate %f, skipping data for faction %u", repRate.questDailyRate, factionId);
            continue;
        }

        if (repRate.questWeeklyRate < 0.0f)
        {
            TC_LOG_ERROR("sql.sql", "Table reputation_reward_rate has quest_weekly_rate with invalid rate %f, skipping data for faction %u", repRate.questWeeklyRate, factionId);
            continue;
        }

        if (repRate.questMonthlyRate < 0.0f)
        {
            TC_LOG_ERROR("sql.sql", "Table reputation_reward_rate has quest_monthly_rate with invalid rate %f, skipping data for faction %u", repRate.questMonthlyRate, factionId);
            continue;
        }

        if (repRate.questRepeatableRate < 0.0f)
        {
            TC_LOG_ERROR("sql.sql", "Table reputation_reward_rate has quest_repeatable_rate with invalid rate %f, skipping data for faction %u", repRate.questRepeatableRate, factionId);
            continue;
        }

        if (repRate.creatureRate < 0.0f)
        {
            TC_LOG_ERROR("sql.sql", "Table reputation_reward_rate has creature_rate with invalid rate %f, skipping data for faction %u", repRate.creatureRate, factionId);
            continue;
        }

        if (repRate.spellRate < 0.0f)
        {
            TC_LOG_ERROR("sql.sql", "Table reputation_reward_rate has spell_rate with invalid rate %f, skipping data for faction %u", repRate.spellRate, factionId);
            continue;
        }

        _repRewardRateStore[factionId] = repRate;

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u reputation_reward_rate in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadReputationOnKill()
{
    uint32 oldMSTime = getMSTime();

    // For reload case
    _repOnKillStore.clear();

    uint32 count = 0;

    //                                                0            1                     2
    QueryResult result = WorldDatabase.Query("SELECT creature_id, RewOnKillRepFaction1, RewOnKillRepFaction2, "
    //   3             4             5                   6             7             8                   9
        "IsTeamAward1, MaxStanding1, RewOnKillRepValue1, IsTeamAward2, MaxStanding2, RewOnKillRepValue2, TeamDependent "
        "FROM creature_onkill_reputation");

    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 creature award reputation definitions. DB table `creature_onkill_reputation` is empty.");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 creature_id = fields[0].GetUInt32();

        ReputationOnKillEntry repOnKill;
        repOnKill.RepFaction1          = fields[1].GetInt16();
        repOnKill.RepFaction2          = fields[2].GetInt16();
        repOnKill.IsTeamAward1        = fields[3].GetBool();
        repOnKill.ReputationMaxCap1  = fields[4].GetUInt8();
        repOnKill.RepValue1            = fields[5].GetInt32();
        repOnKill.IsTeamAward2        = fields[6].GetBool();
        repOnKill.ReputationMaxCap2  = fields[7].GetUInt8();
        repOnKill.RepValue2            = fields[8].GetInt32();
        repOnKill.TeamDependent       = fields[9].GetUInt8();

        if (!GetCreatureTemplate(creature_id))
        {
            TC_LOG_ERROR("sql.sql", "Table `creature_onkill_reputation` have data for not existed creature entry (%u), skipped", creature_id);
            continue;
        }

        if (repOnKill.RepFaction1)
        {
            FactionEntry const* factionEntry1 = sFactionStore.LookupEntry(repOnKill.RepFaction1);
            if (!factionEntry1)
            {
                TC_LOG_ERROR("sql.sql", "Faction (faction.dbc) %u does not exist but is used in `creature_onkill_reputation`", repOnKill.RepFaction1);
                continue;
            }
        }

        if (repOnKill.RepFaction2)
        {
            FactionEntry const* factionEntry2 = sFactionStore.LookupEntry(repOnKill.RepFaction2);
            if (!factionEntry2)
            {
                TC_LOG_ERROR("sql.sql", "Faction (faction.dbc) %u does not exist but is used in `creature_onkill_reputation`", repOnKill.RepFaction2);
                continue;
            }
        }

        _repOnKillStore[creature_id] = repOnKill;

        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u creature award reputation definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadReputationSpilloverTemplate()
{
    uint32 oldMSTime = getMSTime();

    _repSpilloverTemplateStore.clear();                      // for reload case

    uint32 count = 0; //                                0         1        2       3        4       5       6         7        8      9        10       11     12        13       14     15
    QueryResult result = WorldDatabase.Query("SELECT faction, faction1, rate_1, rank_1, faction2, rate_2, rank_2, faction3, rate_3, rank_3, faction4, rate_4, rank_4, faction5, rate_5, rank_5 FROM reputation_spillover_template");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded `reputation_spillover_template`, table is empty.");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 factionId                = fields[0].GetUInt16();

        RepSpilloverTemplate repTemplate;

        repTemplate.faction[0]          = fields[1].GetUInt16();
        repTemplate.faction_rate[0]     = fields[2].GetFloat();
        repTemplate.faction_rank[0]     = fields[3].GetUInt8();
        repTemplate.faction[1]          = fields[4].GetUInt16();
        repTemplate.faction_rate[1]     = fields[5].GetFloat();
        repTemplate.faction_rank[1]     = fields[6].GetUInt8();
        repTemplate.faction[2]          = fields[7].GetUInt16();
        repTemplate.faction_rate[2]     = fields[8].GetFloat();
        repTemplate.faction_rank[2]     = fields[9].GetUInt8();
        repTemplate.faction[3]          = fields[10].GetUInt16();
        repTemplate.faction_rate[3]     = fields[11].GetFloat();
        repTemplate.faction_rank[3]     = fields[12].GetUInt8();
        repTemplate.faction[4]          = fields[13].GetUInt16();
        repTemplate.faction_rate[4]     = fields[14].GetFloat();
        repTemplate.faction_rank[4]     = fields[15].GetUInt8();

        FactionEntry const* factionEntry = sFactionStore.LookupEntry(factionId);

        if (!factionEntry)
        {
            TC_LOG_ERROR("sql.sql", "Faction (faction.dbc) %u does not exist but is used in `reputation_spillover_template`", factionId);
            continue;
        }

        if (factionEntry->team == 0)
        {
            TC_LOG_ERROR("sql.sql", "Faction (faction.dbc) %u in `reputation_spillover_template` does not belong to any team, skipping", factionId);
            continue;
        }

        for (uint32 i = 0; i < MAX_SPILLOVER_FACTIONS; ++i)
        {
            if (repTemplate.faction[i])
            {
                FactionEntry const* factionSpillover = sFactionStore.LookupEntry(repTemplate.faction[i]);

                if (!factionSpillover)
                {
                    TC_LOG_ERROR("sql.sql", "Spillover faction (faction.dbc) %u does not exist but is used in `reputation_spillover_template` for faction %u, skipping", repTemplate.faction[i], factionId);
                    continue;
                }

                if (factionSpillover->reputationListID < 0)
                {
                    TC_LOG_ERROR("sql.sql", "Spillover faction (faction.dbc) %u for faction %u in `reputation_spillover_template` can not be listed for client, and then useless, skipping", repTemplate.faction[i], factionId);
                    continue;
                }

                if (repTemplate.faction_rank[i] >= MAX_REPUTATION_RANK)
                {
                    TC_LOG_ERROR("sql.sql", "Rank %u used in `reputation_spillover_template` for spillover faction %u is not valid, skipping", repTemplate.faction_rank[i], repTemplate.faction[i]);
                    continue;
                }
            }
        }

        FactionEntry const* factionEntry0 = sFactionStore.LookupEntry(repTemplate.faction[0]);
        if (repTemplate.faction[0] && !factionEntry0)
        {
            TC_LOG_ERROR("sql.sql", "Faction (faction.dbc) %u does not exist but is used in `reputation_spillover_template`", repTemplate.faction[0]);
            continue;
        }
        FactionEntry const* factionEntry1 = sFactionStore.LookupEntry(repTemplate.faction[1]);
        if (repTemplate.faction[1] && !factionEntry1)
        {
            TC_LOG_ERROR("sql.sql", "Faction (faction.dbc) %u does not exist but is used in `reputation_spillover_template`", repTemplate.faction[1]);
            continue;
        }
        FactionEntry const* factionEntry2 = sFactionStore.LookupEntry(repTemplate.faction[2]);
        if (repTemplate.faction[2] && !factionEntry2)
        {
            TC_LOG_ERROR("sql.sql", "Faction (faction.dbc) %u does not exist but is used in `reputation_spillover_template`", repTemplate.faction[2]);
            continue;
        }
        FactionEntry const* factionEntry3 = sFactionStore.LookupEntry(repTemplate.faction[3]);
        if (repTemplate.faction[3] && !factionEntry3)
        {
            TC_LOG_ERROR("sql.sql", "Faction (faction.dbc) %u does not exist but is used in `reputation_spillover_template`", repTemplate.faction[3]);
            continue;
        }
        FactionEntry const* factionEntry4 = sFactionStore.LookupEntry(repTemplate.faction[4]);
        if (repTemplate.faction[4] && !factionEntry4)
        {
            TC_LOG_ERROR("sql.sql", "Faction (faction.dbc) %u does not exist but is used in `reputation_spillover_template`", repTemplate.faction[4]);
            continue;
        }

        _repSpilloverTemplateStore[factionId] = repTemplate;

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u reputation_spillover_template in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadPointsOfInterest()
{
    uint32 oldMSTime = getMSTime();

    _pointsOfInterestStore.clear();                              // need for reload case

    uint32 count = 0;

    //                                               0   1          2          3     4      5           6
    QueryResult result = WorldDatabase.Query("SELECT ID, PositionX, PositionY, Icon, Flags, Importance, Name FROM points_of_interest");

    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 Points of Interest definitions. DB table `points_of_interest` is empty.");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 point_id = fields[0].GetUInt32();

        PointOfInterest pointOfInterest;
        pointOfInterest.ID         = point_id;
        pointOfInterest.PositionX  = fields[1].GetFloat();
        pointOfInterest.PositionY  = fields[2].GetFloat();
        pointOfInterest.Icon       = fields[3].GetUInt32();
        pointOfInterest.Flags      = fields[4].GetUInt32();
        pointOfInterest.Importance = fields[5].GetUInt32();
        pointOfInterest.Name       = fields[6].GetString();

        if (!Trinity::IsValidMapCoord(pointOfInterest.PositionX, pointOfInterest.PositionY))
        {
            TC_LOG_ERROR("sql.sql", "Table `points_of_interest` (Entry: %u) have invalid coordinates (X: %f Y: %f), ignored.", point_id, pointOfInterest.PositionX, pointOfInterest.PositionY);
            continue;
        }

        _pointsOfInterestStore[point_id] = pointOfInterest;

        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u Points of Interest definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadQuestPOI()
{
    uint32 oldMSTime = getMSTime();

    _questPOIStore.clear();                              // need for reload case

    //                                                   0        1     2               3                 4      5               6      7         8
    QueryResult result = WorldDatabase.Query("SELECT QuestId, Idx1, ObjectiveIndex, QuestObjectiveId, MapID, WorldMapAreaId, Floor, Priority, Flags FROM quest_poi order by QuestId");

    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 quest POI definitions. DB table `quest_poi` is empty.");
        return;
    }

    //                                                   0        1     2  3
    QueryResult points = WorldDatabase.Query("SELECT QuestId, Idx1, X, Y FROM quest_poi_points ORDER BY QuestId DESC, Idx1, Idx2");

    std::unordered_map<uint32, std::map<uint32, std::vector<QuestPOIBlobPoint>>> allPoints;

    if (points)
    {
        do
        {
            Field* fields = points->Fetch();

            uint32 QuestID            = fields[0].GetUInt32();
            uint32 Idx1               = fields[1].GetUInt32();
            int32  X                  = fields[2].GetInt32();
            int32  Y                  = fields[3].GetInt32();

            allPoints[QuestID][Idx1].emplace_back(X, Y);
        } while (points->NextRow());
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 QuestID            = fields[0].GetUInt32();
        uint32 Idx1               = fields[1].GetUInt32();
        int32 ObjectiveIndex      = fields[2].GetInt32();
        uint32 QuestObjectiveId   = fields[3].GetUInt32();
        uint32 MapId              = fields[4].GetUInt32();
        uint32 WorldMapAreaId     = fields[5].GetUInt32();
        uint32 Floor              = fields[6].GetUInt32();
        uint32 Priority           = fields[7].GetUInt32();
        uint32 Flags              = fields[8].GetUInt32();

        if (!GetQuestTemplate(QuestID))
          TC_LOG_ERROR("sql.sql", "`quest_poi` quest id (%u) Idx1 (%u) does not exist in `quest_template`", QuestID, Idx1);

        if (std::map<uint32, std::vector<QuestPOIBlobPoint>>* blobs = Trinity::Containers::MapGetValuePtr(allPoints, QuestID))
        {
            if (std::vector<QuestPOIBlobPoint> *points = Trinity::Containers::MapGetValuePtr(*blobs, Idx1))
            {
                QuestPOIData &poiData = _questPOIStore[QuestID];
                poiData.QuestID = QuestID;
                poiData.Blobs.emplace_back(
                    Idx1, ObjectiveIndex, QuestObjectiveId, MapId,
                    WorldMapAreaId, Floor, Priority, Flags, std::move(*points));
                continue;
            }
        }

        TC_LOG_ERROR("sql.sql", "Table quest_poi references unknown quest points for quest %u POI id %u", QuestID, Idx1);
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u quest POI definitions in %u ms", _questPOIStore.size(), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadNPCSpellClickSpells()
{
    uint32 oldMSTime = getMSTime();

    _spellClickInfoStore.clear();
    //                                                0          1         2            3
    QueryResult result = WorldDatabase.Query("SELECT npc_entry, spell_id, cast_flags, user_type FROM npc_spellclick_spells");

    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 spellclick spells. DB table `npc_spellclick_spells` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 npc_entry = fields[0].GetUInt32();
        CreatureTemplate const* cInfo = GetCreatureTemplate(npc_entry);
        if (!cInfo)
        {
            TC_LOG_ERROR("sql.sql", "Table npc_spellclick_spells references unknown creature_template %u. Skipping entry.", npc_entry);
            continue;
        }

        uint32 spellid = fields[1].GetUInt32();
        SpellInfo const* spellinfo = sSpellMgr->GetSpellInfo(spellid);
        if (!spellinfo)
        {
            TC_LOG_ERROR("sql.sql", "Table npc_spellclick_spells references unknown spellid %u. Skipping entry.", spellid);
            continue;
        }

        uint8 userType = fields[3].GetUInt16();
        if (userType >= SPELL_CLICK_USER_MAX)
            TC_LOG_ERROR("sql.sql", "Table npc_spellclick_spells references unknown user type %u. Skipping entry.", uint32(userType));

        uint8 castFlags = fields[2].GetUInt8();
        SpellClickInfo info;
        info.spellId = spellid;
        info.castFlags = castFlags;
        info.userType = SpellClickUserTypes(userType);
        _spellClickInfoStore.insert(SpellClickInfoContainer::value_type(npc_entry, info));

        ++count;
    }
    while (result->NextRow());

    // all spellclick data loaded, now we check if there are creatures with NPC_FLAG_SPELLCLICK but with no data
    // NOTE: It *CAN* be the other way around: no spellclick flag but with spellclick data, in case of creature-only vehicle accessories
    for (auto& creatureTemplatePair : _creatureTemplateStore)
    {
        if ((creatureTemplatePair.second.npcflag & UNIT_NPC_FLAG_SPELLCLICK) && !_spellClickInfoStore.count(creatureTemplatePair.first))
        {
            TC_LOG_ERROR("sql.sql", "npc_spellclick_spells: Creature template %u has UNIT_NPC_FLAG_SPELLCLICK but no data in spellclick table! Removing flag", creatureTemplatePair.first);
            creatureTemplatePair.second.npcflag &= ~UNIT_NPC_FLAG_SPELLCLICK;
        }
    }

    TC_LOG_INFO("server.loading", ">> Loaded %u spellclick definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::DeleteCreatureData(uint32 guid)
{
    // remove mapid*cellid -> guid_set map
    CreatureData const* data = GetCreatureData(guid);
    if (data)
        RemoveCreatureFromGrid(guid, data);

    _creatureDataStore.erase(guid);
}

void ObjectMgr::DeleteGOData(uint32 guid)
{
    // remove mapid*cellid -> guid_set map
    GameObjectData const* data = GetGOData(guid);
    if (data)
        RemoveGameobjectFromGrid(guid, data);

    _gameObjectDataStore.erase(guid);
}

void ObjectMgr::LoadQuestRelationsHelper(QuestRelations& map, QuestRelationsReverse* reversedMap, std::string const& table)
{
    uint32 oldMSTime = getMSTime();

    map.clear();                                            // need for reload case

    uint32 count = 0;

    QueryResult result = WorldDatabase.PQuery("SELECT id, quest FROM %s", table.c_str());

    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 quest relations from `%s`, table is empty.", table.c_str());
        return;
    }

    do
    {
        uint32 id     = result->Fetch()[0].GetUInt32();
        uint32 quest  = result->Fetch()[1].GetUInt32();

        if (_questTemplates.find(quest) == _questTemplates.end())
        {
            TC_LOG_ERROR("sql.sql", "Table `%s`: Quest %u listed for entry %u does not exist.", table.c_str(), quest, id);
            continue;
        }


        map.insert(QuestRelations::value_type(id, quest));
        if (reversedMap)
            reversedMap->insert(QuestRelationsReverse::value_type(quest, id));
        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u quest relations from %s in %u ms", count, table.c_str(), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadGameobjectQuestStarters()
{
    LoadQuestRelationsHelper(_goQuestRelations, nullptr, "gameobject_queststarter");

    for (QuestRelations::iterator itr = _goQuestRelations.begin(); itr != _goQuestRelations.end(); ++itr)
    {
        GameObjectTemplate const* goInfo = GetGameObjectTemplate(itr->first);
        if (!goInfo)
            TC_LOG_ERROR("sql.sql", "Table `gameobject_queststarter` have data for not existed gameobject entry (%u) and existed quest %u", itr->first, itr->second);
        else if (goInfo->type != GAMEOBJECT_TYPE_QUESTGIVER)
            TC_LOG_ERROR("sql.sql", "Table `gameobject_queststarter` have data gameobject entry (%u) for quest %u, but GO is not GAMEOBJECT_TYPE_QUESTGIVER", itr->first, itr->second);
    }
}

void ObjectMgr::LoadGameobjectQuestEnders()
{
    LoadQuestRelationsHelper(_goQuestInvolvedRelations, &_goQuestInvolvedRelationsReverse, "gameobject_questender");

    for (QuestRelations::iterator itr = _goQuestInvolvedRelations.begin(); itr != _goQuestInvolvedRelations.end(); ++itr)
    {
        GameObjectTemplate const* goInfo = GetGameObjectTemplate(itr->first);
        if (!goInfo)
            TC_LOG_ERROR("sql.sql", "Table `gameobject_questender` have data for not existed gameobject entry (%u) and existed quest %u", itr->first, itr->second);
        else if (goInfo->type != GAMEOBJECT_TYPE_QUESTGIVER)
            TC_LOG_ERROR("sql.sql", "Table `gameobject_questender` have data gameobject entry (%u) for quest %u, but GO is not GAMEOBJECT_TYPE_QUESTGIVER", itr->first, itr->second);
    }
}

void ObjectMgr::LoadCreatureQuestStarters()
{
    LoadQuestRelationsHelper(_creatureQuestRelations, nullptr, "creature_queststarter");

    for (QuestRelations::iterator itr = _creatureQuestRelations.begin(); itr != _creatureQuestRelations.end(); ++itr)
    {
        CreatureTemplate const* cInfo = GetCreatureTemplate(itr->first);
        if (!cInfo)
            TC_LOG_ERROR("sql.sql", "Table `creature_queststarter` have data for not existed creature entry (%u) and existed quest %u", itr->first, itr->second);
        else if (!(cInfo->npcflag & UNIT_NPC_FLAG_QUESTGIVER))
            TC_LOG_ERROR("sql.sql", "Table `creature_queststarter` has creature entry (%u) for quest %u, but npcflag does not include UNIT_NPC_FLAG_QUESTGIVER", itr->first, itr->second);
    }
}

void ObjectMgr::LoadCreatureQuestEnders()
{
    LoadQuestRelationsHelper(_creatureQuestInvolvedRelations, &_creatureQuestInvolvedRelationsReverse, "creature_questender");

    for (QuestRelations::iterator itr = _creatureQuestInvolvedRelations.begin(); itr != _creatureQuestInvolvedRelations.end(); ++itr)
    {
        CreatureTemplate const* cInfo = GetCreatureTemplate(itr->first);
        if (!cInfo)
            TC_LOG_ERROR("sql.sql", "Table `creature_questender` have data for not existed creature entry (%u) and existed quest %u", itr->first, itr->second);
        else if (!(cInfo->npcflag & UNIT_NPC_FLAG_QUESTGIVER))
            TC_LOG_ERROR("sql.sql", "Table `creature_questender` has creature entry (%u) for quest %u, but npcflag does not include UNIT_NPC_FLAG_QUESTGIVER", itr->first, itr->second);
    }
}

void QuestRelationResult::Iterator::_skip()
{
    while ((_it != _end) && !Quest::IsTakingQuestEnabled(_it->second))
        ++_it;
}

bool QuestRelationResult::HasQuest(uint32 questId) const
{
    return (std::find_if(_begin, _end, [questId](QuestRelations::value_type const& pair) { return (pair.second == questId); }) != _end) && (!_onlyActive || Quest::IsTakingQuestEnabled(questId));
}

void ObjectMgr::LoadReservedPlayersNames()
{
    uint32 oldMSTime = getMSTime();

    _reservedNamesStore.clear();                                // need for reload case

    QueryResult result = CharacterDatabase.Query("SELECT name FROM reserved_name");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 reserved player names. DB table `reserved_name` is empty!");
        return;
    }

    uint32 count = 0;

    Field* fields;
    do
    {
        fields = result->Fetch();
        std::string name= fields[0].GetString();

        std::wstring wstr;
        if (!Utf8toWStr (name, wstr))
        {
            TC_LOG_ERROR("misc", "Table `reserved_name` have invalid name: %s", name.c_str());
            continue;
        }

        wstrToLower(wstr);

        _reservedNamesStore.insert(wstr);
        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u reserved player names in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

bool ObjectMgr::IsReservedName(const std::string& name) const
{
    std::wstring wstr;
    if (!Utf8toWStr (name, wstr))
        return false;

    wstrToLower(wstr);

    return _reservedNamesStore.find(wstr) != _reservedNamesStore.end();
}

enum LanguageType
{
    LT_BASIC_LATIN    = 0x0000,
    LT_EXTENDEN_LATIN = 0x0001,
    LT_CYRILLIC       = 0x0002,
    LT_EAST_ASIA      = 0x0004,
    LT_ANY            = 0xFFFF
};

static LanguageType GetRealmLanguageType(bool create)
{
    switch (sWorld->getIntConfig(CONFIG_REALM_ZONE))
    {
        case REALM_ZONE_UNKNOWN:                            // any language
        case REALM_ZONE_DEVELOPMENT:
        case REALM_ZONE_TEST_SERVER:
        case REALM_ZONE_QA_SERVER:
            return LT_ANY;
        case REALM_ZONE_UNITED_STATES:                      // extended-Latin
        case REALM_ZONE_OCEANIC:
        case REALM_ZONE_LATIN_AMERICA:
        case REALM_ZONE_ENGLISH:
        case REALM_ZONE_GERMAN:
        case REALM_ZONE_FRENCH:
        case REALM_ZONE_SPANISH:
            return LT_EXTENDEN_LATIN;
        case REALM_ZONE_KOREA:                              // East-Asian
        case REALM_ZONE_TAIWAN:
        case REALM_ZONE_CHINA:
            return LT_EAST_ASIA;
        case REALM_ZONE_RUSSIAN:                            // Cyrillic
            return LT_CYRILLIC;
        default:
            return create ? LT_BASIC_LATIN : LT_ANY;        // basic-Latin at create, any at login
    }
}

bool isValidString(const std::wstring& wstr, uint32 strictMask, bool numericOrSpace, bool create = false)
{
    if (strictMask == 0)                                       // any language, ignore realm
    {
        if (isExtendedLatinString(wstr, numericOrSpace))
            return true;
        if (isCyrillicString(wstr, numericOrSpace))
            return true;
        if (isEastAsianString(wstr, numericOrSpace))
            return true;
        return false;
    }

    if (strictMask & 0x2)                                    // realm zone specific
    {
        LanguageType lt = GetRealmLanguageType(create);
        if (lt & LT_EXTENDEN_LATIN)
            if (isExtendedLatinString(wstr, numericOrSpace))
                return true;
        if (lt & LT_CYRILLIC)
            if (isCyrillicString(wstr, numericOrSpace))
                return true;
        if (lt & LT_EAST_ASIA)
            if (isEastAsianString(wstr, numericOrSpace))
                return true;
    }

    if (strictMask & 0x1)                                    // basic Latin
    {
        if (isBasicLatinString(wstr, numericOrSpace))
            return true;
    }

    return false;
}

uint8 ObjectMgr::CheckPlayerName(const std::string& name, bool create)
{
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return CHAR_NAME_INVALID_CHARACTER;

    if (wname.size() > MAX_PLAYER_NAME)
        return CHAR_NAME_TOO_LONG;

    uint32 minName = sWorld->getIntConfig(CONFIG_MIN_PLAYER_NAME);
    if (wname.size() < minName)
        return CHAR_NAME_TOO_SHORT;

    uint32 strictMask = sWorld->getIntConfig(CONFIG_STRICT_PLAYER_NAMES);
    if (!isValidString(wname, strictMask, false, create))
        return CHAR_NAME_MIXED_LANGUAGES;

    wstrToLower(wname);
    for (size_t i = 2; i < wname.size(); ++i)
        if (wname[i] == wname[i-1] && wname[i] == wname[i-2])
            return CHAR_NAME_THREE_CONSECUTIVE;

    return CHAR_NAME_SUCCESS;
}

bool ObjectMgr::IsValidCharterName(const std::string& name)
{
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return false;

    if (wname.size() > MAX_CHARTER_NAME)
        return false;

    uint32 minName = sWorld->getIntConfig(CONFIG_MIN_CHARTER_NAME);
    if (wname.size() < minName)
        return false;

    uint32 strictMask = sWorld->getIntConfig(CONFIG_STRICT_CHARTER_NAMES);

    return isValidString(wname, strictMask, true);
}

PetNameInvalidReason ObjectMgr::CheckPetName(const std::string& name)
{
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return PET_NAME_INVALID;

    if (wname.size() > MAX_PET_NAME)
        return PET_NAME_TOO_LONG;

    uint32 minName = sWorld->getIntConfig(CONFIG_MIN_PET_NAME);
    if (wname.size() < minName)
        return PET_NAME_TOO_SHORT;

    uint32 strictMask = sWorld->getIntConfig(CONFIG_STRICT_PET_NAMES);
    if (!isValidString(wname, strictMask, false))
        return PET_NAME_MIXED_LANGUAGES;

    return PET_NAME_SUCCESS;
}

void ObjectMgr::LoadGameObjectForQuests()
{
    uint32 oldMSTime = getMSTime();

    _gameObjectForQuestStore.clear();                         // need for reload case

    if (sObjectMgr->GetGameObjectTemplates()->empty())
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 GameObjects for quests");
        return;
    }

    uint32 count = 0;

    // collect GO entries for GO that must activated
    GameObjectTemplateContainer const* gotc = sObjectMgr->GetGameObjectTemplates();
    for (GameObjectTemplateContainer::const_iterator itr = gotc->begin(); itr != gotc->end(); ++itr)
    {
        switch (itr->second.type)
        {
            // scan GO chest with loot including quest items
            case GAMEOBJECT_TYPE_CHEST:
            {
                uint32 loot_id = (itr->second.GetLootId());

                // find quest loot for GO
                if (itr->second.chest.questId || LootTemplates_Gameobject.HaveQuestLootFor(loot_id))
                {
                    _gameObjectForQuestStore.insert(itr->second.entry);
                    ++count;
                }
                break;
            }
            case GAMEOBJECT_TYPE_GENERIC:
            {
                if (itr->second._generic.questID > 0)            //quests objects
                {
                    _gameObjectForQuestStore.insert(itr->second.entry);
                    count++;
                }
                break;
            }
            case GAMEOBJECT_TYPE_GOOBER:
            {
                if (itr->second.goober.questId > 0)              //quests objects
                {
                    _gameObjectForQuestStore.insert(itr->second.entry);
                    count++;
                }
                break;
            }
            default:
                break;
        }
    }

    TC_LOG_INFO("server.loading", ">> Loaded %u GameObjects for quests in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

bool ObjectMgr::LoadTrinityStrings(const char* table, int32 min_value, int32 max_value)
{
    uint32 oldMSTime = getMSTime();

    int32 start_value = min_value;
    int32 end_value   = max_value;
    // some string can have negative indexes range
    if (start_value < 0)
    {
        if (end_value >= start_value)
        {
            TC_LOG_ERROR("sql.sql", "Table '%s' attempt loaded with invalid range (%d - %d), strings not loaded.", table, min_value, max_value);
            return false;
        }

        // real range (max+1, min+1) exaple: (-10, -1000) -> -999...-10+1
        std::swap(start_value, end_value);
        ++start_value;
        ++end_value;
    }
    else
    {
        if (start_value >= end_value)
        {
            TC_LOG_ERROR("sql.sql", "Table '%s' attempt loaded with invalid range (%d - %d), strings not loaded.", table, min_value, max_value);
            return false;
        }
    }

    // cleanup affected map part for reloading case
    for (TrinityStringLocaleContainer::iterator itr = _trinityStringLocaleStore.begin(); itr != _trinityStringLocaleStore.end();)
    {
        if (itr->first >= start_value && itr->first < end_value)
            _trinityStringLocaleStore.erase(itr++);
        else
            ++itr;
    }

    QueryResult result = WorldDatabase.PQuery("SELECT entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, content_loc9, content_loc10, content_loc11 FROM %s", table);

    if (!result)
    {
        if (min_value == MIN_TRINITY_STRING_ID)              // error only in case internal strings
            TC_LOG_ERROR("server.loading", ">> Loaded 0 trinity strings. DB table `%s` is empty. Cannot continue.", table);
        else
            TC_LOG_INFO("server.loading", ">> Loaded 0 string templates. DB table `%s` is empty.", table);

        return false;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        int32 entry = fields[0].GetInt32();

        if (entry == 0)
        {
            TC_LOG_ERROR("sql.sql", "Table `%s` contain reserved entry 0, ignored.", table);
            continue;
        }
        else if (entry < start_value || entry >= end_value)
        {
            TC_LOG_ERROR("sql.sql", "Table `%s` contain entry %i out of allowed range (%d - %d), ignored.", table, entry, min_value, max_value);
            continue;
        }

        TrinityStringLocale& data = _trinityStringLocaleStore[entry];

        if (!data.Content.empty())
        {
            TC_LOG_ERROR("sql.sql", "Table `%s` contain data for already loaded entry  %i (from another table?), ignored.", table, entry);
            continue;
        }

        data.Content.resize(1);
        ++count;

        for (uint8 i = 0; i < TOTAL_LOCALES; ++i)
            AddLocaleString(fields[i + 1].GetString(), LocaleConstant(i), data.Content);
    } while (result->NextRow());

    if (min_value == MIN_TRINITY_STRING_ID)
        TC_LOG_INFO("server.loading", ">> Loaded %u Trinity strings from table %s in %u ms", count, table, GetMSTimeDiffToNow(oldMSTime));
    else
        TC_LOG_INFO("server.loading", ">> Loaded %u string templates from %s in %u ms", count, table, GetMSTimeDiffToNow(oldMSTime));

    return true;
}

const char *ObjectMgr::GetTrinityString(int32 entry, LocaleConstant locale_idx) const
{
    if (TrinityStringLocale const* msl = GetTrinityStringLocale(entry))
    {
        if (msl->Content.size() > size_t(locale_idx) && !msl->Content[locale_idx].empty())
            return msl->Content[locale_idx].c_str();

        return msl->Content[DEFAULT_LOCALE].c_str();
    }

    if (entry > 0)
        TC_LOG_ERROR("sql.sql", "Entry %i not found in `trinity_string` table.", entry);
    else
        TC_LOG_ERROR("sql.sql", "Trinity string entry %i not found in DB.", entry);
    return "<error>";
}

void ObjectMgr::LoadFishingBaseSkillLevel()
{
    uint32 oldMSTime = getMSTime();

    _fishingBaseForAreaStore.clear();                            // for reload case

    QueryResult result = WorldDatabase.Query("SELECT entry, skill FROM skill_fishing_base_level");

    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 areas for fishing base skill level. DB table `skill_fishing_base_level` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        uint32 entry  = fields[0].GetUInt32();
        int32 skill   = fields[1].GetInt16();

        AreaTableEntry const* fArea = sAreaTableStore.LookupEntry(entry);
        if (!fArea)
        {
            TC_LOG_ERROR("sql.sql", "AreaId %u defined in `skill_fishing_base_level` does not exist", entry);
            continue;
        }

        _fishingBaseForAreaStore[entry] = skill;
        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u areas for fishing base skill level in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

// Table at .data:00ADCE88 in binary build 12340
struct DeclineForm
{
    std::wstring EndingVariants[9]; // Client: wchar_t[9][5] + 1 more wchar_t due to aligning
    ::Gender Gender;
    std::wstring RootEnding; // Client: wchar_t[4]
    uint32 Unknown; // Always zero
    uint32 DeclensionSetMale;
    uint32 DeclensionSetFemale;
    uint32 DeclensionSetNone;
} DeclineForms[55] =
{
    { L"и",    L"у",   L"ы",   L"э",   L"ю",   L"аа",  L"",    L"",    L"",    GENDER_NONE,   L"",     0,  0, 71, 71 },
    { L"ай",   L"ей",  L"уй",  L"эй",  L"юй",  L"яй",  L"ёй",  L"",    L"",    GENDER_MALE,   L"й",    0,  1,  0, 71 },
    { L"ай",   L"ей",  L"уй",  L"эй",  L"юй",  L"яй",  L"",    L"",    L"",    GENDER_FEMALE, L"й",    0,  0,  1, 71 },
    { L"ок",   L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"ок",   0,  2,  3,  0 },
    { L"чек",  L"шек", L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_MALE,   L"ек",   0,  4,  5,  0 },
    { L"чек",  L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_FEMALE, L"ек",   0,  4,  6,  0 },
    { L"лек",  L"мек", L"нек", L"рек", L"сек", L"тек", L"",    L"",    L"",    GENDER_NONE,   L"ек",   0,  8,  7,  0 },
    { L"аек",  L"еек", L"иек", L"уек", L"ёек", L"",    L"",    L"",    L"",    GENDER_NONE,   L"ек",   0,  9, 10,  0 },
    { L"аец",  L"еец", L"иец", L"оец", L"уец", L"ыец", L"эец", L"юец", L"яец", GENDER_NONE,   L"ец",   0, 11,  0, 71 },
    { L"ец",   L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"ец",   0, 12, 13,  0 },
    { L"ка",   L"га",  L"ха",  L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"а",    0, 14,  0, 71 },
    { L"ия",   L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"я",    0, 15,  0, 71 },
    { L"ея",   L"ёя",  L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"я",    0, 16,  0, 71 },
    { L"емя",  L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"я",    0, 17,  0, 71 },
    { L"ой",   L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"ой",   0, 18, 19,  0 },
    { L"ый",   L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"ый",   0, 20, 21,  0 },
    { L"ий",   L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"ий",   0, 22, 23, 52 },
    { L"чая",  L"щая", L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"ая",   0, 24,  0, 71 },
    { L"ая",   L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"ая",   0, 25, 26,  0 },
    { L"яя",   L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"яя",   0, 27, 28,  0 },
    { L"ое",   L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_MALE,   L"ое",   0, 29, 30,  0 },
    { L"ое",   L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_FEMALE, L"ое",   0, 29, 31,  0 },
    { L"ее",   L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_MALE,   L"ее",   0, 32, 33,  0 },
    { L"ее",   L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_FEMALE, L"ее",   0, 32, 34,  0 },
    { L"о",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"о",    0,  0, 35, 71 },
    { L"ие",   L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"е",    0, 37,  0,  0 },
    { L"е",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"е",    0,  0, 38, 71 },
    { L"я",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"я",    0, 39,  0, 71 },
    { L"ь",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_MALE,   L"ь",    0, 40, 41,  0 },
    { L"ь",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_FEMALE, L"ь",    0, 41, 40,  0 },
    { L"ча",   L"ша",  L"ща",  L"жа",  L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"а",    0, 42,  0, 71 },
    { L"ца",   L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"а",    0, 43,  0, 71 },
    { L"а",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"а",    0, 44,  0, 71 },
    { L"ч",    L"щ",   L"ж",   L"ш",   L"ж",   L"",    L"",    L"",    L"",    GENDER_MALE,   L"",     0, 45, 46,  0 },
    { L"ч",    L"щ",   L"ж",   L"ш",   L"ж",   L"",    L"",    L"",    L"",    GENDER_FEMALE, L"",     0,  0, 45, 46 },
    { L"",     L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_MALE,   L"",     0, 47,  0, 71 }, // 35
    { L"",     L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_FEMALE, L"",     0,  0, 47, 71 }, // 36
    { L"ийся", L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"ийся", 0, 48,  0, 71 },
    { L"ееся", L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"ееся", 0, 49,  0, 71 },
    { L"аяся", L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"аяся", 0, 50,  0, 71 },
    { L"иеся", L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"иеся", 0, 51,  0, 71 },
    { L"ё",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"ё",    0,  0, 53, 71 },
    { L"ень",  L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_MALE,   L"ень",  0, 54, 55,  0 },
    { L"ень",  L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_FEMALE, L"ень",  0,  0, 54, 56 },
    { L"ёнь",  L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_MALE,   L"ёнь",  0, 57, 58,  0 },
    { L"ёнь",  L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_FEMALE, L"ёнь",  0,  0, 57, 59 },
    { L"оть",  L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_MALE,   L"оть",  0, 60, 61,  0 },
    { L"оть",  L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_FEMALE, L"оть",  0, 61, 60,  0 },
    { L"ще",   L"ше",  L"че",  L"це",  L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"е",    0, 62,  0, 71 },
    { L"лёк",  L"мёк", L"нёк", L"рёк", L"сёк", L"тёк", L"",    L"",    L"",    GENDER_NONE,   L"ёк",   0, 63, 64,  0 },
    { L"аёк",  L"еёк", L"иёк", L"уёк", L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"ёк",   0,  9, 10,  0 },
    { L"аёц",  L"еёц", L"иёц", L"оёц", L"уёц", L"ыёц", L"эёц", L"юёц", L"яёц", GENDER_NONE,   L"ёц",   0, 11,  0, 71 },
    { L"лец",  L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"ец",   0, 65, 66,  0 },
    { L"лёц",  L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"ёц",   0, 67, 68,  0 },
    { L"ёц",   L"",    L"",    L"",    L"",    L"",    L"",    L"",    L"",    GENDER_NONE,   L"ёц",   0, 69, 70,  0 },
};

struct DetectDeclinedNameFormResult
{
    uint32 MatchedDeclineFormIndex = 35;
    uint32 RootEndIndex = 0;
    std::wstring Name; // Client: wchar_t[1024]
    bool SoftRootFound = false;
    uint32 SoftRootEndIndex = 0;
};

#pragma warning(push)
#pragma warning(disable: 4066)

// Client function at .text:0076DE70 in binary build 12340
bool DetectDeclinedNameForm(std::string name, DetectDeclinedNameFormResult& result, Gender gender)
{
    if (!Utf8toWStr(name, result.Name))
        return false;

    wstrToLower(result.Name);

    int32 length = result.Name.length();

    result.RootEndIndex = length;
    result.MatchedDeclineFormIndex = gender == GENDER_FEMALE ? 36 : 35;
    result.SoftRootFound = false;

    // Search for a best matching ending form from DeclineForms table
    uint32 mostMatchedChars = 0;
    uint32 const formsTotal = sizeof(DeclineForms) / sizeof(DeclineForms[0]);
    for (uint32 formIndex = 0; formIndex < formsTotal; ++formIndex)
    {
        DeclineForm& form = DeclineForms[formIndex];

        if (form.Gender == GENDER_NONE || gender == GENDER_NONE || gender == form.Gender)
        {
            uint32 matchedChars;
            for (auto&& variant : form.EndingVariants)
            {
                matchedChars = 0;

                int32 variantIndex = variant.length() - 1;
                wchar_t* variantPtr = &variant[std::max(0, variantIndex)]; // Crash in Debug build due to index being -1
                int32 nameIndex = length - 1;
                wchar_t* namePtr = &result.Name[nameIndex];

                while (variantIndex >= 0)
                {
                    if (nameIndex < 0)
                        break;

                    if (*variantPtr)
                    {
                        wchar_t nameChar = *namePtr;
                        --nameIndex;
                        --namePtr;

                        if (*variantPtr != nameChar)
                            break;

                        ++matchedChars;
                    }
                    --variantIndex;
                    --variantPtr;
                }

                if (matchedChars > 0 && variantIndex < 0)
                    break;
            }

            if (mostMatchedChars < matchedChars)
            {
                mostMatchedChars = matchedChars;
                result.MatchedDeclineFormIndex = formIndex;

                int32 endingIndex = form.RootEnding.length() - 1;
                wchar_t* endingPtr = &form.RootEnding[std::max(0, endingIndex)]; // Crash in Debug build due to index being -1
                int32 nameIndex = length - 1;
                wchar_t* namePtr = &result.Name[nameIndex];

                while (endingIndex >= 0)
                {
                    if (nameIndex < 0)
                        break;

                    if (*endingPtr)
                    {
                        wchar_t nameChar = *namePtr;
                        --nameIndex;
                        --namePtr;

                        if (*endingPtr != nameChar)
                            break;
                    }
                    --endingIndex;
                    --endingPtr;
                }

                result.RootEndIndex = nameIndex + 1;
            }
        }
    }

    if (length > 2)
    {
        int32 rootEndIndex = 0;
        if (result.MatchedDeclineFormIndex == 35 || result.MatchedDeclineFormIndex == 36)
        {
            rootEndIndex = length - 2;
        }
        else
        {
            if (length <= 3 || result.Name[length - 1] != L'ь')
                return true;

            for (auto&& variant : DeclineForms[result.MatchedDeclineFormIndex].EndingVariants)
            {
                if (variant[0] == L'ь')
                {
                    rootEndIndex = length - 3;
                    break;
                }
            }
        }

        if (rootEndIndex > 0)
        {
            if (result.Name[rootEndIndex] == L'е' ||
                result.Name[rootEndIndex] == L'о' ||
                result.Name[rootEndIndex] == L'ё')
            {
                result.SoftRootEndIndex = rootEndIndex;
                result.SoftRootFound = true;
            }
        }
    }

    return true;
}

#pragma warning(pop)

// Client function at .text:0076E390 in binary build 12340
bool ObjectMgr::CheckDeclinedName(std::string originalName, std::string declinedName)
{
    DetectDeclinedNameFormResult result;
    if (!DetectDeclinedNameForm(originalName, result, GENDER_NONE))
        return false;

    std::wstring declinedNameWStr;
    if (!Utf8toWStr(declinedName, declinedNameWStr))
        return false;

    if (!isCyrillicString(declinedNameWStr, false))
        return false;

    int32 rootEndIndex = result.SoftRootFound ? result.SoftRootEndIndex : result.RootEndIndex;
    int32 i = 0;
    if (rootEndIndex <= 0)
    {
        while (declinedNameWStr[i])
            ++i;

        return i - rootEndIndex <= 5;
    }
    else
    {
        while (result.Name[i] == wcharToLower(declinedNameWStr[i]))
        {
            ++i;
            if (i >= rootEndIndex)
            {
                while (declinedNameWStr[i])
                    ++i;

                return i - rootEndIndex <= 5;
            }
        }
        return false;
    }
}

bool ObjectMgr::CheckDeclinedNames(std::string originalName, DeclinedName const& declinedNames)
{
    for (auto&& name : declinedNames.name)
        if (!CheckDeclinedName(originalName, name))
            return false;

    return true;
}

uint32 ObjectMgr::GetAreaTriggerScriptId(uint32 trigger_id)
{
    AreaTriggerScriptContainer::const_iterator i = _areaTriggerScriptStore.find(trigger_id);
    if (i!= _areaTriggerScriptStore.end())
        return i->second;
    return 0;
}

SpellScriptsBounds ObjectMgr::GetSpellScriptsBounds(uint32 spellId)
{
    return SpellScriptsBounds(_spellScriptsStore.equal_range(spellId));
}

// this allows calculating base reputations to offline players, just by race and class
int32 ObjectMgr::GetBaseReputationOf(FactionEntry const* factionEntry, uint8 race, uint8 playerClass)
{
    if (!factionEntry)
        return 0;

    uint32 raceMask = (1 << (race - 1));
    uint32 classMask = (1 << (playerClass-1));

    for (int i = 0; i < 4; i++)
    {
        if ((!factionEntry->BaseRepClassMask[i] ||
            factionEntry->BaseRepClassMask[i] & classMask) &&
            (!factionEntry->BaseRepRaceMask[i] ||
            factionEntry->BaseRepRaceMask[i] & raceMask))
            return factionEntry->BaseRepValue[i];
    }

    return 0;
}

QuestPOIData const* ObjectMgr::GetQuestPOIData(uint32 questId)
{
  return Trinity::Containers::MapGetValuePtr(_questPOIStore, questId);
}

SkillRangeType GetSkillRangeType(SkillLineEntry const* pSkill, bool racial)
{
    switch (pSkill->categoryId)
    {
        case SKILL_CATEGORY_LANGUAGES:
            return SKILL_RANGE_LANGUAGE;
        case SKILL_CATEGORY_WEAPON:
            return SKILL_RANGE_LEVEL;
        case SKILL_CATEGORY_ARMOR:
        case SKILL_CATEGORY_CLASS:
            if (pSkill->id != SKILL_LOCKPICKING)
                return SKILL_RANGE_MONO;
            else
                return SKILL_RANGE_LEVEL;
        case SKILL_CATEGORY_SECONDARY:
        case SKILL_CATEGORY_PROFESSION:
            // not set skills for professions and racial abilities
            if (IsProfessionSkill(pSkill->id))
                return SKILL_RANGE_RANK;
            else if (racial)
                return SKILL_RANGE_NONE;
            else
                return SKILL_RANGE_MONO;
        case SKILL_CATEGORY_GENERIC:                        // Generic (DND)
        case SKILL_CATEGORY_ATTRIBUTES:                     // At least pandaren racial.
            return SKILL_RANGE_MONO;
        default:
            return SKILL_RANGE_NONE;
    }
}

void ObjectMgr::LoadGameTele()
{
    uint32 oldMSTime = getMSTime();

    _gameTeleStore.clear();                                  // for reload case

    //                                                0       1           2           3           4        5     6
    QueryResult result = WorldDatabase.Query("SELECT id, position_x, position_y, position_z, orientation, map, name FROM game_tele");

    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 GameTeleports. DB table `game_tele` is empty!");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 id         = fields[0].GetUInt32();

        GameTele gt;

        gt.position_x     = fields[1].GetFloat();
        gt.position_y     = fields[2].GetFloat();
        gt.position_z     = fields[3].GetFloat();
        gt.orientation    = fields[4].GetFloat();
        gt.mapId          = fields[5].GetUInt16();
        gt.name           = fields[6].GetString();

        if (!MapManager::IsValidMapCoord(gt.mapId, gt.position_x, gt.position_y, gt.position_z, gt.orientation))
        {
            TC_LOG_ERROR("sql.sql", "Wrong position for id %u (name: %s) in `game_tele` table, ignoring.", id, gt.name.c_str());
            continue;
        }

        if (!Utf8toWStr(gt.name, gt.wnameLow))
        {
            TC_LOG_ERROR("sql.sql", "Wrong UTF8 name for id %u in `game_tele` table, ignoring.", id);
            continue;
        }

        wstrToLower(gt.wnameLow);

        _gameTeleStore[id] = gt;

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u GameTeleports in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

GameTele const* ObjectMgr::GetGameTele(const std::string& name) const
{
    // explicit name case
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return NULL;

    // converting string that we try to find to lower case
    wstrToLower(wname);

    // Alternative first GameTele what contains wnameLow as substring in case no GameTele location found
    const GameTele* alt = NULL;
    for (GameTeleContainer::const_iterator itr = _gameTeleStore.begin(); itr != _gameTeleStore.end(); ++itr)
    {
        if (itr->second.wnameLow == wname)
            return &itr->second;
        else if (alt == NULL && itr->second.wnameLow.find(wname) != std::wstring::npos)
            alt = &itr->second;
    }

    return alt;
}

GameTele const* ObjectMgr::GetGameTeleExactName(const std::string& name) const
{
    // explicit name case
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return NULL;

    // converting string that we try to find to lower case
    wstrToLower(wname);

    for (GameTeleContainer::const_iterator itr = _gameTeleStore.begin(); itr != _gameTeleStore.end(); ++itr)
    {
        if (itr->second.wnameLow == wname)
            return &itr->second;
    }

    return NULL;
}

bool ObjectMgr::AddGameTele(GameTele& tele)
{
    // find max id
    uint32 new_id = 0;
    for (GameTeleContainer::const_iterator itr = _gameTeleStore.begin(); itr != _gameTeleStore.end(); ++itr)
        if (itr->first > new_id)
            new_id = itr->first;

    // use next
    ++new_id;

    if (!Utf8toWStr(tele.name, tele.wnameLow))
        return false;

    wstrToLower(tele.wnameLow);

    _gameTeleStore[new_id] = tele;

    WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_INS_GAME_TELE);

    stmt->setUInt32(0, new_id);
    stmt->setFloat(1, tele.position_x);
    stmt->setFloat(2, tele.position_y);
    stmt->setFloat(3, tele.position_z);
    stmt->setFloat(4, tele.orientation);
    stmt->setUInt16(5, uint16(tele.mapId));
    stmt->setString(6, tele.name);

    WorldDatabase.Execute(stmt);

    return true;
}

bool ObjectMgr::DeleteGameTele(const std::string& name)
{
    // explicit name case
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return false;

    // converting string that we try to find to lower case
    wstrToLower(wname);

    for (GameTeleContainer::iterator itr = _gameTeleStore.begin(); itr != _gameTeleStore.end(); ++itr)
    {
        if (itr->second.wnameLow == wname)
        {
            WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_GAME_TELE);

            stmt->setString(0, itr->second.name);

            WorldDatabase.Execute(stmt);

            _gameTeleStore.erase(itr);
            return true;
        }
    }

    return false;
}

void ObjectMgr::LoadMailLevelRewards()
{
    uint32 oldMSTime = getMSTime();

    _mailLevelRewardStore.clear();                           // for reload case

    //                                                 0        1             2            3
    QueryResult result = WorldDatabase.Query("SELECT level, raceMask, mailTemplateId, senderEntry FROM mail_level_reward");

    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 level dependent mail rewards. DB table `mail_level_reward` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint8 level           = fields[0].GetUInt8();
        uint32 raceMask       = fields[1].GetUInt32();
        uint32 mailTemplateId = fields[2].GetUInt32();
        uint32 senderEntry    = fields[3].GetUInt32();

        if (level > MAX_LEVEL)
        {
            TC_LOG_ERROR("sql.sql", "Table `mail_level_reward` have data for level %u that more supported by client (%u), ignoring.", level, MAX_LEVEL);
            continue;
        }

        if (!(raceMask & RACEMASK_ALL_PLAYABLE))
        {
            TC_LOG_ERROR("sql.sql", "Table `mail_level_reward` have raceMask (%u) for level %u that not include any player races, ignoring.", raceMask, level);
            continue;
        }

        if (!sMailTemplateStore.LookupEntry(mailTemplateId))
        {
            TC_LOG_ERROR("sql.sql", "Table `mail_level_reward` have invalid mailTemplateId (%u) for level %u that invalid not include any player races, ignoring.", mailTemplateId, level);
            continue;
        }

        if (!GetCreatureTemplate(senderEntry))
        {
            TC_LOG_ERROR("sql.sql", "Table `mail_level_reward` have not existed sender creature entry (%u) for level %u that invalid not include any player races, ignoring.", senderEntry, level);
            continue;
        }

        _mailLevelRewardStore[level].push_back(MailLevelReward(raceMask, mailTemplateId, senderEntry));

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u level dependent mail rewards in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::AddSpellToTrainer(uint32 entry, uint32 spell, uint32 spellCost, uint32 reqSkill, uint32 reqSkillValue, uint32 reqLevel)
{
    if (entry >= TRINITY_TRAINER_START_REF)
        return;

    CreatureTemplate const* cInfo = GetCreatureTemplate(entry);
    if (!cInfo)
    {
        TC_LOG_ERROR("sql.sql", "Table `npc_trainer` contains entries for a non-existing creature template (Entry: %u), ignoring", entry);
        return;
    }

    if (!(cInfo->npcflag & UNIT_NPC_FLAG_TRAINER))
    {
        TC_LOG_ERROR("sql.sql", "Table `npc_trainer` contains entries for a creature template (Entry: %u) without trainer flag, ignoring", entry);
        return;
    }

    SpellInfo const* spellinfo = sSpellMgr->GetSpellInfo(spell);
    if (!spellinfo)
    {
        TC_LOG_ERROR("sql.sql", "Table `npc_trainer` contains an entry (Entry: %u) for a non-existing spell (Spell: %u), ignoring", entry, spell);
        return;
    }

    if (!SpellMgr::IsSpellValid(spellinfo))
    {
        TC_LOG_ERROR("sql.sql", "Table `npc_trainer` contains an entry (Entry: %u) for a broken spell (Spell: %u), ignoring", entry, spell);
        return;
    }

    if (GetTalentSpellCost(spell))
    {
        TC_LOG_ERROR("sql.sql", "Table `npc_trainer` contains an entry (Entry: %u) for a non-existing spell (Spell: %u) which is a talent, ignoring", entry, spell);
        return;
    }

    TrainerSpellData& data = _cacheTrainerSpellStore[entry];

    TrainerSpell& trainerSpell = data.spellList[spell];
    trainerSpell.spell         = spell;
    trainerSpell.spellCost     = spellCost;
    trainerSpell.reqSkill      = reqSkill;
    trainerSpell.reqSkillValue = reqSkillValue;
    trainerSpell.reqLevel      = reqLevel;

    if (!trainerSpell.reqLevel)
        trainerSpell.reqLevel = spellinfo->SpellLevel;

    if (IsProfessionSkill(reqSkill))
        data.trainerType = 2;

    // calculate learned spell for profession case when stored cast-spell
    trainerSpell.learnedSpell[0] = spell;
    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        if (spellinfo->Effects[i].Effect != SPELL_EFFECT_LEARN_SPELL)
            continue;
        if (trainerSpell.learnedSpell[0] == spell)
            trainerSpell.learnedSpell[0] = 0;
        // player must be able to cast spell on himself
        if (spellinfo->Effects[i].TargetA.GetTarget() != 0 && spellinfo->Effects[i].TargetA.GetTarget() != TARGET_UNIT_TARGET_ALLY
            && spellinfo->Effects[i].TargetA.GetTarget() != TARGET_UNIT_TARGET_ANY && spellinfo->Effects[i].TargetA.GetTarget() != TARGET_UNIT_CASTER)
        {
            TC_LOG_ERROR("sql.sql", "Table `npc_trainer` has spell %u for trainer entry %u with learn effect which has incorrect target type, ignoring learn effect!", spell, entry);
            continue;
        }

        trainerSpell.learnedSpell[i] = spellinfo->Effects[i].TriggerSpell;

        if (trainerSpell.learnedSpell[i])
        {
            SpellInfo const* learnedSpellInfo = sSpellMgr->GetSpellInfo(trainerSpell.learnedSpell[i]);
            if (learnedSpellInfo && learnedSpellInfo->IsProfession())
                data.trainerType = 2;
        }
    }

    return;
}

void ObjectMgr::LoadTrainerSpell()
{
    uint32 oldMSTime = getMSTime();

    // For reload case
    _cacheTrainerSpellStore.clear();

    QueryResult result = WorldDatabase.Query("SELECT b.entry, a.spell, a.spellcost, a.reqskill, a.reqskillvalue, a.reqlevel FROM npc_trainer AS a "
                                             "INNER JOIN npc_trainer AS b ON a.entry = -(b.spell) "
                                             "UNION SELECT * FROM npc_trainer WHERE spell > 0");

    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">>  Loaded 0 Trainers. DB table `npc_trainer` is empty!");

        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 entry         = fields[0].GetUInt32();
        uint32 spell         = fields[1].GetUInt32();
        uint32 spellCost     = fields[2].GetUInt32();
        uint32 reqSkill      = fields[3].GetUInt16();
        uint32 reqSkillValue = fields[4].GetUInt16();
        uint32 reqLevel      = fields[5].GetUInt8();

        AddSpellToTrainer(entry, spell, spellCost, reqSkill, reqSkillValue, reqLevel);

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %d Trainers in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

int ObjectMgr::LoadReferenceVendor(int32 vendor, int32 item, uint8 type, std::set<uint32> *skip_vendors)
{
    // find all items from the reference vendor
    WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_NPC_VENDOR_REF);
    stmt->setUInt32(0, uint32(item));
    stmt->setUInt8(1, type);
    PreparedQueryResult result = WorldDatabase.Query(stmt);

    if (!result)
        return 0;

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        int32 item_id = fields[0].GetInt32();

        // if item is a negative, its a reference
        if (item_id < 0)
            count += LoadReferenceVendor(vendor, -item_id, type, skip_vendors);
        else
        {
            int32  maxcount     = fields[1].GetUInt32();
            uint32 incrtime     = fields[2].GetUInt32();
            uint32 ExtendedCost = fields[3].GetUInt32();
            uint8  type         = fields[4].GetUInt8();

            if (!IsVendorItemValid(vendor, item_id, maxcount, incrtime, ExtendedCost, type, NULL, skip_vendors))
                continue;

            if (type != ITEM_VENDOR_TYPE_CURRENCY)
            {
                auto itr = _itemTemplateStore.find(item_id);
                if (itr != _itemTemplateStore.end())
                    itr->second.FlagsCu |= ITEM_FLAGS_CU_VENDOR;
            }

            VendorItemData& vList = _cacheVendorItemStore[vendor];

            vList.AddItem(item_id, maxcount, incrtime, ExtendedCost, type);
            ++count;
        }
    } while (result->NextRow());

    return count;
}

void ObjectMgr::LoadVendors()
{
    uint32 oldMSTime = getMSTime();

    // For reload case
    for (CacheVendorItemContainer::iterator itr = _cacheVendorItemStore.begin(); itr != _cacheVendorItemStore.end(); ++itr)
        itr->second.Clear();
    _cacheVendorItemStore.clear();

    std::set<uint32> skip_vendors;

    QueryResult result = WorldDatabase.Query("SELECT entry, item, maxcount, incrtime, ExtendedCost, type FROM npc_vendor ORDER BY entry, slot ASC");
    if (!result)
    {

        TC_LOG_ERROR("server.loading", ">>  Loaded 0 Vendors. DB table `npc_vendor` is empty!");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 entry        = fields[0].GetUInt32();
        int32 item_id      = fields[1].GetInt32();

        // if item is a negative, its a reference
        if (item_id < 0)
            count += LoadReferenceVendor(entry, -item_id, 0, &skip_vendors);
        else
        {
            uint32 maxcount     = fields[2].GetUInt32();
            uint32 incrtime     = fields[3].GetUInt32();
            uint32 ExtendedCost = fields[4].GetUInt32();
            uint8  type         = fields[5].GetUInt8();

            if (!IsVendorItemValid(entry, item_id, maxcount, incrtime, ExtendedCost, type, NULL, &skip_vendors))
                continue;

            if (type != ITEM_VENDOR_TYPE_CURRENCY)
            {
                auto itr = _itemTemplateStore.find(item_id);
                if (itr != _itemTemplateStore.end())
                    itr->second.FlagsCu |= ITEM_FLAGS_CU_VENDOR;
            }

            VendorItemData& vList = _cacheVendorItemStore[entry];

            vList.AddItem(item_id, maxcount, incrtime, ExtendedCost, type);
            ++count;
        }
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %d Vendors in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadGossipMenu()
{
    uint32 oldMSTime = getMSTime();

    _gossipMenusStore.clear();

    //                                               0       1
    QueryResult result = WorldDatabase.Query("SELECT MenuID, TextID FROM gossip_menu");

    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 gossip_menu IDs. DB table `gossip_menu` is empty!");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        GossipMenus gMenu;

        gMenu.MenuID = fields[0].GetUInt32();
        gMenu.TextID = fields[1].GetUInt32();

        if (!GetGossipText(gMenu.TextID))
        {
            TC_LOG_ERROR("sql.sql", "Table gossip_menu: ID %u is using non-existing TextID %u", gMenu.MenuID, gMenu.TextID);
            continue;
        }

        _gossipMenusStore.insert(GossipMenusContainer::value_type(gMenu.MenuID, gMenu));
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u gossip_menu IDs in %u ms", uint32(_gossipMenusStore.size()), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadGossipMenuItems()
{
    uint32 oldMSTime = getMSTime();

    _gossipMenuItemsStore.clear();

    QueryResult result = WorldDatabase.Query(
        //      0       1         2           3           4                      5           6              7             8            9         10        11       12
        "SELECT MenuID, OptionID, OptionIcon, OptionText, OptionBroadcastTextID, OptionType, OptionNpcFlag, ActionMenuID, ActionPoiID, BoxCoded, BoxMoney, BoxText, BoxBroadcastTextID "
        "FROM gossip_menu_option ORDER BY MenuID, OptionID");

    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 gossip_menu_option IDs. DB table `gossip_menu_option` is empty!");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        GossipMenuItems gMenuItem;

        gMenuItem.MenuID                = fields[0].GetUInt32();
        gMenuItem.OptionID              = fields[1].GetUInt32();
        gMenuItem.OptionIcon            = GossipOptionIcon(fields[2].GetUInt32());
        gMenuItem.OptionText            = fields[3].GetString();
        gMenuItem.OptionBroadcastTextID = fields[4].GetUInt32();
        gMenuItem.OptionType            = fields[5].GetUInt32();
        gMenuItem.OptionNpcFlag         = fields[6].GetUInt32();
        gMenuItem.ActionMenuID          = fields[7].GetUInt32();
        gMenuItem.ActionPoiID           = fields[8].GetUInt32();
        gMenuItem.BoxCoded              = fields[9].GetBool();
        gMenuItem.BoxMoney              = fields[10].GetUInt32();
        gMenuItem.BoxText               = fields[11].GetString();
        gMenuItem.BoxBroadcastTextID    = fields[12].GetUInt32();

        if (gMenuItem.OptionIcon >= GOSSIP_ICON_MAX)
        {
            TC_LOG_ERROR("sql.sql", "Table `gossip_menu_option` for MenuId %u, OptionIndex %u has unknown icon id %u. Replacing with GOSSIP_ICON_CHAT", gMenuItem.MenuID, gMenuItem.OptionID, gMenuItem.OptionIcon);
            gMenuItem.OptionIcon = GOSSIP_ICON_CHAT;
        }

        if (gMenuItem.OptionBroadcastTextID)
        {
            if (!GetBroadcastText(gMenuItem.OptionBroadcastTextID))
            {
                TC_LOG_ERROR("sql.sql", "Table `gossip_menu_option` for MenuId %u, OptionIndex %u has non-existing or incompatible OptionBroadcastTextID %u, ignoring.", gMenuItem.MenuID, gMenuItem.OptionID, gMenuItem.OptionBroadcastTextID);
                gMenuItem.OptionBroadcastTextID = 0;
            }
        }

        if (gMenuItem.OptionType >= GOSSIP_OPTION_MAX)
            TC_LOG_ERROR("sql.sql", "Table `gossip_menu_option` for MenuId %u, OptionIndex %u has unknown option id %u. Option will not be used", gMenuItem.MenuID, gMenuItem.OptionID, gMenuItem.OptionType);

        if (gMenuItem.ActionPoiID && !GetPointOfInterest(gMenuItem.ActionPoiID))
        {
            TC_LOG_ERROR("sql.sql", "Table `gossip_menu_option` for MenuId %u, OptionIndex %u use non-existing ActionPoiID %u, ignoring", gMenuItem.MenuID, gMenuItem.OptionID, gMenuItem.ActionPoiID);
            gMenuItem.ActionPoiID = 0;
        }

        if (gMenuItem.BoxBroadcastTextID)
        {
            if (!GetBroadcastText(gMenuItem.BoxBroadcastTextID))
            {
                TC_LOG_ERROR("sql.sql", "Table `gossip_menu_option` for MenuId %u, OptionIndex %u has non-existing or incompatible BoxBroadcastTextID %u, ignoring.", gMenuItem.MenuID, gMenuItem.OptionID, gMenuItem.BoxBroadcastTextID);
                gMenuItem.BoxBroadcastTextID = 0;
            }
        }

        _gossipMenuItemsStore.insert(GossipMenuItemsContainer::value_type(gMenuItem.MenuID, gMenuItem));
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u gossip_menu_option entries in %u ms", uint32(_gossipMenuItemsStore.size()), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::AddVendorItem(uint32 entry, uint32 item, int32 maxcount, uint32 incrtime, uint32 extendedCost, uint8 type, bool persist /*= true*/)
{
    VendorItemData& vList = _cacheVendorItemStore[entry];
    vList.AddItem(item, maxcount, incrtime, extendedCost, type);

    if (persist)
    {
        WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_INS_NPC_VENDOR);

        stmt->setUInt32(0, entry);
        stmt->setUInt32(1, item);
        stmt->setUInt8(2, maxcount);
        stmt->setUInt32(3, incrtime);
        stmt->setUInt32(4, extendedCost);
        stmt->setUInt8(5, type);

        WorldDatabase.Execute(stmt);
    }
}

bool ObjectMgr::RemoveVendorItem(uint32 entry, uint32 item, uint8 type, bool persist /*= true*/)
{
    CacheVendorItemContainer::iterator  iter = _cacheVendorItemStore.find(entry);
    if (iter == _cacheVendorItemStore.end())
        return false;

    if (!iter->second.RemoveItem(item, type))
        return false;

    if (persist)
    {
        WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_NPC_VENDOR);

        stmt->setUInt32(0, entry);
        stmt->setUInt32(1, item);
        stmt->setUInt8(2, type);

        WorldDatabase.Execute(stmt);
    }

    return true;
}

bool ObjectMgr::IsVendorItemValid(uint32 vendor_entry, uint32 id, int32 maxcount, uint32 incrtime, uint32 ExtendedCost, uint8 type, Player* player, std::set<uint32>* skip_vendors, uint32 ORnpcflag) const
{
    /*
    CreatureTemplate const* cInfo = sObjectMgr->GetCreatureTemplate(vendor_entry);
    if (!cInfo)
    {
        if (player)
            ChatHandler(player->GetSession()).SendSysMessage(LANG_COMMAND_VENDORSELECTION);
        else
            TC_LOG_ERROR("sql.sql", "Table `(game_event_)npc_vendor` have data for not existed creature template (Entry: %u), ignore", vendor_entry);
        return false;
    }

    if (!((cInfo->npcflag | ORnpcflag) & UNIT_NPC_FLAG_VENDOR))
    {
        if (!skip_vendors || skip_vendors->count(vendor_entry) == 0)
        {
            if (player)
                ChatHandler(player->GetSession()).SendSysMessage(LANG_COMMAND_VENDORSELECTION);
            else
                TC_LOG_ERROR("sql.sql", "Table `(game_event_)npc_vendor` have data for not creature template (Entry: %u) without vendor flag, ignore", vendor_entry);

            if (skip_vendors)
                skip_vendors->insert(vendor_entry);
        }
        return false;
    }
    */
    if ((type == ITEM_VENDOR_TYPE_ITEM && !sObjectMgr->GetItemTemplate(id)) ||
        (type == ITEM_VENDOR_TYPE_CURRENCY && !sCurrencyTypesStore.LookupEntry(id)))
    {
        if (player)
            ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_NOT_FOUND, id, type);
        else
            TC_LOG_ERROR("sql.sql", "Table `(game_event_)npc_vendor` for Vendor (Entry: %u) have in item list non-existed item (%u, type %u), ignore", vendor_entry, id, type);
        return false;
    }

    if (ExtendedCost && !sItemExtendedCostStore.LookupEntry(ExtendedCost))
    {
        if (player)
            ChatHandler(player->GetSession()).PSendSysMessage(LANG_EXTENDED_COST_NOT_EXIST, ExtendedCost);
        else
            TC_LOG_ERROR("sql.sql", "Table `(game_event_)npc_vendor` have Item (Entry: %u) with wrong ExtendedCost (%u) for vendor (%u), ignore", id, ExtendedCost, vendor_entry);
        return false;
    }

    if (type == ITEM_VENDOR_TYPE_ITEM) // not applicable to currencies
    {
        if (maxcount > 0 && incrtime == 0)
        {
            if (player)
                ChatHandler(player->GetSession()).PSendSysMessage("MaxCount != 0 (%u) but IncrTime == 0", maxcount);
            else
                TC_LOG_ERROR("sql.sql", "Table `(game_event_)npc_vendor` has `maxcount` (%u) for item %u of vendor (Entry: %u) but `incrtime`=0, ignore", maxcount, id, vendor_entry);
            return false;
        }
        else if (maxcount == 0 && incrtime > 0)
        {
            if (player)
                ChatHandler(player->GetSession()).PSendSysMessage("MaxCount == 0 but IncrTime<>= 0");
            else
                TC_LOG_ERROR("sql.sql", "Table `(game_event_)npc_vendor` has `maxcount`=0 for item %u of vendor (Entry: %u) but `incrtime`<>0, ignore", id, vendor_entry);
            return false;
        }
    }

    VendorItemData const* vItems = GetNpcVendorItemList(vendor_entry);
    if (!vItems)
        return true;                                        // later checks for non-empty lists

    if (vItems->FindItemCostPair(id, ExtendedCost, type))
    {
        if (player)
            ChatHandler(player->GetSession()).PSendSysMessage(LANG_ITEM_ALREADY_IN_LIST, id, ExtendedCost, type);
        else
            TC_LOG_ERROR("sql.sql", "Table `npc_vendor` has duplicate items %u (with extended cost %u, type %u) for vendor (Entry: %u), ignoring", id, ExtendedCost, type, vendor_entry);
        return false;
    }

    if (type == ITEM_VENDOR_TYPE_CURRENCY && maxcount == 0)
    {
        TC_LOG_ERROR("sql.sql", "Table `(game_event_)npc_vendor` have Item (Entry: %u, type: %u) with missing maxcount for vendor (%u), ignore", id, type, vendor_entry);
        return false;
    }

    return true;
}

void ObjectMgr::LoadScriptNames()
{
    uint32 oldMSTime = getMSTime();

    _scriptNamesStore.emplace_back("");

    QueryResult result = WorldDatabase.Query(
        "SELECT DISTINCT(ScriptName) FROM achievement_criteria_data WHERE ScriptName <> '' AND type = 11 "
        "UNION "
        "SELECT DISTINCT(ScriptName) FROM battleground_template WHERE ScriptName <> '' "
        "UNION "
        "SELECT DISTINCT(ScriptName) FROM creature WHERE ScriptName <> '' "
        "UNION "
        "SELECT DISTINCT(ScriptName) FROM creature_template WHERE ScriptName <> '' "
        "UNION "
        "SELECT DISTINCT(ScriptName) FROM gameobject WHERE ScriptName <> '' "
        "UNION "
        "SELECT DISTINCT(ScriptName) FROM gameobject_template WHERE ScriptName <> '' "
        "UNION "
        "SELECT DISTINCT(ScriptName) FROM item_script_names WHERE ScriptName <> '' "
        "UNION "
        "SELECT DISTINCT(ScriptName) FROM areatrigger_scripts WHERE ScriptName <> '' "
        "UNION "
        "SELECT DISTINCT(ScriptName) FROM spell_script_names WHERE ScriptName <> '' "
        "UNION "
        "SELECT DISTINCT(ScriptName) FROM game_weather WHERE ScriptName <> '' "
        "UNION "
        "SELECT DISTINCT(ScriptName) FROM conditions WHERE ScriptName <> '' "
        "UNION "
        "SELECT DISTINCT(ScriptName) FROM game_event WHERE ScriptName <> '' "
        "UNION "
        "SELECT DISTINCT(ScriptName) FROM outdoorpvp_template WHERE ScriptName <> '' "
        "UNION "
        "SELECT DISTINCT(ScriptName) FROM world_map_template WHERE ScriptName <> '' "
        "UNION "
        "SELECT DISTINCT(script) FROM instance_template WHERE script <> '' "
        "UNION "
        "SELECT DISTINCT(ScriptName) FROM scene_template WHERE ScriptName <> '' "
        "UNION "
        "SELECT DISTINCT(ScriptName) FROM quest_template_addon WHERE ScriptName <> '' "
        "UNION "
        "SELECT DISTINCT(ScriptName) FROM spell_areatrigger_template WHERE ScriptName <> ''");

    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded empty set of Script Names!");
        return;
    }

    _scriptNamesStore.reserve(result->GetRowCount() + 1);

    do
    {
        _scriptNamesStore.emplace_back((*result)[0].GetString());
    }
    while (result->NextRow());

    std::sort(_scriptNamesStore.begin(), _scriptNamesStore.end());

#ifdef SCRIPTS
    for (size_t i = 1; i < _scriptNamesStore.size(); ++i)
        UnusedScriptNames.push_back(_scriptNamesStore[i]);
#endif

    TC_LOG_INFO("server.loading", ">> Loaded " SZFMTD " ScriptNames in %u ms", _scriptNamesStore.size(), GetMSTimeDiffToNow(oldMSTime));
}

ObjectMgr::ScriptNameContainer const& ObjectMgr::GetAllScriptNames() const
{
    return _scriptNamesStore;
}

std::string const& ObjectMgr::GetScriptName(uint32 id) const
{
    static std::string const empty = "";
    return (id < _scriptNamesStore.size()) ? _scriptNamesStore[id] : empty;
}

uint32 ObjectMgr::GetScriptId(std::string const& name)
{
    // use binary search to find the script name in the sorted vector
    // assume "" is the first element
    if (name.empty())
        return 0;

    ScriptNameContainer::const_iterator itr = std::lower_bound(_scriptNamesStore.begin(), _scriptNamesStore.end(), name);
    if (itr == _scriptNamesStore.end() || (*itr != name))
        return 0;

    return uint32(itr - _scriptNamesStore.begin());
}

void ObjectMgr::LoadBroadcastTexts()
{
    uint32 oldMSTime = getMSTime();

    _broadcastTextStore.clear(); // for reload case

    //                                               0   1            2      3      4         5         6         7            8            9            10              11        12
    QueryResult result = WorldDatabase.Query("SELECT ID, LanguageID, `Text`, Text1, EmoteID1, EmoteID2, EmoteID3, EmoteDelay1, EmoteDelay2, EmoteDelay3, SoundEntriesID, EmotesID, Flags FROM broadcast_text");
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 broadcast texts. DB table `broadcast_text` is empty.");
        return;
    }

    _broadcastTextStore.reserve(result->GetRowCount());

    do
    {
        Field* fields = result->Fetch();

        BroadcastText bct;

        bct.Id = fields[0].GetUInt32();
        bct.LanguageID = fields[1].GetUInt32();
        bct.Text[DEFAULT_LOCALE] = fields[2].GetString();
        bct.Text1[DEFAULT_LOCALE] = fields[3].GetString();
        bct.EmoteId1 = fields[4].GetUInt32();
        bct.EmoteId2 = fields[5].GetUInt32();
        bct.EmoteId3 = fields[6].GetUInt32();
        bct.EmoteDelay1 = fields[7].GetUInt32();
        bct.EmoteDelay2 = fields[8].GetUInt32();
        bct.EmoteDelay3 = fields[9].GetUInt32();
        bct.SoundEntriesID = fields[10].GetUInt32();
        bct.EmotesID = fields[11].GetUInt32();
        bct.Flags = fields[12].GetUInt32();

        if (bct.SoundEntriesID)
        {
            if (!sSoundEntriesStore.LookupEntry(bct.SoundEntriesID))
            {
                TC_LOG_DEBUG("broadcasttext", "BroadcastText (Id: %u) in table `broadcast_text` has SoundEntriesID %u but sound does not exist.", bct.Id, bct.SoundEntriesID);
                bct.SoundEntriesID = 0;
            }
        }

        if (!GetLanguageDescByID(bct.LanguageID))
        {
            TC_LOG_DEBUG("broadcasttext", "BroadcastText (Id: %u) in table `broadcast_text` using LanguageID %u but Language does not exist.", bct.Id, bct.LanguageID);
            bct.LanguageID = LANG_UNIVERSAL;
        }

        if (bct.EmoteId1)
        {
            if (!sEmotesStore.LookupEntry(bct.EmoteId1))
            {
                TC_LOG_DEBUG("broadcasttext", "BroadcastText (Id: %u) in table `broadcast_text` has EmoteId1 %u but emote does not exist.", bct.Id, bct.EmoteId1);
                bct.EmoteId1 = 0;
            }
        }

        if (bct.EmoteId2)
        {
            if (!sEmotesStore.LookupEntry(bct.EmoteId2))
            {
                TC_LOG_DEBUG("broadcasttext", "BroadcastText (Id: %u) in table `broadcast_text` has EmoteId2 %u but emote does not exist.", bct.Id, bct.EmoteId2);
                bct.EmoteId2 = 0;
            }
        }

        if (bct.EmoteId3)
        {
            if (!sEmotesStore.LookupEntry(bct.EmoteId3))
            {
                TC_LOG_DEBUG("broadcasttext", "BroadcastText (Id: %u) in table `broadcast_text` has EmoteId3 %u but emote does not exist.", bct.Id, bct.EmoteId3);
                bct.EmoteId3 = 0;
            }
        }

        _broadcastTextStore[bct.Id] = bct;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded " SZFMTD " broadcast texts in %u ms", _broadcastTextStore.size(), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadBroadcastTextLocales()
{
    uint32 oldMSTime = getMSTime();

    //                                               0   1        2     3
    QueryResult result = WorldDatabase.Query("SELECT ID, locale, `Text`, Text1 FROM broadcast_text_locale");
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 broadcast text locales. DB table `broadcast_text_locale` is empty.");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 id               = fields[0].GetUInt32();
        std::string localeName  = fields[1].GetString();
        std::string Text    = fields[2].GetString();
        std::string Text1  = fields[3].GetString();

        BroadcastTextContainer::iterator bct = _broadcastTextStore.find(id);
        if (bct == _broadcastTextStore.end())
        {
            TC_LOG_ERROR("sql.sql", "BroadcastText (Id: %u) in table `broadcast_text_locale` does not exist. Skipped!", id);
            continue;
        }

        LocaleConstant locale = GetLocaleByName(localeName);
        if (!sWorld->getBoolConfig(CONFIG_LOAD_LOCALES) || locale == LOCALE_enUS)
            continue;

        AddLocaleString(Text, locale, bct->second.Text);
        AddLocaleString(Text1, locale, bct->second.Text1);
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u broadcast text locales in %u ms", uint32(_broadcastTextStore.size()), GetMSTimeDiffToNow(oldMSTime));
}

CreatureBaseStats const* ObjectMgr::GetCreatureBaseStats(uint8 level, uint8 unitClass)
{
    CreatureBaseStatsContainer::const_iterator it = _creatureBaseStatsStore.find(MAKE_PAIR16(level, unitClass));

    if (it != _creatureBaseStatsStore.end())
        return &(it->second);

    struct DefaultCreatureBaseStats : public CreatureBaseStats
    {
        DefaultCreatureBaseStats()
        {
            BaseArmor = 1;
            for (uint8 j = 0; j < MAX_CREATURE_BASE_HP; ++j)
                BaseHealth[j] = 1;
            BaseMana = 0;
        }
    };
    static const DefaultCreatureBaseStats def_stats;
    return &def_stats;
}

void ObjectMgr::LoadCreatureClassLevelStats()
{
    uint32 oldMSTime = getMSTime();
    //                                                 0      1       2        3        4        5        6         7         8          9           10                  11            12           13           14           15
    QueryResult result = WorldDatabase.Query("SELECT level, class, basehp0, basehp1, basehp2, basehp3, basehp4, basemana, basearmor, attackpower, rangedattackpower, damage_base, damage_exp1, damage_exp2, damage_exp3, damage_exp4 FROM creature_classlevelstats");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 creature base stats. DB table `creature_classlevelstats` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint8 Level = fields[0].GetInt8();
        uint8 Class = fields[1].GetInt8();

        CreatureBaseStats stats;

        for (uint8 i = 0; i < MAX_CREATURE_BASE_HP; ++i)
            stats.BaseHealth[i] = fields[i + 2].GetUInt32();

        stats.BaseMana = fields[7].GetUInt32();
        stats.BaseArmor = fields[8].GetUInt32();

        stats.AttackPower = fields[9].GetUInt16();
        stats.RangedAttackPower = fields[10].GetUInt16();

        for (uint8 i = 0; i < MAX_CREATURE_BASE_DAMAGE; ++i)
            stats.BaseDamage[i] = fields[i + 11].GetFloat();

        if (!Class || ((1 << (Class - 1)) & CLASSMASK_ALL_CREATURES) == 0)
            TC_LOG_ERROR("sql.sql", "Creature base stats for level %u has invalid class %u", Level, Class);

        for (uint8 i = 0; i < MAX_CREATURE_BASE_HP; ++i)
        {
            if (stats.BaseHealth[i] < 1)
            {
                TC_LOG_ERROR("sql.sql", "Creature base stats for class %u, level %u has invalid zero base HP[%u] - set to 1", Class, Level, i);
                stats.BaseHealth[i] = 1;
            }
        }

        _creatureBaseStatsStore[MAKE_PAIR16(Level, Class)] = stats;

        ++count;
    }
    while (result->NextRow());

    CreatureTemplateContainer const* ctc = sObjectMgr->GetCreatureTemplates();
    for (CreatureTemplateContainer::const_iterator itr = ctc->begin(); itr != ctc->end(); ++itr)
    {
        for (uint16 lvl = itr->second.minlevel; lvl <= itr->second.maxlevel; ++lvl)
        {
            if (_creatureBaseStatsStore.find(MAKE_PAIR16(lvl, itr->second.unit_class)) == _creatureBaseStatsStore.end())
                TC_LOG_ERROR("sql.sql", "Missing base stats for creature class %u level %u", itr->second.unit_class, lvl);
        }
    }

    TC_LOG_INFO("server.loading", ">> Loaded %u creature base stats in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadFactionChangeAchievements()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT alliance_id, horde_id FROM player_factionchange_achievement");

    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 faction change achievement pairs. DB table `player_factionchange_achievement` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 alliance = fields[0].GetUInt32();
        uint32 horde = fields[1].GetUInt32();

        if (!sAchievementMgr->GetAchievement(alliance))
            TC_LOG_ERROR("sql.sql", "Achievement %u (alliance_id) referenced in `player_factionchange_achievement` does not exist, pair skipped!", alliance);
        else if (!sAchievementMgr->GetAchievement(horde))
            TC_LOG_ERROR("sql.sql", "Achievement %u (horde_id) referenced in `player_factionchange_achievement` does not exist, pair skipped!", horde);
        else
            FactionChangeAchievements[alliance] = horde;

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u faction change achievement pairs in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadFactionChangeItems()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT alliance_id, horde_id FROM player_factionchange_items");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 faction change item pairs. DB table `player_factionchange_items` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 alliance = fields[0].GetUInt32();
        uint32 horde = fields[1].GetUInt32();

        if (!GetItemTemplate(alliance))
            TC_LOG_ERROR("sql.sql", "Item %u (alliance_id) referenced in `player_factionchange_items` does not exist, pair skipped!", alliance);
        else if (!GetItemTemplate(horde))
            TC_LOG_ERROR("sql.sql", "Item %u (horde_id) referenced in `player_factionchange_items` does not exist, pair skipped!", horde);
        else
            FactionChangeItems[alliance] = horde;

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u faction change item pairs in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadFactionChangeQuests()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT alliance_id, horde_id FROM player_factionchange_quests");

    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 faction change quest pairs. DB table `player_factionchange_quests` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 alliance = fields[0].GetUInt32();
        uint32 horde = fields[1].GetUInt32();

        if (!sObjectMgr->GetQuestTemplate(alliance))
            TC_LOG_ERROR("sql.sql", "Quest %u (alliance_id) referenced in `player_factionchange_quests` does not exist, pair skipped!", alliance);
        else if (!sObjectMgr->GetQuestTemplate(horde))
            TC_LOG_ERROR("sql.sql", "Quest %u (horde_id) referenced in `player_factionchange_quests` does not exist, pair skipped!", horde);
        else
            FactionChangeQuests[alliance] = horde;

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u faction change quest pairs in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadFactionChangeReputations()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT alliance_id, horde_id FROM player_factionchange_reputations");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 faction change reputation pairs. DB table `player_factionchange_reputations` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 alliance = fields[0].GetUInt32();
        uint32 horde = fields[1].GetUInt32();

        if (!sFactionStore.LookupEntry(alliance))
            TC_LOG_ERROR("sql.sql", "Reputation %u (alliance_id) referenced in `player_factionchange_reputations` does not exist, pair skipped!", alliance);
        else if (!sFactionStore.LookupEntry(horde))
            TC_LOG_ERROR("sql.sql", "Reputation %u (horde_id) referenced in `player_factionchange_reputations` does not exist, pair skipped!", horde);
        else
            FactionChangeReputation[alliance] = horde;

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u faction change reputation pairs in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadHotfixData()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT entry, type, UNIX_TIMESTAMP(hotfixDate) FROM hotfix_data");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 hotfix info entries. DB table `hotfix_data` is empty.");
        return;
    }

    uint32 count = 0;

    _hotfixData.reserve(result->GetRowCount());

    do
    {
        Field* fields = result->Fetch();

        HotfixInfo info;
        info.Entry = fields[0].GetUInt32();
        info.Type = fields[1].GetUInt32();
        info.Timestamp = fields[2].GetUInt64();
        _hotfixData.push_back(info);

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u hotfix info entries in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

time_t ObjectMgr::GetHotfixDate(uint32 entry, uint32 type) const
{
    time_t ret = 0;
    for (HotfixData::const_iterator itr = _hotfixData.begin(); itr != _hotfixData.end(); ++itr)
        if (itr->Entry == entry && itr->Type == type)
            if (itr->Timestamp > ret)
                ret = itr->Timestamp;

    return ret ? ret : time(NULL);
}

void ObjectMgr::LoadMissingKeyChains()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT keyId, k1, k2, k3, k4, k5, k6, k7, k8, "
                                                     "k9, k10, k11, k12, k13, k14, k15, k16, "
                                                     "k17, k18, k19, k20, k21, k22, k23, k24, "
                                                     "k25, k26, k27, k28, k29, k30, k31, k32 "
                                                     "FROM keychain_db2 ORDER BY keyId DESC");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 KeyChain entries. DB table `keychain_db2` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        uint32 id = fields[0].GetUInt32();

        KeyChainEntry* kce = sKeyChainStore.CreateEntry(id, true);
        kce->Id = id;
        for (uint32 i = 0; i < KEYCHAIN_SIZE; ++i)
            kce->Key[i] = fields[1 + i].GetUInt8();

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u KeyChain entries in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadFactionChangeSpells()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT alliance_id, horde_id FROM player_factionchange_spells");

    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 faction change spell pairs. DB table `player_factionchange_spells` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 alliance = fields[0].GetUInt32();
        uint32 horde = fields[1].GetUInt32();

        if (!sSpellMgr->GetSpellInfo(alliance))
            TC_LOG_ERROR("sql.sql", "Spell %u (alliance_id) referenced in `player_factionchange_spells` does not exist, pair skipped!", alliance);
        else if (!sSpellMgr->GetSpellInfo(horde))
            TC_LOG_ERROR("sql.sql", "Spell %u (horde_id) referenced in `player_factionchange_spells` does not exist, pair skipped!", horde);
        else
            FactionChangeSpells[alliance] = horde;

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u faction change spell pairs in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadFactionChangeTitles()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT alliance_id, horde_id FROM player_factionchange_titles");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 faction change title pairs. DB table `player_factionchange_title` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 alliance = fields[0].GetUInt32();
        uint32 horde = fields[1].GetUInt32();

        if (!sCharTitlesStore.LookupEntry(alliance))
            TC_LOG_ERROR("sql.sql", "Title %u (alliance_id) referenced in `player_factionchange_title` does not exist, pair skipped!", alliance);
        else if (!sCharTitlesStore.LookupEntry(horde))
            TC_LOG_ERROR("sql.sql", "Title %u (horde_id) referenced in `player_factionchange_title` does not exist, pair skipped!", horde);
        else
            FactionChangeTitles[alliance] = horde;

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u faction change title pairs in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadPhaseDefinitions()
{
    _PhaseDefinitionStore.clear();

    uint32 oldMSTime = getMSTime();

    //                                                 0       1       2         3            4           5             6
    QueryResult result = WorldDatabase.Query("SELECT zoneId, entry, phasemask, phaseId, terrainswapmap, worldMapArea, flags FROM `phase_definitions` ORDER BY `entry` ASC");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 phasing definitions. DB table `phase_definitions` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        PhaseDefinition PhaseDefinition;

        PhaseDefinition.zoneId                = fields[0].GetUInt32();
        PhaseDefinition.entry                 = fields[1].GetUInt32();
        PhaseDefinition.phasemask             = fields[2].GetUInt32();
        PhaseDefinition.phaseId               = fields[3].GetUInt32();
        PhaseDefinition.terrainswapmap        = fields[4].GetUInt32();
        PhaseDefinition.worldMapArea          = fields[5].GetUInt32();
        PhaseDefinition.flags                 = fields[6].GetUInt32();

        // Checks
        if ((PhaseDefinition.flags & PHASE_FLAG_OVERWRITE_EXISTING) && (PhaseDefinition.flags & PHASE_FLAG_NEGATE_PHASE))
        {
            TC_LOG_ERROR("sql.sql", "Flags defined in phase_definitions in zoneId %d and entry %u does contain PHASE_FLAG_OVERWRITE_EXISTING and PHASE_FLAG_NEGATE_PHASE. Setting flags to PHASE_FLAG_OVERWRITE_EXISTING", PhaseDefinition.zoneId, PhaseDefinition.entry);
            PhaseDefinition.flags &= ~PHASE_FLAG_NEGATE_PHASE;
        }

        _PhaseDefinitionStore[PhaseDefinition.zoneId].push_back(PhaseDefinition);

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u phasing definitions in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadTerrainSwapDefaults()
{
    _terrainMapDefaultStore.clear();

    uint32 oldMSTime = getMSTime();

    //                                               0       1
    QueryResult result = WorldDatabase.Query("SELECT MapId, TerrainSwapMap FROM `terrain_swap_defaults`");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 terrain swap defaults. DB table `terrain_swap_defaults` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 mapId = fields[0].GetUInt32();

        MapEntry const* map = sMapStore.LookupEntry(mapId);
        if (!map)
        {
            TC_LOG_INFO("sql.sql", "Map %u defined in `terrain_swap_defaults` does not exist, skipped.", mapId);
            continue;
        }

        uint32 terrainSwap = fields[1].GetUInt32();

        map = sMapStore.LookupEntry(terrainSwap);
        if (!map)
        {
            TC_LOG_INFO("sql.sql", "TerrainSwapMap %u defined in `terrain_swap_defaults` does not exist, skipped.", terrainSwap);
            continue;
        }
        _terrainMapDefaultStore[mapId].push_back(terrainSwap);

        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u terrain swap defaults in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadTerrainPhaseInfo()
{
    _terrainPhaseInfoStore.clear();

    uint32 oldMSTime = getMSTime();

    //                                               0       1
    QueryResult result = WorldDatabase.Query("SELECT Id, TerrainSwapMap FROM `terrain_phase_info`");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 terrain phase infos. DB table `terrain_phase_info` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 phaseId = fields[0].GetUInt32();

        PhaseEntry const* phase = sPhaseStore.LookupEntry(phaseId);
        if (!phase)
        {
            TC_LOG_INFO("sql.sql", "Phase %u defined in `terrain_phase_info` does not exist, skipped.", phaseId);
            continue;
        }

        uint32 terrainSwap = fields[1].GetUInt32();
        _terrainPhaseInfoStore[phaseId].push_back(terrainSwap);

        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u terrain phase infos in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadTerrainWorldMaps()
{
    _terrainWorldMapStore.clear();

    uint32 oldMSTime = getMSTime();

    //                                               0               1
    QueryResult result = WorldDatabase.Query("SELECT TerrainSwapMap, WorldMapArea FROM `terrain_worldmap`");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 terrain world maps. DB table `terrain_worldmap` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 mapId = fields[0].GetUInt32();

        if (!sMapStore.LookupEntry(mapId))
        {
            TC_LOG_INFO("sql.sql", "TerrainSwapMap %u defined in `terrain_worldmap` does not exist, skipped.", mapId);
            continue;
        }

        uint32 worldMapArea = fields[1].GetUInt32();

        _terrainWorldMapStore[mapId].push_back(worldMapArea);

        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u terrain world maps in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadAreaPhases()
{
    _areaPhases.clear();

    uint32 oldMSTime = getMSTime();

    //                                               0       1
    QueryResult result = WorldDatabase.Query("SELECT AreaId, PhaseId FROM `phase_area`");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 phase areas. DB table `phase_area` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        PhaseInfoStruct phase;
        uint32 area = fields[0].GetUInt32();
        phase.id = fields[1].GetUInt32();

        if (!sAreaTableStore.LookupEntry(area))
        {
            TC_LOG_ERROR("sql.sql", "Area %u defined in `phase_area` does not exist, skipped.", area);
            continue;
        }

        if (!sPhaseStore.LookupEntry(phase.id))
        {
            TC_LOG_ERROR("sql.sql", "Phase %u defined in `phase_area` does not exist, skipped.", phase.id);
            continue;
        }

        _areaPhases[area].push_back(phase);

        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u phase areas in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadSpellPhaseInfo()
{
    _SpellPhaseStore.clear();

    uint32 oldMSTime = getMSTime();

    //                                               0       1            2
    QueryResult result = WorldDatabase.Query("SELECT id, phasemask, terrainswapmap FROM `spell_phase`");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 spell dbc infos. DB table `spell_phase` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        SpellPhaseInfo spellPhaseInfo;
        spellPhaseInfo.spellId                = fields[0].GetUInt32();

        SpellInfo const* spell = sSpellMgr->GetSpellInfo(spellPhaseInfo.spellId);
        if (!spell)
        {
            TC_LOG_ERROR("sql.sql", "Spell %u defined in `spell_phase` does not exists, skipped.", spellPhaseInfo.spellId);
            continue;
        }

        if (!spell->HasAura(SPELL_AURA_PHASE))
        {
            TC_LOG_ERROR("sql.sql", "Spell %u defined in `spell_phase` does not have aura effect type SPELL_AURA_PHASE, useless value.", spellPhaseInfo.spellId);
            continue;
        }

        spellPhaseInfo.phasemask              = fields[1].GetUInt32();
        spellPhaseInfo.terrainswapmap         = fields[2].GetUInt32();

        _SpellPhaseStore[spellPhaseInfo.spellId] = spellPhaseInfo;

        ++count;
    }
    while (result->NextRow());
    TC_LOG_INFO("server.loading", ">> Loaded %u spell dbc infos in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

GameObjectTemplate const* ObjectMgr::GetGameObjectTemplate(uint32 entry)
{
    GameObjectTemplateContainer::const_iterator itr = _gameObjectTemplateStore.find(entry);
    if (itr != _gameObjectTemplateStore.end())
        return &(itr->second);

    return NULL;
}

GameObjectTemplateAddon const* ObjectMgr::GetGameObjectTemplateAddon(uint32 entry) const
{
    auto itr = _gameObjectTemplateAddonStore.find(entry);
    if (itr != _gameObjectTemplateAddonStore.end())
        return &itr->second;

    return nullptr;
}

CreatureTemplate const* ObjectMgr::GetCreatureTemplate(uint32 entry) const
{
    CreatureTemplateContainer::const_iterator itr = _creatureTemplateStore.find(entry);
    if (itr != _creatureTemplateStore.end())
        return &(itr->second);

    return nullptr;
}

AreaTriggerTemplate const* ObjectMgr::GetAreaTriggerTemplate(uint32 entry) const
{
    AreaTriggerTemplateContainer::const_iterator iter = _areaTriggerTemplateStore.find(entry);
    return (iter != _areaTriggerTemplateStore.end()) ? &iter->second : NULL;
}

VehicleAccessoryList const* ObjectMgr::GetVehicleAccessoryList(Vehicle* veh) const
{
    if (Creature* cre = veh->GetBase()->ToCreature())
    {
        // Give preference to GUID-based accessories
        VehicleAccessoryContainer::const_iterator itr = _vehicleAccessoryStore.find(cre->GetDBTableGUIDLow());
        if (itr != _vehicleAccessoryStore.end())
            return &itr->second;
    }

    // Otherwise return entry-based
    VehicleAccessoryContainer::const_iterator itr = _vehicleTemplateAccessoryStore.find(veh->GetCreatureEntry());
    if (itr != _vehicleTemplateAccessoryStore.end())
        return &itr->second;
    return NULL;
}

PlayerInfo const* ObjectMgr::GetPlayerInfo(uint32 race, uint32 class_) const
{
    if (race >= MAX_RACES)
        return NULL;
    if (class_ >= MAX_CLASSES)
        return NULL;
    PlayerInfo const* info = _playerInfo[race][class_];
    if (!info)
        return NULL;
    return info;
}

void ObjectMgr::LoadResearchDigsiteInfo()
{
    _researchDigsiteStore.clear();

    uint32 oldMSTime = getMSTime();

    //                                                   0       1                 3                      4
    QueryResult result = WorldDatabase.Query("SELECT digsiteId, branchId, requiredSkillValue, requiredLevel FROM `research_digsite_data`");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 research digsite infos. DB table `research_digsite_data` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        ResearchDigsiteInfo digsiteInfo;
        digsiteInfo.digsiteId = fields [0].GetUInt32();

        ResearchSiteEntry const* siteEntry = sResearchSiteStore.LookupEntry(digsiteInfo.digsiteId);
        if (!siteEntry)
        {
            TC_LOG_ERROR("sql.sql", "Digsite %u defined in `research_digsite_data` does not exists in DBC, skipped.", digsiteInfo.digsiteId);
            continue;
        }

        digsiteInfo.branchId = fields [1].GetUInt32();
        digsiteInfo.requiredSkillValue = fields [2].GetUInt32();
        digsiteInfo.requiredLevel = fields [3].GetUInt32();
        _researchDigsiteStore [siteEntry->MapId].push_back(digsiteInfo);

        ++count;
    }
    while (result->NextRow());
    TC_LOG_INFO("server.loading", ">> Loaded %u research digsite infos in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadArchaeologyFindInfo()
{
    _archaeologyFindStore.clear();

    uint32 oldMSTime = getMSTime();

    //                                                0        1         2        3           4           5
    QueryResult result = WorldDatabase.Query("SELECT guid, digsiteId, goEntry, position_x, position_y, position_z FROM `research_digsite_finds`");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 archaeology find infos. DB table `research_digsite_finds` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        ArchaeologyFindInfo findInfo;
        findInfo.guid = fields [0].GetUInt32();
        uint32 digsiteId = fields [1].GetUInt32();

        if (!sResearchSiteStore.LookupEntry(digsiteId))
        {
            TC_LOG_ERROR("sql.sql", "Digsite %u referenced in `research_digsite_finds` does not exists in DBC, skipped.", digsiteId);
            continue;
        }

        findInfo.goEntry = fields [2].GetUInt32();
        if (!GetGameObjectTemplate(findInfo.goEntry))
        {
            TC_LOG_ERROR("sql.sql", "Table `research_digsite_finds` has archaeology find with non existing gameobject entry %u (Digsite Id: %u), skipped.", findInfo.goEntry, digsiteId);
            continue;
        }

        findInfo.x = fields [3].GetFloat();
        findInfo.y = fields [4].GetFloat();
        findInfo.z = fields [5].GetFloat();

        _archaeologyFindStore [digsiteId].push_back(findInfo);

        ++count;
    }
    while (result->NextRow());
    TC_LOG_INFO("server.loading", ">> Loaded %u archaeology find infos in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadResearchProjectRequirements()
{
    _researchProjectRequirementStore.clear();

    uint32 oldMSTime = getMSTime();

    //                                                   0              1              2
    QueryResult result = WorldDatabase.Query("SELECT projectId, requiredSkillValue, chance FROM `research_project_requirements`");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 research project requirements. DB table `research_project_requirements` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 projectId = fields [0].GetUInt32();
        if (!sResearchProjectStore.LookupEntry(projectId))
        {
            TC_LOG_ERROR("sql.sql", "Research project %u referenced in `research_project_requirements` does not exists in DBC, skipped.", projectId);
            continue;
        }

        ResearchProjectRequirements requirements;
        requirements.requiredSkillValue = fields [1].GetUInt32();
        requirements.chance = fields [2].GetFloat();

        _researchProjectRequirementStore [projectId] = requirements;

        ++count;
    }
    while (result->NextRow());
    TC_LOG_INFO("server.loading", ">> Loaded %u research project requirements in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

ResearchDigsiteInfo const* ObjectMgr::GetResearchDigsiteInfo(uint32 digsiteId) const
{
    for (ResearchDigsiteContainer::const_iterator itr = _researchDigsiteStore.begin(); itr != _researchDigsiteStore.end(); ++itr)
        for (ResearchDigsiteList::const_iterator digsite = itr->second.begin(); digsite != itr->second.end(); ++digsite)
            if (digsite->digsiteId == digsiteId)
                return &(*digsite);

    return nullptr;
}

ResearchDigsiteList const* ObjectMgr::GetResearchDigsitesForContinent(uint32 mapId) const
{
    ResearchDigsiteContainer::const_iterator iter = _researchDigsiteStore.find(mapId);
    if (iter != _researchDigsiteStore.end())
        return &iter->second;

    return nullptr;    
}

ArchaeologyFindInfo const* ObjectMgr::GetArchaeologyFindInfo(uint32 findGUID, uint32 digsiteId)
{
    ArchaeologyFindContainer::const_iterator itr = _archaeologyFindStore.find(digsiteId);
    if (itr == _archaeologyFindStore.end())
        return NULL;

    for (ArchaeologyFindList::const_iterator find = itr->second.begin(); find != itr->second.end(); ++find)
        if (find->guid == findGUID)
            return &(*find);

    return nullptr;    
}

ArchaeologyFindInfo const* ObjectMgr::GetRandomArchaeologyFindForDigsite(uint32 digsiteId)
{
    ArchaeologyFindContainer::const_iterator itr = _archaeologyFindStore.find(digsiteId);
    if (itr == _archaeologyFindStore.end())
        return nullptr;

    if (itr->second.empty())
        return nullptr;

    return &Trinity::Containers::SelectRandomContainerElement(itr->second);    
}

ArchaeologyFindList const* ObjectMgr::GetArcheologyFindListForDigsite(uint32 digsiteId)
{
    ArchaeologyFindContainer::const_iterator itr = _archaeologyFindStore.find(digsiteId);
    return itr != _archaeologyFindStore.end() ? &itr->second : nullptr;    
}

ResearchProjectRequirements const* ObjectMgr::GetResearchProjectRequirements(uint32 projectId) const
{
    ResearchProjectRequirementContainer::const_iterator iter = _researchProjectRequirementStore.find(projectId);
    if (iter != _researchProjectRequirementStore.end())
        return &iter->second;

    return nullptr;    
}

void ObjectMgr::LoadBattlePetBreedData()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT species, breed FROM `battle_pet_breed`");
    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 Battle Pets breed definitions. DB table `battle_pet_breed` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint16 speciesId = fields[0].GetUInt16();
        uint8 breedId    = fields[1].GetUInt8();

        if (!sBattlePetSpeciesStore.HasRecord(speciesId))
        {
            TC_LOG_ERROR("sql.sql", "Battle Pet species %u defined in `battle_pet_breed` does not exists, skipped.", speciesId);
            continue;
        }

        if (sBattlePetBreedSet.find(breedId) == sBattlePetBreedSet.end())
        {
            TC_LOG_ERROR("sql.sql", "Battle Pet breed %u defined in `battle_pet_breed` does not exists, skipped.", breedId);
            continue;
        }

        if (_battlePetBreedXSpeciesStore.find(speciesId) == _battlePetBreedXSpeciesStore.end())
            _battlePetBreedXSpeciesStore.insert(make_pair(speciesId, BattleBetBreedSet()));

        _battlePetBreedXSpeciesStore[speciesId].insert(breedId);

        count++;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u Battle Pet breed definitions in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadBattlePetQualityData()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT species, quality FROM `battle_pet_quality`");
    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 Battle Pets quality definitions. DB table `battle_pet_quality` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint16 speciesId = fields[0].GetUInt16();
        uint8 quality    = fields[1].GetUInt8();

        if (!sBattlePetSpeciesStore.HasRecord(speciesId))
        {
            TC_LOG_ERROR("sql.sql", "Battle Pet species %u defined in `battle_pet_quality` does not exists, skipped.", speciesId);
            continue;
        }

        if (quality > ITEM_QUALITY_LEGENDARY)
        {
            TC_LOG_ERROR("sql.sql", "Battle Pet quality %u defined in `battle_pet_quality` is invalid, skipped.", quality);
            continue;
        }

        if (_battlePetQualityXSpeciesStore.find(speciesId) == _battlePetQualityXSpeciesStore.end())
            _battlePetQualityXSpeciesStore.insert(make_pair(speciesId, BattlePetQualitySet()));

        _battlePetQualityXSpeciesStore[speciesId].insert(quality);

        count++;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u Battle Pet quality definitions in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadBattlePetItemToSpeciesData()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT itemId, species FROM `battle_pet_item_to_species`");
    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 Battle Pets item to species definitions. DB table `battle_pet_item_to_species` is empty!");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();
        uint32 itemId = fields[0].GetUInt32();
        uint16 speciesId = fields[1].GetUInt16();

        if (!GetItemTemplate(itemId))
        {
            TC_LOG_ERROR("sql.sql", "Item id %u defined in `battle_pet_item_to_species` does not exists, skipped!", itemId);
            continue;
        }

        if (!sBattlePetSpeciesStore.LookupEntry(speciesId))
        {
            TC_LOG_ERROR("sql.sql", "Battle Pet species %u defined in `battle_pet_item_to_species` does not exists, skipped!", speciesId);
            continue;
        }

        _battlePetItemToSpeciesStore[itemId] = speciesId;
        count++;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u Battle Pet quality definitions in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

uint8 ObjectMgr::BattlePetGetRandomBreed(uint32 speciesId) const
{
    for (auto mapCitr = _battlePetBreedXSpeciesStore.begin(); mapCitr != _battlePetBreedXSpeciesStore.end(); mapCitr++)
    {
        if (mapCitr->first == speciesId)
        {
            BattleBetBreedSet::iterator setItr(mapCitr->second.begin());
            std::advance(setItr, urand(0, mapCitr->second.size() - 1));

            return *setItr;
        }
    }

    return 0;
}

uint8 ObjectMgr::BattlePetGetRandomQuality(uint32 speciesId) const
{
    for (auto mapCitr = _battlePetQualityXSpeciesStore.begin(); mapCitr != _battlePetQualityXSpeciesStore.end(); mapCitr++)
    {
        if (mapCitr->first == speciesId)
        {
            BattlePetQualitySet::iterator setItr(mapCitr->second.begin());
            std::advance(setItr, urand(0, mapCitr->second.size() - 1));

            return *setItr;
        }
    }

    return ITEM_QUALITY_NORMAL;
}

uint16 ObjectMgr::BattlePetGetSpeciesFromItem(uint32 itemId) const
{
    auto it = _battlePetItemToSpeciesStore.find(itemId);
    if (it == _battlePetItemToSpeciesStore.end())
        return 0;

    return it->second;
}

void ObjectMgr::LoadSceneTemplates()
{
    uint32 oldMSTime = getMSTime();
    _sceneTemplateStore.clear();

    QueryResult templates = WorldDatabase.Query("SELECT SceneId, Flags, ScriptPackageID, ScriptName FROM scene_template");

    if (!templates)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 scene templates. DB table `scene_template` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = templates->Fetch();

        uint32 sceneId = fields[0].GetUInt32();
        SceneTemplate& sceneTemplate = _sceneTemplateStore[sceneId];
        sceneTemplate.SceneId = sceneId;
        sceneTemplate.PlaybackFlags = fields[1].GetUInt32();
        sceneTemplate.ScenePackageId = fields[2].GetUInt32();
        sceneTemplate.ScriptId = sObjectMgr->GetScriptId(fields[3].GetString());

    } while (templates->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u scene templates in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadQuestObjectiveVisualEffects()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT `objectiveId`, `visualEffect` FROM `quest_objective_effects` ORDER BY `objectiveId` ASC");
    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 Quest Objective visual effects. DB table `quest_objective_effects` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 objectiveId  = fields[0].GetUInt32();
        uint32 visualEffect = fields[1].GetUInt32();

        auto objective = GetQuestObjective(objectiveId);
        if (!objective)
        {
            TC_LOG_ERROR("sql.sql", "Visual effect %u has non existant Quest Objective Id %u! Skipping.", visualEffect, objectiveId);
            continue;
        }

        Quest const* quest = GetQuestTemplate(objective->ID);
        if (!quest)
            continue;

        bool match = false;
        for (auto questObjective : quest->Objectives)
        {
            if (questObjective.ID == objectiveId)
            {
                questObjective.VisualEffects.push_back(visualEffect);
                match = true;
                break;
            }
        }

        if (!match)
        {
            TC_LOG_ERROR("sql.sql", "Visual effect %u has non existant Quest Objective %u for Quest %u! Skipping.", visualEffect, objectiveId, quest->GetQuestId());
            continue;
        }

        count++;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u Quest Objective visual effects in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadQuestStartersAndEnders()
{
    TC_LOG_INFO("server.loading", "Loading GO Start Quest Data...");
    LoadGameobjectQuestStarters();
    TC_LOG_INFO("server.loading", "Loading GO End Quest Data...");
    LoadGameobjectQuestEnders();
    TC_LOG_INFO("server.loading", "Loading Creature Start Quest Data...");
    LoadCreatureQuestStarters();
    TC_LOG_INFO("server.loading", "Loading Creature End Quest Data...");
    LoadCreatureQuestEnders();    
}

void ObjectMgr::LoadQuestTemplateLocale()
{
    uint32 oldMSTime = getMSTime();

    _questTemplateLocaleStore.clear(); // need for reload case
    //                                               0     1
    QueryResult result = WorldDatabase.Query("SELECT Id, locale, "
    //      2           3                 4                5                 6                  7                   8                   9                  10
        "LogTitle, LogDescription, QuestDescription, AreaDescription, PortraitGiverText, PortraitGiverName, PortraitTurnInText, PortraitTurnInName, QuestCompletionLog"
        " FROM quest_template_locale");
    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();

        uint32 id                       = fields[0].GetUInt32();
        std::string localeName          = fields[1].GetString();

        LocaleConstant locale = GetLocaleByName(localeName);
        if (!sWorld->getBoolConfig(CONFIG_LOAD_LOCALES) || locale == LOCALE_enUS)
            continue;

        QuestTemplateLocale& data = _questTemplateLocaleStore[id];
        AddLocaleString(fields[2].GetString(), locale, data.LogTitle);
        AddLocaleString(fields[3].GetString(), locale, data.LogDescription);
        AddLocaleString(fields[4].GetString(), locale, data.QuestDescription);
        AddLocaleString(fields[5].GetString(), locale, data.AreaDescription);
        AddLocaleString(fields[6].GetString(), locale, data.PortraitGiverText);
        AddLocaleString(fields[7].GetString(), locale, data.PortraitGiverName);
        AddLocaleString(fields[8].GetString(), locale, data.PortraitTurnInText);
        AddLocaleString(fields[9].GetString(), locale, data.PortraitTurnInName);
        AddLocaleString(fields[10].GetString(), locale, data.QuestCompletionLog);
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %lu Quest Template locale strings in %u ms", _questTemplateLocaleStore.size(), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadQuestObjectivesLocale()
{
    uint32 oldMSTime = getMSTime();

    _questObjectivesLocaleStore.clear(); // need for reload case
    //                                               0     1          2
    QueryResult result = WorldDatabase.Query("SELECT Id, locale, Description FROM quest_objectives_locale");
    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();

        uint32 id                           = fields[0].GetUInt32();
        std::string localeName              = fields[1].GetString();

        LocaleConstant locale = GetLocaleByName(localeName);
        if (!sWorld->getBoolConfig(CONFIG_LOAD_LOCALES) || locale == LOCALE_enUS)
            continue;

        QuestObjectivesLocale& data = _questObjectivesLocaleStore[id];
        AddLocaleString(fields[2].GetString(), locale, data.Description);
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %lu Quest Objectives locale strings in %u ms", _questObjectivesLocaleStore.size(), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadQuestOfferRewardLocale()
{
    uint32 oldMSTime = getMSTime();

    _questOfferRewardLocaleStore.clear(); // need for reload case
    //                                               0     1          2
    QueryResult result = WorldDatabase.Query("SELECT Id, locale, RewardText FROM quest_offer_reward_locale");
    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();

        uint32 id = fields[0].GetUInt32();
        std::string localeName = fields[1].GetString();

        LocaleConstant locale = GetLocaleByName(localeName);
        if (!sWorld->getBoolConfig(CONFIG_LOAD_LOCALES) || locale == LOCALE_enUS)
            continue;

        QuestOfferRewardLocale& data = _questOfferRewardLocaleStore[id];
        AddLocaleString(fields[2].GetString(), locale, data.RewardText);
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %lu Quest Offer Reward locale strings in %u ms", _questOfferRewardLocaleStore.size(), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadQuestRequestItemsLocale()
{
    uint32 oldMSTime = getMSTime();

    _questRequestItemsLocaleStore.clear(); // need for reload case
    //                                               0     1          2
    QueryResult result = WorldDatabase.Query("SELECT Id, locale, CompletionText FROM quest_request_items_locale");
    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();

        uint32 id = fields[0].GetUInt32();
        std::string localeName = fields[1].GetString();

        LocaleConstant locale = GetLocaleByName(localeName);
        if (!sWorld->getBoolConfig(CONFIG_LOAD_LOCALES) || locale == LOCALE_enUS)
            continue;

        QuestRequestItemsLocale& data = _questRequestItemsLocaleStore[id];
        AddLocaleString(fields[2].GetString(), locale, data.CompletionText);
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %lu Quest Request Items locale strings in %u ms", _questRequestItemsLocaleStore.size(), GetMSTimeDiffToNow(oldMSTime));
}

CreatureDifficultyInfo const* ObjectMgr::GetCreatureDifficultyInfo(Difficulty difficulty, uint32 id) const
{
    if (difficulty >= MAX_CREATURE_DIFFICULTY)
        return nullptr;

    auto it = _creatureDifficulty[difficulty].find(id);
    if (it == _creatureDifficulty[difficulty].end())
        return nullptr;
    return &it->second;
}

CreatureDifficultyInfo const* ObjectMgr::SelectDifficultyInfo(Map const* map, uint32 entry) const
{
    Difficulty difficulty = map->GetDifficulty();
    if (map->IsBattleground())
        if (auto newBracket = GetBattlegroundBracketByLevel(map->GetId(), ((BattlegroundMap*)map)->GetBG()->GetMinLevel()))
            difficulty = Difficulty(newBracket->bracketId + MAX_DIFFICULTY);

    auto difficultyInfo = GetCreatureDifficultyInfo(difficulty, entry);
    if (difficultyInfo)
        return difficultyInfo;

    Difficulty alt = difficulty;
    switch (difficulty)
    {
        case RAID_DIFFICULTY_10MAN_HEROIC:
        case RAID_DIFFICULTY_1025MAN_FLEX:
            alt = RAID_DIFFICULTY_10MAN_NORMAL;
            break;
        case RAID_DIFFICULTY_25MAN_HEROIC:
        case RAID_DIFFICULTY_25MAN_LFR:
            alt = RAID_DIFFICULTY_25MAN_NORMAL;
            break;
        case DUNGEON_DIFFICULTY_CHALLENGE:
            alt = DUNGEON_DIFFICULTY_HEROIC;
            break;
    }

    if (alt != difficulty)
        difficultyInfo = GetCreatureDifficultyInfo(alt, entry);
    if (!difficultyInfo && difficulty != REGULAR_DIFFICULTY)
    {
        //TC_LOG_ERROR("sql.sql", "No difficulty entry for creature %u and map difficulty %u", entry, difficulty);
        return nullptr;
    }
    return difficultyInfo;
}

ItemSpecInfo const* ObjectMgr::GetItemSpecInfo(uint32 id, uint32 level)
{
    ItemTemplate const* proto = GetItemTemplate(id);
    if (!proto)
        return nullptr;

    // Client do it vice versa. But I think on a server side we even don't need the lowlevel case.
    if (level < 41)
        id = id | 0x40000000;

    //
    {
        std::unique_lock<std::mutex> lock{ _itemSpecCacheLock };
        auto it = _itemsSpecCache.find(id);
        if (it != _itemsSpecCache.end())
            return it->second.get();
    }

    std::unique_ptr<ItemSpecInfo> info{ new ItemSpecInfo{proto, level } };
    ItemSpecInfo const* ret = info.get();
    std::unique_lock<std::mutex> lock{ _itemSpecCacheLock };
    _itemsSpecCache.emplace(id, std::move(info)); // Move constructor
    return ret;
}

void ObjectMgr::LoadRealmCompletedChallenges()
{
    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    //                                                    0       1            2               3                4               5            6              7            8             9
    QueryResult result = CharacterDatabase.Query("SELECT map, attemptId, completion_time, completion_date, medal_earned, group_members, member_1_guid, member_1_spec, member_2_guid, member_2_spec, "
    //                                                   10           11             12               13            14             15
                                                 "member_3_guid, member_3_spec, member_4_guid, member_4_spec, member_5_guid, member_5_spec FROM group_completed_challenges");
    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 group completed challenges. DB table `group_completed_challenges` is empty.");
        return;
    }

    do
    {
        Field* fields = result->Fetch();
        uint32 mapId = fields[0].GetUInt32();

        RealmCompletedChallenge& realmChallenge = m_GroupsCompletedChallenges[mapId];

        realmChallenge.GuildId = 0;
        realmChallenge.AttemptId = fields[1].GetUInt32();
        realmChallenge.CompletionTime = fields[2].GetUInt32();
        realmChallenge.CompletionDate = fields[3].GetUInt32();
        realmChallenge.MedalEarned = fields[4].GetUInt8();
        realmChallenge.MembersCount = fields[5].GetUInt8();

        for (uint8 i = 0; i < 5; i++)
        {
            realmChallenge.Members[i].Guid = ObjectGuid(HighGuid::Player, fields[6 + i + i].GetUInt32());
            realmChallenge.Members[i].SpecId = fields[7 + i + i].GetUInt32();
        }

        count++;
    }
    while (result->NextRow());

    //                                        0     1          2           3                  4               5               6             7            8                9            10
    result = CharacterDatabase.Query("SELECT map, guildId, attemptId, completion_time, completion_date, medal_earned, guild_members, member_1_guid, member_1_spec, member_2_guid, member_2_spec, "
    //                                        11          12              13             14           15             16
                                     "member_3_guid, member_3_spec, member_4_guid, member_4_spec, member_5_guid, member_5_spec FROM guild_completed_challenges");
    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 guild completed challenges. DB table `guild_completed_challenges` is empty.");
        return;
    }

    do
    {
        Field* fields = result->Fetch();
        uint32 mapId = fields[0].GetUInt32();

        RealmCompletedChallenge& realmChallenge = m_GuildsCompletedChallenges[mapId];

        realmChallenge.GuildId = fields[1].GetUInt32();
        realmChallenge.AttemptId = fields[2].GetUInt32();
        realmChallenge.CompletionTime = fields[3].GetUInt32();
        realmChallenge.CompletionDate = fields[4].GetUInt32();
        realmChallenge.MedalEarned = fields[5].GetUInt8();
        realmChallenge.MembersCount = fields[6].GetUInt8();

        for (uint8 i = 0; i < 5; i++)
        {
            realmChallenge.Members[i].Guid = ObjectGuid(HighGuid::Player, fields[7 + i + i].GetUInt32());
            realmChallenge.Members[i].SpecId = fields[8 + i + i].GetUInt32();
        }

        count++;
    }
    while (result->NextRow());

    TC_LOG_ERROR("server.loading", ">> Loaded %u realm completed challenges in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadChallengeRewards()
{
    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    //                                                0        1            2             3             4          5            6              7           8         9          10
    QueryResult result = WorldDatabase.Query("SELECT map, none_money, bronze_money, silver_money, gold_money, none_valor, bronze_valor, silver_valor, gold_valor, title, platinum_achievement FROM challenge_mode_rewards");
    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 challenge rewards. DB table `challenge_mode_rewards` is empty.");
        return;
    }

    do
    {
        uint32 index = 0;
        Field* fields = result->Fetch();
        uint32 mapId = fields[0].GetUInt32();

        ChallengeReward& rewards = m_ChallengeRewardsMap[mapId];

        rewards.MapId = mapId;

        for (uint8 i = 0; i < 4; i++)
            rewards.MoneyReward[i] = fields[1 + i].GetUInt32();

        for (uint8 i = 0; i < 4; i++)
            rewards.ValorReward[i] = fields[5 + i].GetUInt32();

        rewards.TitleID       = fields[9].GetUInt32();
        rewards.AchievementID = fields[10].GetUInt32();

        count++;
    }
    while (result->NextRow());

    TC_LOG_ERROR("server.loading", ">> Loaded %u challenge mode rewards in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::AddGroupCompletedChallenge(uint32 mapId, RealmCompletedChallenge challengers)
{
    RealmCompletedChallenge& realmChallenge = m_GroupsCompletedChallenges[mapId];
    realmChallenge = challengers;
}

void ObjectMgr::AddGuildCompletedChallenge(uint32 mapId, RealmCompletedChallenge challengers)
{
    RealmCompletedChallenge& guildChallenge = m_GuildsCompletedChallenges[mapId];
    guildChallenge = challengers;
}

void ObjectMgr::LoadBrokenQuests()
{
    uint32 oldMSTime = getMSTime();
    _brokenQuests.clear();

    //                                                 0
    QueryResult result = WorldDatabase.Query("SELECT questId FROM broken_quests");
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 broken quests. DB table `broken_quests` is empty.");
        return;
    }

    do
    {
        Field* fields = result->Fetch();
        uint32 entry = fields[0].GetUInt32();

        Quest const* quest = GetQuestTemplate(entry);
        if (!quest)
        {
            TC_LOG_ERROR("sql.sql", "ObjectMgr::LoadBrokenQuests, not existed quest entry (%u), skipped", entry);
            continue;
        }

        if (quest->IsDailyOrWeekly() || quest->IsDFQuest() || quest->IsRaidQuest(Difficulty(REGULAR_DIFFICULTY)))
        {
            TC_LOG_ERROR("sql.sql", "ObjectMgr::LoadBrokenQuests trying to load unwished quest %u, skipped", entry);
            continue;
        }

        _brokenQuests.insert(entry);
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u broken quests in %u ms.", uint32(_brokenQuests.size()), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadCustomVisibility()
{
    m_customVisibilityInfo.clear();

    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    if (QueryResult result = WorldDatabase.PQuery("SELECT type+0, entry, distance, active, importance+0, comment FROM object_visibility"))
    {
        do
        {
            Field* fields = result->Fetch();
            CustomVisibility::Type type = (CustomVisibility::Type)fields[0].GetUInt32();
            uint32 entry = fields[1].GetUInt32();

            CustomVisibility::Info& info = m_customVisibilityInfo[type][entry];
            info.Type = type;
            info.Entry = entry;
            info.Distance = fields[2].GetFloat();
            info.Active = fields[3].GetBool();
            info.Importance = (CustomVisibility::Importance)fields[4].GetUInt32();
            info.Comment = fields[5].GetString();

            ++count;
        }
        while (result->NextRow());
    }

    TC_LOG_INFO("server.loading", ">> Loaded %u custom object visibility settings in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

bool ObjectMgr::LoadCustomVisibility(CustomVisibility::Type type, uint32 entry)
{
    if (QueryResult result = WorldDatabase.PQuery("SELECT distance, active, importance+0, comment FROM object_visibility WHERE type = %u AND entry = %u", (uint32)type, entry))
    {
        Field* fields = result->Fetch();

        CustomVisibility::Info& info = m_customVisibilityInfo[type][entry];
        info.Type = type;
        info.Entry = entry;
        info.Distance = fields[0].GetFloat();
        info.Active = fields[1].GetBool();
        info.Importance = (CustomVisibility::Importance)fields[2].GetUInt32();
        info.Comment = fields[3].GetString();
        return true;
    }
    else
    {
        auto itr = m_customVisibilityInfo[type].find(entry);
        if (itr == m_customVisibilityInfo[type].end())
            return false;

        m_customVisibilityInfo[type].erase(itr);
        return true;
    }
}

bool ObjectMgr::HasStats(ItemTemplate const* proto) const
{
    if (proto->RandomProperty != 0 || proto->RandomSuffix != 0)
        return true;

    for (uint8 i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
        if (proto->ItemStat[i].ItemStatValue != 0)
            return true;

    return false;
}

bool ObjectMgr::CanBeTransmogrified(ItemTemplate const* proto) const
{
    if (!proto)
        return false;

    if (proto->Quality == ITEM_QUALITY_LEGENDARY)
        return false;

    if (proto->Class != ITEM_CLASS_ARMOR &&
        proto->Class != ITEM_CLASS_WEAPON)
        return false;

    if (proto->Class == ITEM_CLASS_WEAPON && proto->SubClass == ITEM_SUBCLASS_WEAPON_FISHING_POLE)
        return false;

    if (proto->Flags2 & ITEM_FLAGS_EXTRA_CANNOT_BE_TRANSMOG && !(proto->Flags2 & ITEM_FLAGS_EXTRA_CAN_TRANSMOG))
        return false;

    // Client definitely use this to show variants, idk about terminology and naming of this shit
    if (proto->Flags2 & ITEM_FLAGS_EXTRA_CAN_TRANSMOG)
        return true;

    if (!HasStats(proto) && proto->Quality < ITEM_QUALITY_RARE)
        return false;

    return true;
}

bool ObjectMgr::CanTransmogrify(ItemTemplate const* proto) const
{
    if (!proto)
        return false;

    if (proto->Flags2 & ITEM_FLAGS_EXTRA_CANNOT_TRANSMOG)
        return false;

    if (proto->Quality == ITEM_QUALITY_LEGENDARY)
        return false;

    if (proto->Class != ITEM_CLASS_ARMOR &&
        proto->Class != ITEM_CLASS_WEAPON)
        return false;

    if (proto->Class == ITEM_CLASS_WEAPON && proto->SubClass == ITEM_SUBCLASS_WEAPON_FISHING_POLE)
        return false;

    if (proto->Flags2 & ITEM_FLAGS_EXTRA_CAN_TRANSMOG)
        return true;

    if (!HasStats(proto))
        return false;

    return true;
}

bool ObjectMgr::SubClassesCompatible(ItemTemplate const* transmogrified, ItemTemplate const* transmogrifier) const
{
    if (!transmogrified || !transmogrifier)
        return false;

    // Patch 5.2 - Throne of Thunder
    // One-Handed
    // One-handed axes, maces, and swords can be transmogrified to each other
    if ((transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_AXE ||
         transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_MACE ||
         transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_SWORD) &&
        (transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_AXE ||
         transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_MACE ||
         transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_SWORD))
        return true;

    // Two-Handed
    // Two-handed axes, maces, and swords can be transmogrified to each other
    if ((transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_AXE2 ||
         transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_MACE2 ||
         transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_SWORD2) &&
        (transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_AXE2 ||
         transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_MACE2 ||
         transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_SWORD2))
        return true;

    // Ranged
    // Bows, guns and crosbows can be transmogrified to each other
    if ((transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_BOW ||
         transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_GUN ||
         transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_CROSSBOW) &&
        (transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_BOW ||
         transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_GUN ||
         transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_CROSSBOW))
        return true;

    // Polearm and Staff
    // Staves and polearms can be transmogrified to each other
    if ((transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_POLEARM ||
         transmogrified->SubClass == ITEM_SUBCLASS_WEAPON_STAFF) &&
        (transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_POLEARM ||
         transmogrifier->SubClass == ITEM_SUBCLASS_WEAPON_STAFF))
        return true;

    return false;
}

bool ObjectMgr::CanTransmogrifyItemWithItem(ItemTemplate const* transmogrified, ItemTemplate const* transmogrifier)
{
    if (!transmogrifier || !transmogrified)
        return false;

    if (transmogrifier->ItemId == transmogrified->ItemId)
        return false;

    if (!CanTransmogrify(transmogrified) || !CanBeTransmogrified(transmogrifier))
        return false;

    if (transmogrifier->InventoryType == INVTYPE_BAG ||
        transmogrifier->InventoryType == INVTYPE_RELIC ||
        transmogrifier->InventoryType == INVTYPE_BODY ||
        transmogrifier->InventoryType == INVTYPE_FINGER ||
        transmogrifier->InventoryType == INVTYPE_TRINKET ||
        transmogrifier->InventoryType == INVTYPE_AMMO ||
        transmogrifier->InventoryType == INVTYPE_QUIVER)
        return false;

    if (transmogrifier->SubClass != ITEM_SUBCLASS_ARMOR_COSMETIC && (transmogrifier->Class != ITEM_CLASS_WEAPON || !transmogrified->IsRangedWeapon() || !transmogrifier->IsRangedWeapon()) &&
        (transmogrifier->SubClass != transmogrified->SubClass && !SubClassesCompatible(transmogrifier, transmogrified)))
        return false;

    if (transmogrifier->IsRangedWeapon() && transmogrified->IsRangedWeapon())
        return true;

    if (transmogrifier->InventoryType != transmogrified->InventoryType &&
        (transmogrifier->Class != ITEM_CLASS_WEAPON || (transmogrified->InventoryType != INVTYPE_WEAPONMAINHAND && transmogrified->InventoryType != INVTYPE_WEAPONOFFHAND)) &&
        (transmogrifier->Class != ITEM_CLASS_ARMOR || (transmogrifier->InventoryType != INVTYPE_CHEST && transmogrified->InventoryType != INVTYPE_ROBE && transmogrifier->InventoryType != INVTYPE_ROBE && transmogrified->InventoryType != INVTYPE_CHEST)))
        return false;

    return true;
}

void ObjectMgr::LoadCreatureScaling()
{
    uint32 oldMSTime = getMSTime();

    _creatureScalingStore.clear();                            // need for reload case

    QueryResult result = WorldDatabase.Query("SELECT entry, group_size, health, damage_mod FROM creature_scaling");
    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 creature scaling data. DB table `creature_scaling` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();
        uint32 groupSize = fields[1].GetUInt32();

        CreatureScalingInfo& info = _creatureScalingStore[MAKE_PAIR32(entry, groupSize)];
        info.Health = fields[2].GetFloat();
        info.DamageMod = fields[3].GetFloat();

        count++;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u creature scaling data in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadPromotionAuras()
{
    _promotionAuras.clear();

    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    if (QueryResult result = LoginDatabase.PQuery("SELECT entry, start_date, lenght, active FROM promotion_auras"))
    {
        _promotionAuras.reserve(result->GetRowCount());

        do
        {
            Field* fields = result->Fetch();

            PromotionAurasInfo info;
            info.Entry = fields[0].GetUInt32();
            info.StartDate = fields[1].GetUInt32();
            info.Lenght = fields[2].GetUInt32() * MINUTE; // in minutes
            info.Active = fields[3].GetBool();

            // skip outdated promotions
            if (time(nullptr) > (info.StartDate + info.Lenght))
                info.Active = 0;

            _promotionAuras.push_back(info);

            ++count;
        } while (result->NextRow());
    }

    TC_LOG_INFO("server.loading", ">> Loaded %u promotion auras in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadObjectVisibilityState()
{
    uint32 oldMSTime = getMSTime();

    _objectVisibilityStateStore.clear(); // need for reload case

    QueryResult result = WorldDatabase.Query("SELECT entryorguid, type+0, visibilityQuestID, visibilityQuestState FROM object_visibility_state");
    if (!result)
    {
        TC_LOG_ERROR("server.loading", ">> Loaded 0 object visibility state data. DB table `object_visibility_state` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        int32 entryorguid = fields[0].GetInt32();

        ObjectVisibilityState::ObjectVisibilityStateInfo& info = _objectVisibilityStateStore[entryorguid];
        info.type = (ObjectVisibilityState::objectType)fields[1].GetUInt32();
        info.questId = fields[2].GetUInt32();
        info.questState = fields[3].GetUInt32();

        count++;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u object visibility state data in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}
