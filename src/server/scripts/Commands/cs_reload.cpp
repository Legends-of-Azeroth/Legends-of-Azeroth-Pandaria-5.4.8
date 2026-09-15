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

/* ScriptData
Name: reload_commandscript
%Complete: 100
Comment: All reload related commands
Category: commandscripts
EndScriptData */

#include "AccountMgr.h"
#include "AchievementMgr.h"
#include "AuctionHouseMgr.h"
#include "BattlegroundMgr.h"
#include "BattlePayMgr.h"
#include "Chat.h"
#include "CreatureTextMgr.h"
#include "DisableMgr.h"
#include "Language.h"
#include "LFGMgr.h"
#include "MapManager.h"
#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ServiceBoost.h"
#include "SkillDiscovery.h"
#include "SkillExtraItems.h"
#include "SmartAI.h"
#include "SpellMgr.h"
#include "TicketMgr.h"
#include "WardenCheckMgr.h"
#include "WaypointManager.h"
#include "WordFilterMgr.h"
#ifdef ELUNA
#include "LuaEngine.h"
#include "HookMgr.h"
extern void StartEluna(bool restart);
#endif

class reload_commandscript : public CommandScript
{
public:
    reload_commandscript() : CommandScript("reload_commandscript") { }

    std::vector<ChatCommand> GetCommands() const
    {
        static std::vector<ChatCommand> reloadAllCommandTable =
        {
            { "achievement",    &HandleReloadAllAchievementCommand, rbac::RBAC_PERM_COMMAND_RELOAD_ALL_ACHIEVEMENT, Trinity::ChatCommands::Console::Yes },
            { "area",           &HandleReloadAllAreaCommand,        rbac::RBAC_PERM_COMMAND_RELOAD_ALL_AREA,        Trinity::ChatCommands::Console::Yes },
            { "gossips", &HandleReloadAllGossipsCommand, rbac::RBAC_PERM_COMMAND_RELOAD_ALL_GOSSIPS, Trinity::ChatCommands::Console::Yes },
            { "item",           &HandleReloadAllItemCommand,        rbac::RBAC_PERM_COMMAND_RELOAD_ALL_ITEM,        Trinity::ChatCommands::Console::Yes },
            { "locales",        &HandleReloadAllLocalesCommand,     rbac::RBAC_PERM_COMMAND_RELOAD_ALL_LOCALES,     Trinity::ChatCommands::Console::Yes },
            { "loot",           &HandleReloadAllLootCommand,        rbac::RBAC_PERM_COMMAND_RELOAD_ALL_LOOT,        Trinity::ChatCommands::Console::Yes },
            { "npc",            &HandleReloadAllNpcCommand,         rbac::RBAC_PERM_COMMAND_RELOAD_ALL_NPC,         Trinity::ChatCommands::Console::Yes },
            { "quest",          &HandleReloadAllQuestCommand,       rbac::RBAC_PERM_COMMAND_RELOAD_ALL_QUEST,       Trinity::ChatCommands::Console::Yes },
            { "scripts",        &HandleReloadAllScriptsCommand,     rbac::RBAC_PERM_COMMAND_RELOAD_ALL_SCRIPTS,     Trinity::ChatCommands::Console::Yes },
            { "spell",          &HandleReloadAllSpellCommand,       rbac::RBAC_PERM_COMMAND_RELOAD_ALL_SPELL,       Trinity::ChatCommands::Console::Yes },
            { "",               &HandleReloadAllCommand,            rbac::RBAC_PERM_COMMAND_RELOAD_ALL,             Trinity::ChatCommands::Console::Yes },
        };
        static std::vector<ChatCommand> reloadCommandTable =
        {
            { "auctions",                       &HandleReloadAuctionsCommand,                   rbac::RBAC_PERM_COMMAND_RELOAD_AUCTIONS,                       Trinity::ChatCommands::Console::Yes },
            { "access_requirement",             &HandleReloadAccessRequirementCommand,          rbac::RBAC_PERM_COMMAND_RELOAD_ACCESS_REQUIREMENT,             Trinity::ChatCommands::Console::Yes },
            { "achievement_criteria_data",      &HandleReloadAchievementCriteriaDataCommand,    rbac::RBAC_PERM_COMMAND_RELOAD_ACHIEVEMENT_CRITERIA_DATA,      Trinity::ChatCommands::Console::Yes },
            { "achievement_reward",             &HandleReloadAchievementRewardCommand,          rbac::RBAC_PERM_COMMAND_RELOAD_ACHIEVEMENT_REWARD,             Trinity::ChatCommands::Console::Yes },
            { "all",                            reloadAllCommandTable,                           rbac::RBAC_PERM_COMMAND_RELOAD_ALL,                            Trinity::ChatCommands::Console::Yes },
            { "areatrigger_queststarter", &HandleReloadQuestGiverAreaTriggersCommand, rbac::RBAC_PERM_COMMAND_RELOAD_AREATRIGGER_QUESTSTARTER, Trinity::ChatCommands::Console::Yes },
            { "areatrigger_involvedrelation",   &HandleReloadQuestAreaTriggersCommand,          rbac::RBAC_PERM_COMMAND_RELOAD_AREATRIGGER_INVOLVEDRELATION,    Trinity::ChatCommands::Console::Yes },
            { "areatrigger_tavern",             &HandleReloadAreaTriggerTavernCommand,          rbac::RBAC_PERM_COMMAND_RELOAD_AREATRIGGER_TAVERN,             Trinity::ChatCommands::Console::Yes },
            { "areatrigger_teleport",           &HandleReloadAreaTriggerTeleportCommand,        rbac::RBAC_PERM_COMMAND_RELOAD_AREATRIGGER_TELEPORT,           Trinity::ChatCommands::Console::Yes },
            { "autobroadcast",                  &HandleReloadAutobroadcastCommand,              rbac::RBAC_PERM_COMMAND_RELOAD_AUTOBROADCAST,                  Trinity::ChatCommands::Console::Yes },
            { "battleground_template",          &HandleReloaBattlegroundTemplateCommand,        rbac::RBAC_PERM_COMMAND_RELOAD_BATTLEGROUND_TEMPLATE,          Trinity::ChatCommands::Console::Yes },
            { "broadcast_text",                 &HandleReloadBroadcastTextCommand,              rbac::RBAC_PERM_COMMAND_RELOAD_BROADCAST_TEXT,                 Trinity::ChatCommands::Console::Yes },
            { "command", &HandleReloadCommandCommand, rbac::RBAC_PERM_COMMAND_RELOAD_COMMAND, Trinity::ChatCommands::Console::Yes },
            { "conditions",                     &HandleReloadConditions,                        rbac::RBAC_PERM_COMMAND_RELOAD_CONDITIONS,                     Trinity::ChatCommands::Console::Yes },
            { "config",                         &HandleReloadConfigCommand,                     rbac::RBAC_PERM_COMMAND_RELOAD_CONFIG,                         Trinity::ChatCommands::Console::Yes },
            { "creature_text",                  &HandleReloadCreatureText,                      rbac::RBAC_PERM_COMMAND_RELOAD_CREATURE_TEXT,                  Trinity::ChatCommands::Console::Yes },
            { "creature_text_locale", &HandleReloadLocalesCreatureTextCommand, rbac::RBAC_PERM_COMMAND_RELOAD_CREATURE_TEXT_LOCALE, Trinity::ChatCommands::Console::Yes },
            { "creature_questender",            &HandleReloadCreatureQuestEnderCommand,         rbac::RBAC_PERM_COMMAND_RELOAD_CREATURE_QUESTENDER,            Trinity::ChatCommands::Console::Yes },
            { "creature_linked_respawn",        &HandleReloadLinkedRespawnCommand,              rbac::RBAC_PERM_COMMAND_RELOAD_CREATURE_LINKED_RESPAWN,        Trinity::ChatCommands::Console::Yes },
            { "creature_loot_template",         &HandleReloadLootTemplatesCreatureCommand,      rbac::RBAC_PERM_COMMAND_RELOAD_CREATURE_LOOT_TEMPLATE,         Trinity::ChatCommands::Console::Yes },
            { "creature_movement_override",     &HandleReloadCreatureMovementOverrideCommand,   rbac::RBAC_PERM_COMMAND_RELOAD_CREATURE_MOVEMENT_OVERRIDE,     Trinity::ChatCommands::Console::Yes },
            { "creature_onkill_reputation",     &HandleReloadOnKillReputationCommand,           rbac::RBAC_PERM_COMMAND_RELOAD_CREATURE_ONKILL_REPUTATION,     Trinity::ChatCommands::Console::Yes },
            { "creature_queststarter",          &HandleReloadCreatureQuestStarterCommand,       rbac::RBAC_PERM_COMMAND_RELOAD_CREATURE_QUESTSTARTER,          Trinity::ChatCommands::Console::Yes },
            { "creature_scaling", &HandleReloadCreatureScallingCommand, rbac::RBAC_PERM_COMMAND_RELOAD_CREATURE_SCALING, Trinity::ChatCommands::Console::Yes },
            { "creature_summon_groups",         &HandleReloadCreatureSummonGroupsCommand,       rbac::RBAC_PERM_COMMAND_RELOAD_CREATURE_SUMMON_GROUPS,         Trinity::ChatCommands::Console::Yes },
            { "creature_template",              &HandleReloadCreatureTemplateCommand,           rbac::RBAC_PERM_COMMAND_RELOAD_CREATURE_TEMPLATE,              Trinity::ChatCommands::Console::Yes },
            { "disables",                       &HandleReloadDisablesCommand,                   rbac::RBAC_PERM_COMMAND_RELOAD_DISABLES,                       Trinity::ChatCommands::Console::Yes },
            { "disenchant_loot_template",       &HandleReloadLootTemplatesDisenchantCommand,    rbac::RBAC_PERM_COMMAND_RELOAD_DISENCHANT_LOOT_TEMPLATE,       Trinity::ChatCommands::Console::Yes },
            { "event_scripts",                  &HandleReloadEventScriptsCommand,               rbac::RBAC_PERM_COMMAND_RELOAD_EVENT_SCRIPTS,                  Trinity::ChatCommands::Console::Yes },
            { "fishing_loot_template",          &HandleReloadLootTemplatesFishingCommand,       rbac::RBAC_PERM_COMMAND_RELOAD_FISHING_LOOT_TEMPLATE,          Trinity::ChatCommands::Console::Yes },
            { "game_graveyard_zone", &HandleReloadGameGraveyardZoneCommand, rbac::RBAC_PERM_COMMAND_RELOAD_GAME_GRAVEYARD_ZONE, Trinity::ChatCommands::Console::Yes },
            { "game_tele",                      &HandleReloadGameTeleCommand,                   rbac::RBAC_PERM_COMMAND_RELOAD_GAME_TELE,                      Trinity::ChatCommands::Console::Yes },
            { "gameobject_questender",          &HandleReloadGOQuestEnderCommand,               rbac::RBAC_PERM_COMMAND_RELOAD_GAMEOBJECT_QUESTENDER,          Trinity::ChatCommands::Console::Yes },
            { "gameobject_loot_template", &HandleReloadLootTemplatesGameobjectCommand, rbac::RBAC_PERM_COMMAND_RELOAD_GAMEOBJECT_LOOT_TEMPLATE, Trinity::ChatCommands::Console::Yes },
            { "gameobject_queststarter",        &HandleReloadGOQuestStarterCommand,             rbac::RBAC_PERM_COMMAND_RELOAD_GAMEOBJECT_QUESTSTARTER,        Trinity::ChatCommands::Console::Yes },
            { "gameobject_template_locale",     &HandleReloadLocalesGameobjectCommand,          rbac::RBAC_PERM_COMMAND_RELOAD_GAMEOBJECT_TEMPLATE_LOCALE,     Trinity::ChatCommands::Console::Yes },
            { "gm_tickets",                     &HandleReloadGMTicketsCommand,                  rbac::RBAC_PERM_COMMAND_RELOAD_GM_TICKETS,                     Trinity::ChatCommands::Console::Yes },
            { "gossip_menu",                    &HandleReloadGossipMenuCommand,                 rbac::RBAC_PERM_COMMAND_RELOAD_GOSSIP_MENU,                    Trinity::ChatCommands::Console::Yes },
            { "gossip_menu_option",             &HandleReloadGossipMenuOptionCommand,           rbac::RBAC_PERM_COMMAND_RELOAD_GOSSIP_MENU_OPTION,             Trinity::ChatCommands::Console::Yes },
            { "item_enchantment_template",      &HandleReloadItemEnchantementsCommand,          rbac::RBAC_PERM_COMMAND_RELOAD_ITEM_ENCHANTMENT_TEMPLATE,      Trinity::ChatCommands::Console::Yes },
            { "item_loot_template",             &HandleReloadLootTemplatesItemCommand,          rbac::RBAC_PERM_COMMAND_RELOAD_ITEM_LOOT_TEMPLATE,             Trinity::ChatCommands::Console::Yes },
            { "item_template_locale",           &HandleReloadLocalesItemCommand,                rbac::RBAC_PERM_COMMAND_RELOAD_ITEM_TEMPLATE_LOCALE,             Trinity::ChatCommands::Console::Yes },
            { "lfg_dungeon_rewards",            &HandleReloadLfgRewardsCommand,                 rbac::RBAC_PERM_COMMAND_RELOAD_LFG_DUNGEON_REWARDS,          Trinity::ChatCommands::Console::Yes },
            { "achievement_reward_locale",      &HandleReloadLocalesAchievementRewardCommand,   rbac::RBAC_PERM_COMMAND_RELOAD_ACHIEVEMENT_REWARD_LOCALE,      Trinity::ChatCommands::Console::Yes },
            { "creature_template_locale", &HandleReloadLocalesCreatureCommand, rbac::RBAC_PERM_COMMAND_RELOAD_CREATURE_TEMPLATE_LOCALE, Trinity::ChatCommands::Console::Yes },
            { "gossip_menu_option_locale",      &HandleReloadLocalesGossipMenuOptionCommand,    rbac::RBAC_PERM_COMMAND_RELOAD_GOSSIP_MENU_OPTION_LOCALE,      Trinity::ChatCommands::Console::Yes },
            { "quest_template_locale",          &HandleReloadLocalesQuestCommand,               rbac::RBAC_PERM_COMMAND_RELOAD_QUEST_TEMPLATE_LOCALE,          Trinity::ChatCommands::Console::Yes },
            { "mail_level_reward",              &HandleReloadMailLevelRewardCommand,            rbac::RBAC_PERM_COMMAND_RELOAD_MAIL_LEVEL_REWARD,            Trinity::ChatCommands::Console::Yes },
            { "mail_loot_template",             &HandleReloadLootTemplatesMailCommand,          rbac::RBAC_PERM_COMMAND_RELOAD_MAIL_LOOT_TEMPLATE,             Trinity::ChatCommands::Console::Yes },
            { "milling_loot_template",          &HandleReloadLootTemplatesMillingCommand,       rbac::RBAC_PERM_COMMAND_RELOAD_MILLING_LOOT_TEMPLATE,         Trinity::ChatCommands::Console::Yes },
            { "npc_spellclick_spells",          &HandleReloadSpellClickSpellsCommand,           rbac::RBAC_PERM_COMMAND_RELOAD_NPC_SPELLCLICK_SPELLS,          Trinity::ChatCommands::Console::Yes },
            { "npc_text_locale",                &HandleReloadLocalesNpcTextCommand,             rbac::RBAC_PERM_COMMAND_RELOAD_NPC_TEXT_LOCALE,                Trinity::ChatCommands::Console::Yes },
            { "npc_trainer", &HandleReloadNpcTrainerCommand, rbac::RBAC_PERM_COMMAND_RELOAD_NPC_TRAINER, Trinity::ChatCommands::Console::Yes },
            { "npc_vendor",                     &HandleReloadNpcVendorCommand,                  rbac::RBAC_PERM_COMMAND_RELOAD_NPC_VENDOR,                     Trinity::ChatCommands::Console::Yes },
            { "page_text",                      &HandleReloadPageTextsCommand,                  rbac::RBAC_PERM_COMMAND_RELOAD_PAGE_TEXT,                      Trinity::ChatCommands::Console::Yes },
            { "page_text_locale",               &HandleReloadLocalesPageTextCommand,            rbac::RBAC_PERM_COMMAND_RELOAD_PAGE_TEXT_LOCALE,               Trinity::ChatCommands::Console::Yes },
            { "pickpocketing_loot_template",    &HandleReloadLootTemplatesPickpocketingCommand, rbac::RBAC_PERM_COMMAND_RELOAD_PICKPOCKETING_LOOT_TEMPLATE,   Trinity::ChatCommands::Console::Yes },
            { "points_of_interest",             &HandleReloadPointsOfInterestCommand,           rbac::RBAC_PERM_COMMAND_RELOAD_POINTS_OF_INTEREST,           Trinity::ChatCommands::Console::Yes },
            { "points_of_interest_locale",      &HandleReloadLocalesPointsOfInterestCommand,    rbac::RBAC_PERM_COMMAND_RELOAD_POINTS_OF_INTEREST_LOCALE,     Trinity::ChatCommands::Console::Yes },
            { "promotion_auras", &HandleReloadPromotionAurasCommand, rbac::RBAC_PERM_COMMAND_RELOAD_PROMOTION_AURAS, Trinity::ChatCommands::Console::Yes },
            { "prospecting_loot_template",      &HandleReloadLootTemplatesProspectingCommand,   rbac::RBAC_PERM_COMMAND_RELOAD_PROSPECTING_LOOT_TEMPLATE,     Trinity::ChatCommands::Console::Yes },
            { "quest_poi",                      &HandleReloadQuestPOICommand,                   rbac::RBAC_PERM_COMMAND_RELOAD_QUEST_POI,                      Trinity::ChatCommands::Console::Yes },
            { "quest_template",                 &HandleReloadQuestTemplateCommand,              rbac::RBAC_PERM_COMMAND_RELOAD_QUEST_TEMPLATE,                 Trinity::ChatCommands::Console::Yes },
            { "reference_loot_template",        &HandleReloadLootTemplatesReferenceCommand,     rbac::RBAC_PERM_COMMAND_RELOAD_REFERENCE_LOOT_TEMPLATE,        Trinity::ChatCommands::Console::Yes },
            { "reserved_name",                  &HandleReloadReservedNameCommand,               rbac::RBAC_PERM_COMMAND_RELOAD_RESERVED_NAME,                  Trinity::ChatCommands::Console::Yes },
            { "reputation_reward_rate",         &HandleReloadReputationRewardRateCommand,       rbac::RBAC_PERM_COMMAND_RELOAD_REPUTATION_REWARD_RATE,       Trinity::ChatCommands::Console::Yes },
            { "reputation_spillover_template", &HandleReloadReputationRewardRateCommand, rbac::RBAC_PERM_COMMAND_RELOAD_REPUTATION_SPILLOVER_TEMPLATE, Trinity::ChatCommands::Console::Yes },
            { "rbac",                           &HandleReloadRBACCommand,                       rbac::RBAC_PERM_COMMAND_RELOAD_RBAC,                           Trinity::ChatCommands::Console::Yes },
            { "skill_discovery_template",       &HandleReloadSkillDiscoveryTemplateCommand,     rbac::RBAC_PERM_COMMAND_RELOAD_SKILL_DISCOVERY_TEMPLATE,     Trinity::ChatCommands::Console::Yes },
            { "skill_extra_item_template",      &HandleReloadSkillExtraItemTemplateCommand,     rbac::RBAC_PERM_COMMAND_RELOAD_SKILL_EXTRA_ITEM_TEMPLATE,     Trinity::ChatCommands::Console::Yes },
            { "skill_fishing_base_level",       &HandleReloadSkillFishingBaseLevelCommand,      rbac::RBAC_PERM_COMMAND_RELOAD_SKILL_FISHING_BASE_LEVEL,      Trinity::ChatCommands::Console::Yes },
            { "skinning_loot_template",         &HandleReloadLootTemplatesSkinningCommand,      rbac::RBAC_PERM_COMMAND_RELOAD_SKINNING_LOOT_TEMPLATE,        Trinity::ChatCommands::Console::Yes },
            { "smart_scripts",                  &HandleReloadSmartScripts,                      rbac::RBAC_PERM_COMMAND_RELOAD_SMART_SCRIPTS,                  Trinity::ChatCommands::Console::Yes },
            { "spell_required",                 &HandleReloadSpellRequiredCommand,              rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_REQUIRED,                 Trinity::ChatCommands::Console::Yes },
            { "spell_area",                     &HandleReloadSpellAreaCommand,                  rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_AREA,                     Trinity::ChatCommands::Console::Yes },
            { "spell_bonus_data",               &HandleReloadSpellBonusesCommand,               rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_BONUS_DATA,               Trinity::ChatCommands::Console::Yes },
            { "spell_group",                    &HandleReloadSpellGroupsCommand,                rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_GROUP,                    Trinity::ChatCommands::Console::Yes },
            { "spell_learn_spell",              &HandleReloadSpellLearnSpellCommand,            rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_LEARN_SPELL,              Trinity::ChatCommands::Console::Yes },
            { "spell_loot_template",            &HandleReloadLootTemplatesSpellCommand,         rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_LOOT_TEMPLATE,            Trinity::ChatCommands::Console::Yes },
            { "spell_linked_spell",             &HandleReloadSpellLinkedSpellCommand,           rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_LINKED_SPELL,             Trinity::ChatCommands::Console::Yes },
            { "spell_pet_auras",                &HandleReloadSpellPetAurasCommand,              rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_PET_AURAS,                Trinity::ChatCommands::Console::Yes },
            { "spell_proc_event", &HandleReloadSpellProcEventCommand, rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_PROC_EVENT, Trinity::ChatCommands::Console::Yes },
            { "spell_proc",                     &HandleReloadSpellProcsCommand,                 rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_PROC,                     Trinity::ChatCommands::Console::Yes },
            { "spell_scripts", &HandleReloadSpellScriptsCommand, rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_SCRIPTS, Trinity::ChatCommands::Console::Yes },
            { "spell_target_position",          &HandleReloadSpellTargetPositionCommand,        rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_TARGET_POSITION,          Trinity::ChatCommands::Console::Yes },
            { "spell_threats",                  &HandleReloadSpellThreatsCommand,               rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_THREATS,                  Trinity::ChatCommands::Console::Yes },
            { "spell_group_stack_rules",        &HandleReloadSpellGroupStackRulesCommand,       rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_GROUP_STACK_RULES,        Trinity::ChatCommands::Console::Yes },
            { "trinity_string",                 &HandleReloadTrinityStringCommand,              rbac::RBAC_PERM_COMMAND_RELOAD_TRINITY_STRING,                 Trinity::ChatCommands::Console::Yes },
            { "waypoint_scripts",               &HandleReloadWpScriptsCommand,                  rbac::RBAC_PERM_COMMAND_RELOAD_WAYPOINT_SCRIPTS,               Trinity::ChatCommands::Console::Yes },
            { "waypoint_data",                  &HandleReloadWpCommand,                         rbac::RBAC_PERM_COMMAND_RELOAD_WAYPOINT_DATA,                  Trinity::ChatCommands::Console::Yes },
            { "vehicle_accessory", &HandleReloadVehicleAccessoryCommand, rbac::RBAC_PERM_COMMAND_RELOAD_VEHICLE_ACCESSORY, Trinity::ChatCommands::Console::Yes },
            { "vehicle_template_accessory",     &HandleReloadVehicleTemplateAccessoryCommand,   rbac::RBAC_PERM_COMMAND_RELOAD_VEHICLE_TEMPLATE_ACCESSORY,     Trinity::ChatCommands::Console::Yes },
            { "creature_difficulty", &HandleReloadCreatureDifficulty, rbac::RBAC_PERM_COMMAND_RELOAD_CREATURE_DIFFICULTY, Trinity::ChatCommands::Console::Yes },
            { "scene_template",                 &HandleReloadSceneTemplateCommand,              rbac::RBAC_PERM_COMMAND_RELOAD_SCENE_TEMPLATE,                 Trinity::ChatCommands::Console::Yes },
            { "creature_loot_currency", &HandleReloadCreatureLootCurrency, rbac::RBAC_PERM_COMMAND_RELOAD_CREATURE_LOOT_CURRENCY, Trinity::ChatCommands::Console::Yes },
            { "gocollision", &HandleReloadGOCollisionCommand, rbac::RBAC_PERM_COMMAND_RELOAD_GOCOLLISION, Trinity::ChatCommands::Console::Yes },
            { "battlepay", &HandleReloadBattlePayCommand, rbac::RBAC_PERM_COMMAND_RELOAD_BATTLEPAY, Trinity::ChatCommands::Console::Yes },
            { "bad_word", &HandleReloadBadWordCommand, rbac::RBAC_PERM_COMMAND_RELOAD_BAD_WORD, Trinity::ChatCommands::Console::Yes },
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "reload",                         reloadCommandTable,                              rbac::RBAC_PERM_COMMAND_RELOAD,                            Trinity::ChatCommands::Console::Yes },
        };
        return commandTable;
    }

    //reload commands
    static bool HandleReloadGMTicketsCommand(ChatHandler* /*handler*/, const char* /*args*/)
    {
        sTicketMgr->LoadGmTickets();
        return true;
    }

    static bool HandleReloadAllCommand(ChatHandler* handler, const char* /*args*/)
    {
        HandleReloadSkillFishingBaseLevelCommand(handler, "");

        HandleReloadAllAchievementCommand(handler, "");
        HandleReloadAllAreaCommand(handler, "");
        HandleReloadAllLootCommand(handler, "");
        HandleReloadAllNpcCommand(handler, "");
        HandleReloadAllQuestCommand(handler, "");
        HandleReloadAllSpellCommand(handler, "");
        HandleReloadAllItemCommand(handler, "");
        HandleReloadAllGossipsCommand(handler, "");
        HandleReloadAllLocalesCommand(handler, "");

        HandleReloadAccessRequirementCommand(handler, "");
        HandleReloadMailLevelRewardCommand(handler, "");
        HandleReloadCommandCommand(handler, "");
        HandleReloadReservedNameCommand(handler, "");
        HandleReloadTrinityStringCommand(handler, "");
        HandleReloadGameTeleCommand(handler, "");

        HandleReloadCreatureSummonGroupsCommand(handler, "");

        HandleReloadVehicleAccessoryCommand(handler, "");
        HandleReloadVehicleTemplateAccessoryCommand(handler, "");

        HandleReloadAutobroadcastCommand(handler, "");
        HandleReloadBadWordCommand(handler, "");
        return true;
    }

    static bool HandleReloadBadWordCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Bad Words...");
        sWordFilterMgr->LoadBadWords();
        handler->SendGlobalGMSysMessage("DB table `bad_word` reloaded.");
        return true;
    }

    static bool HandleReloadAllAchievementCommand(ChatHandler* handler, const char* /*args*/)
    {
        HandleReloadAchievementCriteriaDataCommand(handler, "");
        HandleReloadAchievementRewardCommand(handler, "");
        return true;
    }

    static bool HandleReloadAllAreaCommand(ChatHandler* handler, const char* /*args*/)
    {
        //HandleReloadQuestAreaTriggersCommand(handler, ""); -- reloaded in HandleReloadAllQuestCommand
        HandleReloadAreaTriggerTeleportCommand(handler, "");
        HandleReloadAreaTriggerTavernCommand(handler, "");
        HandleReloadGameGraveyardZoneCommand(handler, "");
        return true;
    }

    static bool HandleReloadAllLootCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Loot Tables...");
        sLootMgr->LoadFromDB();
        handler->SendGlobalGMSysMessage("DB tables `*_loot_template` reloaded.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadAllNpcCommand(ChatHandler* handler, const char* args)
    {
        if (*args != 'a')                                          // will be reloaded from all_gossips
        HandleReloadNpcTrainerCommand(handler, "a");
        HandleReloadNpcVendorCommand(handler, "a");
        HandleReloadPointsOfInterestCommand(handler, "a");
        HandleReloadSpellClickSpellsCommand(handler, "a");
        return true;
    }

    static bool HandleReloadAllQuestCommand(ChatHandler* handler, const char* /*args*/)
    {
        HandleReloadQuestAreaTriggersCommand(handler, "a");
        HandleReloadQuestPOICommand(handler, "a");
        HandleReloadQuestTemplateCommand(handler, "a");

        TC_LOG_INFO("misc", "Re-Loading Quests Relations...");
        sObjectMgr->LoadQuestStartersAndEnders();
        handler->SendGlobalGMSysMessage("DB tables `*_queststarter` and `*_questender` reloaded.");
        return true;
    }

    static bool HandleReloadAllScriptsCommand(ChatHandler* handler, const char* /*args*/)
    {
        if (sScriptMgr->IsScriptScheduled())
        {
            handler->PSendSysMessage("DB scripts used currently, please attempt reload later.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        TC_LOG_INFO("misc", "Re-Loading Scripts...");
        HandleReloadEventScriptsCommand(handler, "a");
        HandleReloadSpellScriptsCommand(handler, "a");
        handler->SendGlobalGMSysMessage("DB tables `*_scripts` reloaded.");
        HandleReloadWpScriptsCommand(handler, "a");
        HandleReloadWpCommand(handler, "a");
        return true;
    }

    static bool HandleReloadAllSpellCommand(ChatHandler* handler, const char* /*args*/)
    {
        HandleReloadSkillDiscoveryTemplateCommand(handler, "a");
        HandleReloadSkillExtraItemTemplateCommand(handler, "a");
        HandleReloadSpellRequiredCommand(handler, "a");
        HandleReloadSpellAreaCommand(handler, "a");
        HandleReloadSpellGroupsCommand(handler, "a");
        HandleReloadSpellLearnSpellCommand(handler, "a");
        HandleReloadSpellLinkedSpellCommand(handler, "a");
        HandleReloadSpellProcEventCommand(handler, "a");
        HandleReloadSpellProcsCommand(handler, "a");
        HandleReloadSpellBonusesCommand(handler, "a");
        HandleReloadSpellTargetPositionCommand(handler, "a");
        HandleReloadSpellThreatsCommand(handler, "a");
        HandleReloadSpellGroupStackRulesCommand(handler, "a");
        HandleReloadSpellPetAurasCommand(handler, "a");
        return true;
    }

    static bool HandleReloadAllGossipsCommand(ChatHandler* handler, const char* args)
    {
        HandleReloadGossipMenuCommand(handler, "a");
        HandleReloadGossipMenuOptionCommand(handler, "a");
        if (*args != 'a')                                          // already reload from all_scripts
        HandleReloadPointsOfInterestCommand(handler, "a");
        return true;
    }

    static bool HandleReloadAllItemCommand(ChatHandler* handler, const char* /*args*/)
    {
        HandleReloadPageTextsCommand(handler, "a");
        HandleReloadItemEnchantementsCommand(handler, "a");
        return true;
    }

    static bool HandleReloadAllLocalesCommand(ChatHandler* handler, const char* /*args*/)
    {
        HandleReloadLocalesAchievementRewardCommand(handler, "a");
        HandleReloadLocalesCreatureCommand(handler, "a");
        HandleReloadLocalesCreatureTextCommand(handler, "a");
        HandleReloadLocalesGameobjectCommand(handler, "a");
        HandleReloadLocalesGossipMenuOptionCommand(handler, "a");
        HandleReloadLocalesItemCommand(handler, "a");
        HandleReloadLocalesNpcTextCommand(handler, "a");
        HandleReloadLocalesPageTextCommand(handler, "a");
        HandleReloadLocalesPointsOfInterestCommand(handler, "a");
        HandleReloadLocalesQuestCommand(handler, "a");
        return true;
    }

    static bool HandleReloadConfigCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading config settings...");
        sWorld->LoadConfigSettings(true);
        sMapMgr->InitializeVisibilityDistanceInfo();
        handler->SendGlobalGMSysMessage("World config settings reloaded.");

#ifdef ELUNA  
		if (sWorld->getBoolConfig(CONFIG_BOOL_ELUNA_ENABLED))
		{
			StartEluna(true);
			handler->SendGlobalGMSysMessage("Eluna lua engine reloaded.");
		}
#endif

        return true;
    }

    static bool HandleReloadAccessRequirementCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Access Requirement definitions...");
        sObjectMgr->LoadAccessRequirements();
        handler->SendGlobalGMSysMessage("DB table `access_requirement` reloaded.");
        return true;
    }

    static bool HandleReloadAchievementCriteriaDataCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Additional Achievement Criteria Data...");
        sAchievementMgr->LoadAchievementCriteriaData();
        handler->SendGlobalGMSysMessage("DB table `achievement_criteria_data` reloaded.");
        return true;
    }

    static bool HandleReloadAchievementRewardCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Achievement Reward Data...");
        sAchievementMgr->LoadRewards();
        handler->SendGlobalGMSysMessage("DB table `achievement_reward` reloaded.");
        return true;
    }

    static bool HandleReloadAreaTriggerTavernCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Tavern Area Triggers...");
        sObjectMgr->LoadTavernAreaTriggers();
        handler->SendGlobalGMSysMessage("DB table `areatrigger_tavern` reloaded.");
        return true;
    }

    static bool HandleReloadAreaTriggerTeleportCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading AreaTrigger teleport definitions...");
        sObjectMgr->LoadAreaTriggerTeleports();
        handler->SendGlobalGMSysMessage("DB table `areatrigger_teleport` reloaded.");
        return true;
    }

    static bool HandleReloadAutobroadcastCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Autobroadcasts...");
        sWorld->LoadAutobroadcasts();
        handler->SendGlobalGMSysMessage("DB table `autobroadcast` reloaded.");
        return true;
    }

    static bool HandleReloaBattlegroundTemplateCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Battleground Templates...");
        sBattlegroundMgr->CreateInitialBattlegrounds(true);
        handler->SendGlobalGMSysMessage("DB table `battleground_template` reloaded.");
        return true;
    }

    static bool HandleReloadBroadcastTextCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Broadcast texts...");
        sObjectMgr->LoadBroadcastTexts();
        sObjectMgr->LoadBroadcastTextLocales();
        handler->SendGlobalGMSysMessage("DB table `broadcast_text` reloaded.");
        return true;
    }

    static bool HandleReloadCommandCommand(ChatHandler* handler, const char* /*args*/)
    {
        handler->SetLoadCommandTable(true);
        handler->SendGlobalGMSysMessage("DB table `command` will be reloaded at next chat command use.");
        return true;
    }

    static bool HandleReloadOnKillReputationCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading creature award reputation definitions...");
        sObjectMgr->LoadReputationOnKill();
        handler->SendGlobalGMSysMessage("DB table `creature_onkill_reputation` reloaded.");
        return true;
    }

    static bool HandleReloadCreatureSummonGroupsCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Reloading creature summon groups...");
        sObjectMgr->LoadTempSummons();
        handler->SendGlobalGMSysMessage("DB table `creature_summon_groups` reloaded.");
        return true;
    }

    static bool HandleReloadCreatureTemplateCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Tokenizer entries(std::string(args), ' ');

        for (Tokenizer::const_iterator itr = entries.begin(); itr != entries.end(); ++itr)
        {
            uint32 entry = uint32(atoi(*itr));

            WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_CREATURE_TEMPLATE);
            stmt->setUInt32(0, entry);
            PreparedQueryResult result = WorldDatabase.Query(stmt);

            if (!result)
            {
                handler->PSendSysMessage(LANG_COMMAND_CREATURETEMPLATE_NOTFOUND, entry);
                continue;
            }

            CreatureTemplate* cInfo = const_cast<CreatureTemplate*>(sObjectMgr->GetCreatureTemplate(entry));
            if (!cInfo)
            {
                handler->PSendSysMessage(LANG_COMMAND_CREATURESTORAGE_NOTFOUND, entry);
                continue;
            }

            TC_LOG_INFO("misc", "Reloading creature template entry %u", entry);

            Field* fields = result->Fetch();
            sObjectMgr->LoadCreatureTemplate(fields);
            sObjectMgr->CheckCreatureTemplate(cInfo);
        }

        handler->SendGlobalGMSysMessage("Creature template reloaded.");
        return true;
    }

    static bool HandleReloadCreatureQuestStarterCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Loading Quests Relations... (`creature_queststarter`)");
        sObjectMgr->LoadCreatureQuestStarters();
        handler->SendGlobalGMSysMessage("DB table `creature_queststarter` reloaded.");
        return true;
    }

    static bool HandleReloadCreatureScallingCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Loading Creature Scaling data... (`creature_scalling`)");
        sObjectMgr->LoadCreatureScaling();
        handler->SendGlobalGMSysMessage("DB table `creature_scaling` reloaded.");
        return true;
    }

    static bool HandleReloadLinkedRespawnCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Loading Linked Respawns... (`creature_linked_respawn`)");
        sObjectMgr->LoadLinkedRespawn();
        handler->SendGlobalGMSysMessage("DB table `creature_linked_respawn` (creature linked respawns) reloaded.");
        return true;
    }

    static bool HandleReloadCreatureQuestEnderCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Loading Quests Relations... (`creature_questender`)");
        sObjectMgr->LoadCreatureQuestEnders();
        handler->SendGlobalGMSysMessage("DB table `creature_questender` reloaded.");
        return true;
    }

    static bool HandleReloadGossipMenuCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading `gossip_menu` Table!");
        sObjectMgr->LoadGossipMenu();
        handler->SendGlobalGMSysMessage("DB table `gossip_menu` reloaded.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadGossipMenuOptionCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading `gossip_menu_option` Table!");
        sObjectMgr->LoadGossipMenuItems();
        handler->SendGlobalGMSysMessage("DB table `gossip_menu_option` reloaded.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadGOQuestStarterCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Loading Quests Relations... (`gameobject_queststarter`)");
        sObjectMgr->LoadGameobjectQuestStarters();
        handler->SendGlobalGMSysMessage("DB table `gameobject_queststarter` reloaded.");
        return true;
    }

    static bool HandleReloadGOQuestEnderCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Loading Quests Relations... (`gameobject_questender`)");
        sObjectMgr->LoadGameobjectQuestEnders();
        handler->SendGlobalGMSysMessage("DB table `gameobject_questender` reloaded.");
        return true;
    }

    static bool HandleReloadQuestGiverAreaTriggersCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Quest Giver Area Triggers...");
        sObjectMgr->LoadQuestGiverAreaTriggers();
        handler->SendGlobalGMSysMessage("DB table `areatrigger_queststarter` (quest giver area triggers) reloaded.");
        return true;
    }

    static bool HandleReloadQuestAreaTriggersCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Quest Area Triggers...");
        sObjectMgr->LoadQuestAreaTriggers();
        handler->SendGlobalGMSysMessage("DB table `areatrigger_questender` (quest area triggers) reloaded.");
        return true;
    }

    static bool HandleReloadQuestTemplateCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Quest Templates...");
        sObjectMgr->LoadQuests();
        handler->SendGlobalGMSysMessage("DB table `quest_template` (quest definitions) reloaded.");

        /// dependent also from `gameobject` but this table not reloaded anyway
        TC_LOG_INFO("misc", "Re-Loading GameObjects for quests...");
        sObjectMgr->LoadGameObjectForQuests();
        handler->SendGlobalGMSysMessage("Data GameObjects for quests reloaded.");
        return true;
    }

    static bool HandleReloadLootTemplatesCreatureCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Loot Tables... (`creature_loot_template`)");
        LoadLootTemplates_Creature();
        LootTemplates_Creature.CheckLootRefs();
        handler->SendGlobalGMSysMessage("DB table `creature_loot_template` reloaded.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadCreatureMovementOverrideCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Creature movement overrides...");
        sObjectMgr->LoadCreatureMovementOverrides();
        handler->SendGlobalGMSysMessage("DB table `creature_movement_override` reloaded.");
        return true;
    }    

    static bool HandleReloadLootTemplatesDisenchantCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Loot Tables... (`disenchant_loot_template`)");
        LoadLootTemplates_Disenchant();
        LootTemplates_Disenchant.CheckLootRefs();
        handler->SendGlobalGMSysMessage("DB table `disenchant_loot_template` reloaded.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadLootTemplatesFishingCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Loot Tables... (`fishing_loot_template`)");
        LoadLootTemplates_Fishing();
        LootTemplates_Fishing.CheckLootRefs();
        handler->SendGlobalGMSysMessage("DB table `fishing_loot_template` reloaded.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadLootTemplatesGameobjectCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Loot Tables... (`gameobject_loot_template`)");
        LoadLootTemplates_Gameobject();
        LootTemplates_Gameobject.CheckLootRefs();
        handler->SendGlobalGMSysMessage("DB table `gameobject_loot_template` reloaded.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadLootTemplatesItemCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Loot Tables... (`item_loot_template`)");
        LoadLootTemplates_Item();
        LootTemplates_Item.CheckLootRefs();
        handler->SendGlobalGMSysMessage("DB table `item_loot_template` reloaded.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadLootTemplatesMillingCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Loot Tables... (`milling_loot_template`)");
        LoadLootTemplates_Milling();
        LootTemplates_Milling.CheckLootRefs();
        handler->SendGlobalGMSysMessage("DB table `milling_loot_template` reloaded.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadLootTemplatesPickpocketingCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Loot Tables... (`pickpocketing_loot_template`)");
        LoadLootTemplates_Pickpocketing();
        LootTemplates_Pickpocketing.CheckLootRefs();
        handler->SendGlobalGMSysMessage("DB table `pickpocketing_loot_template` reloaded.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadLootTemplatesProspectingCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Loot Tables... (`prospecting_loot_template`)");
        LoadLootTemplates_Prospecting();
        LootTemplates_Prospecting.CheckLootRefs();
        handler->SendGlobalGMSysMessage("DB table `prospecting_loot_template` reloaded.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadLootTemplatesMailCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Loot Tables... (`mail_loot_template`)");
        LoadLootTemplates_Mail();
        LootTemplates_Mail.CheckLootRefs();
        handler->SendGlobalGMSysMessage("DB table `mail_loot_template` reloaded.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadLootTemplatesReferenceCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Loot Tables... (`reference_loot_template`)");
        LoadLootTemplates_Reference();
        handler->SendGlobalGMSysMessage("DB table `reference_loot_template` reloaded.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadLootTemplatesSkinningCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Loot Tables... (`skinning_loot_template`)");
        LoadLootTemplates_Skinning();
        LootTemplates_Skinning.CheckLootRefs();
        handler->SendGlobalGMSysMessage("DB table `skinning_loot_template` reloaded.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadLootTemplatesSpellCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Loot Tables... (`spell_loot_template`)");
        LoadLootTemplates_Spell();
        LootTemplates_Spell.CheckLootRefs();
        handler->SendGlobalGMSysMessage("DB table `spell_loot_template` reloaded.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadTrinityStringCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading trinity_string Table!");
        sObjectMgr->LoadTrinityStrings();
        handler->SendGlobalGMSysMessage("DB table `trinity_string` reloaded.");
        return true;
    }

    static bool HandleReloadNpcTrainerCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading `npc_trainer` Table!");
        sObjectMgr->LoadTrainerSpell();
        handler->SendGlobalGMSysMessage("DB table `npc_trainer` reloaded.");
        return true;
    }

    static bool HandleReloadNpcVendorCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading `npc_vendor` Table!");
        sObjectMgr->LoadVendors();
        handler->SendGlobalGMSysMessage("DB table `npc_vendor` reloaded.");
        return true;
    }

    static bool HandleReloadPointsOfInterestCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading `points_of_interest` Table!");
        sObjectMgr->LoadPointsOfInterest();
        handler->SendGlobalGMSysMessage("DB table `points_of_interest` reloaded.");
        return true;
    }

    static bool HandleReloadPromotionAurasCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading `promotion_auras` Table!");
        sObjectMgr->LoadPromotionAuras();
        handler->SendGlobalGMSysMessage("DB table `promotion_auras` reloaded.");
        return true;
    }

    static bool HandleReloadQuestPOICommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Quest POI ..." );
        sObjectMgr->LoadQuestPOI();
        handler->SendGlobalGMSysMessage("DB Table `quest_poi` and `quest_poi_points` reloaded.");
        return true;
    }

    static bool HandleReloadSpellClickSpellsCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading `npc_spellclick_spells` Table!");
        sObjectMgr->LoadNPCSpellClickSpells();
        handler->SendGlobalGMSysMessage("DB table `npc_spellclick_spells` reloaded.");
        return true;
    }

    static bool HandleReloadReservedNameCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Loading ReservedNames... (`reserved_name`)");
        sObjectMgr->LoadReservedPlayersNames();
        handler->SendGlobalGMSysMessage("DB table `reserved_name` (player reserved names) reloaded.");
        return true;
    }

    static bool HandleReloadReputationRewardRateCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading `reputation_reward_rate` Table!" );
        sObjectMgr->LoadReputationRewardRate();
        handler->SendGlobalSysMessage("DB table `reputation_reward_rate` reloaded.");
        return true;
    }

    static bool HandleReloadReputationSpilloverTemplateCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading `reputation_spillover_template` Table!" );
        sObjectMgr->LoadReputationSpilloverTemplate();
        handler->SendGlobalSysMessage("DB table `reputation_spillover_template` reloaded.");
        return true;
    }

    static bool HandleReloadRBACCommand(ChatHandler* handler, const char* /*args*/)
    {
        sWorld->ReloadRBAC();
        handler->SendGlobalGMSysMessage("RBAC data reloaded.");
        return true;
    }

    static bool HandleReloadSkillDiscoveryTemplateCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Skill Discovery Table...");
        LoadSkillDiscoveryTable();
        handler->SendGlobalGMSysMessage("DB table `skill_discovery_template` (recipes discovered at crafting) reloaded.");
        return true;
    }

    static bool HandleReloadSkillExtraItemTemplateCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Skill Extra Item Table...");
        LoadSkillExtraItemTable();
        handler->SendGlobalGMSysMessage("DB table `skill_extra_item_template` (extra item creation when crafting) reloaded.");
        return true;
    }

    static bool HandleReloadSkillFishingBaseLevelCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Skill Fishing base level requirements...");
        sObjectMgr->LoadFishingBaseSkillLevel();
        handler->SendGlobalGMSysMessage("DB table `skill_fishing_base_level` (fishing base level for zone/subzone) reloaded.");
        return true;
    }

    static bool HandleReloadSpellAreaCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading SpellArea Data...");
        sSpellMgr->LoadSpellAreas();
        handler->SendGlobalGMSysMessage("DB table `spell_area` (spell dependences from area/quest/auras state) reloaded.");
        return true;
    }

    static bool HandleReloadSpellRequiredCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Spell Required Data... ");
        sSpellMgr->LoadSpellRequired();
        handler->SendGlobalGMSysMessage("DB table `spell_required` reloaded.");
        return true;
    }

    static bool HandleReloadSpellGroupsCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Spell Groups...");
        sSpellMgr->LoadSpellGroups();
        handler->SendGlobalGMSysMessage("DB table `spell_group` (spell groups) reloaded.");
        return true;
    }

    static bool HandleReloadSpellLearnSpellCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Spell Learn Spells...");
        sSpellMgr->LoadSpellLearnSpells();
        handler->SendGlobalGMSysMessage("DB table `spell_learn_spell` reloaded.");
        return true;
    }

    static bool HandleReloadSpellLinkedSpellCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Spell Linked Spells...");
        sSpellMgr->LoadSpellLinked();
        handler->SendGlobalGMSysMessage("DB table `spell_linked_spell` reloaded.");
        return true;
    }

    static bool HandleReloadSpellProcEventCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Spell Proc Event conditions...");
        sSpellMgr->LoadSpellProcEvents();
        handler->SendGlobalGMSysMessage("DB table `spell_proc_event` (spell proc trigger requirements) reloaded.");
        return true;
    }

    static bool HandleReloadSpellProcsCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Spell Proc conditions and data...");
        sSpellMgr->LoadSpellProcs();
        handler->SendGlobalGMSysMessage("DB table `spell_proc` (spell proc conditions and data) reloaded.");
        return true;
    }

    static bool HandleReloadSpellBonusesCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Spell Bonus Data...");
        sSpellMgr->LoadSpellBonusess();
        handler->SendGlobalGMSysMessage("DB table `spell_bonus_data` (spell damage/healing coefficients) reloaded.");
        return true;
    }

    static bool HandleReloadSpellTargetPositionCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Spell target coordinates...");
        sSpellMgr->LoadSpellTargetPositions();
        handler->SendGlobalGMSysMessage("DB table `spell_target_position` (destination coordinates for spell targets) reloaded.");
        return true;
    }

    static bool HandleReloadSpellThreatsCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Aggro Spells Definitions...");
        sSpellMgr->LoadSpellThreats();
        handler->SendGlobalGMSysMessage("DB table `spell_threat` (spell aggro definitions) reloaded.");
        return true;
    }

    static bool HandleReloadSpellGroupStackRulesCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Spell Group Stack Rules...");
        sSpellMgr->LoadSpellGroupStackRules();
        handler->SendGlobalGMSysMessage("DB table `spell_group_stack_rules` (spell stacking definitions) reloaded.");
        return true;
    }

    static bool HandleReloadSpellPetAurasCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Spell pet auras...");
        sSpellMgr->LoadSpellPetAuras();
        handler->SendGlobalGMSysMessage("DB table `spell_pet_auras` reloaded.");
        return true;
    }

    static bool HandleReloadPageTextsCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Page Texts...");
        sObjectMgr->LoadPageTexts();
        handler->SendGlobalGMSysMessage("DB table `page_texts` reloaded.");
        return true;
    }

    static bool HandleReloadItemEnchantementsCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Item Random Enchantments Table...");
        LoadRandomEnchantmentsTable();
        handler->SendGlobalGMSysMessage("DB table `item_enchantment_template` reloaded.");
        return true;
    }

    static bool HandleReloadEventScriptsCommand(ChatHandler* handler, const char* args)
    {
        if (sScriptMgr->IsScriptScheduled())
        {
            handler->SendSysMessage("DB scripts used currently, please attempt reload later.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (*args != 'a')
            TC_LOG_INFO("misc", "Re-Loading Scripts from `event_scripts`...");

        sObjectMgr->LoadEventScripts();

        if (*args != 'a')
            handler->SendGlobalGMSysMessage("DB table `event_scripts` reloaded.");

        return true;
    }

    static bool HandleReloadWpScriptsCommand(ChatHandler* handler, const char* args)
    {
        if (sScriptMgr->IsScriptScheduled())
        {
            handler->SendSysMessage("DB scripts used currently, please attempt reload later.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (*args != 'a')
            TC_LOG_INFO("misc", "Re-Loading Scripts from `waypoint_scripts`...");

        sObjectMgr->LoadWaypointScripts();

        if (*args != 'a')
            handler->SendGlobalGMSysMessage("DB table `waypoint_scripts` reloaded.");

        return true;
    }

    static bool HandleReloadWpCommand(ChatHandler* handler, const char* args)
    {
        if (*args != 'a')
            TC_LOG_INFO("misc", "Re-Loading Waypoints data from 'waypoints_data'");

        sWaypointMgr->Load();

        if (*args != 'a')
            handler->SendGlobalGMSysMessage("DB Table 'waypoint_data' reloaded.");

        return true;
    }

    static bool HandleReloadSpellScriptsCommand(ChatHandler* handler, const char* args)
    {
        if (sScriptMgr->IsScriptScheduled())
        {
            handler->SendSysMessage("DB scripts used currently, please attempt reload later.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (*args != 'a')
            TC_LOG_INFO("misc", "Re-Loading Scripts from `spell_scripts`...");

        sObjectMgr->LoadSpellScripts();

        if (*args != 'a')
            handler->SendGlobalGMSysMessage("DB table `spell_scripts` reloaded.");

        return true;
    }

    static bool HandleReloadGameGraveyardZoneCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Graveyard-zone links...");

        sObjectMgr->LoadGraveyardZones();

        handler->SendGlobalGMSysMessage("DB table `game_graveyard_zone` reloaded.");

        return true;
    }

    static bool HandleReloadGameTeleCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Game Tele coordinates...");

        sObjectMgr->LoadGameTele();

        handler->SendGlobalGMSysMessage("DB table `game_tele` reloaded.");

        return true;
    }

    static bool HandleReloadDisablesCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading disables table...");
        DisableMgr::LoadDisables();
        TC_LOG_INFO("misc", "Checking quest disables...");
        DisableMgr::CheckQuestDisables();
        handler->SendGlobalGMSysMessage("DB table `disables` reloaded.");
        return true;
    }

    static bool HandleReloadLocalesAchievementRewardCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Locales Achievement Reward Data...");
        sAchievementMgr->LoadRewardLocales();
        handler->SendGlobalGMSysMessage("DB table `locales_achievement_reward` reloaded.");
        return true;
    }

    static bool HandleReloadLfgRewardsCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading lfg dungeon rewards...");
        sLFGMgr->LoadRewards();
        handler->SendGlobalGMSysMessage("DB table `lfg_dungeon_rewards` reloaded.");
        return true;
    }

    static bool HandleReloadLocalesCreatureCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Locales Creature ...");
        sObjectMgr->LoadCreatureLocales();
        handler->SendGlobalGMSysMessage("DB table `locales_creature` reloaded.");
        return true;
    }

    static bool HandleReloadLocalesCreatureTextCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Locales Creature Texts...");
        sCreatureTextMgr->LoadCreatureTextLocales();
        handler->SendGlobalGMSysMessage("DB table `locales_creature_text` reloaded.");
        return true;
    }

    static bool HandleReloadLocalesGameobjectCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Gameobject Template Locale ... ");
        sObjectMgr->LoadGameObjectLocales();
        handler->SendGlobalGMSysMessage("DB table `gameobject_template_locale` reloaded.");
        return true;
    }

    static bool HandleReloadLocalesGossipMenuOptionCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Locales Gossip Menu Option ... ");
        sObjectMgr->LoadGossipMenuItemsLocales();
        handler->SendGlobalGMSysMessage("DB table `gossip_menu_option_locales` reloaded.");
        return true;
    }

    static bool HandleReloadLocalesItemCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Locales Item ... ");
        sObjectMgr->LoadItemLocales();
        handler->SendGlobalGMSysMessage("DB table `locales_item` reloaded.");
        return true;
    }

    static bool HandleReloadLocalesNpcTextCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Locales NPC Text ... ");
        sObjectMgr->LoadNpcTextLocales();
        handler->SendGlobalGMSysMessage("DB table `npc_text_locales` reloaded.");
        return true;
    }

    static bool HandleReloadLocalesPageTextCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Locales Page Text ... ");
        sObjectMgr->LoadPageTextLocales();
        handler->SendGlobalGMSysMessage("DB table `locales_page_text` reloaded.");
        return true;
    }

    static bool HandleReloadLocalesPointsOfInterestCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Locales Points Of Interest ... ");
        sObjectMgr->LoadPointOfInterestLocales();
        handler->SendGlobalGMSysMessage("DB table `locales_points_of_interest` reloaded.");
        return true;
    }

    static bool HandleReloadLocalesQuestCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Locales Quest ... ");
        sObjectMgr->LoadQuestTemplateLocale();
        sObjectMgr->LoadQuestObjectivesLocale();
        sObjectMgr->LoadQuestOfferRewardLocale();
        sObjectMgr->LoadQuestRequestItemsLocale();
        handler->SendGlobalGMSysMessage("DB table `locales_quest` reloaded.");
        return true;
    }

    static bool HandleReloadMailLevelRewardCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Player level dependent mail rewards...");
        sObjectMgr->LoadMailLevelRewards();
        handler->SendGlobalGMSysMessage("DB table `mail_level_reward` reloaded.");
        return true;
    }

    static bool HandleReloadAuctionsCommand(ChatHandler* handler, const char* /*args*/)
    {
        ///- Reload dynamic data tables from the database
        TC_LOG_INFO("misc", "Re-Loading Auctions...");
        sAuctionMgr->LoadAuctionItems();
        sAuctionMgr->LoadAuctions();
        handler->SendGlobalGMSysMessage("Auctions reloaded.");
        return true;
    }

    static bool HandleReloadConditions(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Conditions...");
        sConditionMgr->LoadConditions(true);
        handler->SendGlobalGMSysMessage("Conditions reloaded.");
        return true;
    }

    static bool HandleReloadCreatureText(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Creature Texts...");
        sCreatureTextMgr->LoadCreatureTexts();
        handler->SendGlobalGMSysMessage("Creature Texts reloaded.");
        return true;
    }

    static bool HandleReloadSmartScripts(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Smart Scripts...");
        sSmartScriptMgr->LoadSmartAIFromDB();
        handler->SendGlobalGMSysMessage("Smart Scripts reloaded.");
        return true;
    }

    static bool HandleReloadVehicleAccessoryCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Reloading vehicle_accessory table...");
        sObjectMgr->LoadVehicleAccessories();
        handler->SendGlobalGMSysMessage("Vehicle accessories reloaded.");
        return true;
    }

    static bool HandleReloadVehicleTemplateAccessoryCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Reloading vehicle_template_accessory table...");
        sObjectMgr->LoadVehicleTemplateAccessories();
        handler->SendGlobalGMSysMessage("Vehicle template accessories reloaded.");
        return true;
    }


    static bool HandleReloadPhaseDefinitionsCommand(ChatHandler* handler, const char* /*args*/)
    {
        TC_LOG_INFO("misc", "Reloading phase_definitions table...");
        sObjectMgr->LoadPhaseDefinitions();
        sWorld->UpdatePhaseDefinitions();
        handler->SendGlobalGMSysMessage("Phase Definitions reloaded.");
        return true;
    }

    static bool HandleReloadCreatureDifficulty(ChatHandler* handler, char const* /*arg*/)
    {
        TC_LOG_INFO("misc", "Reloading creature_difficulty...");
        sObjectMgr->LoadCreatureDifficultyModifiers();
        handler->SendGlobalGMSysMessage("Table creature_difficulty reloaded.");
        return true;
    }

    static bool HandleReloadSceneTemplateCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "Reloading scene_template table...");
        sObjectMgr->LoadSceneTemplates();
        handler->SendGlobalGMSysMessage("Scenes templates reloaded. New scriptname need a reboot.");
        return true;
    }

    static bool HandleReloadCreatureLootCurrency(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "Reloading creature_loot_currency table...");
        sLootMgr->LoadCreatureLootCurrency();
        handler->SendGlobalGMSysMessage("Table creature_loot_currency reloaded.");
        return true;
    }


    static bool HandleReloadGOCollisionCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        uint32 entry = atoi(args);
        if (!entry)
            return false;

        if (!sObjectMgr->GetGameObjectTemplate(entry))
        {
            handler->PSendSysMessage(LANG_COMMAND_NOGAMEOBJECTFOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 count = 0;
        sMapMgr->DoForAllMaps([&count,entry](Map* map)
        {
            for (auto go : map->GetGameObjectBySpawnIdStore())
            {
                if (go.second->GetEntry() == entry)
                {
                    go.second->UpdateCollision();
                    ++count;
                }
            }
        });

        handler->PSendSysMessage("Updated collision for %u spawned GameObjects", count);
        return true;
    }

    static bool HandleReloadBattlePayCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "Reloading Battle Pay tables...");
        sBattlePayMgr->LoadFromDb();
        LoadBoostItems();
        handler->SendGlobalGMSysMessage("Battle Pay reloaded.");
        return true;
    }
};

void AddSC_reload_commandscript()
{
    new reload_commandscript();
}
