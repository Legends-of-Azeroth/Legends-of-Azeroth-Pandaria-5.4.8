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

/* ScriptData
Name: reset_commandscript
%Complete: 100
Comment: All reset related commands
Category: commandscripts
EndScriptData */

#include "AchievementMgr.h"
#include "Chat.h"
#include "Language.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "Pet.h"
#include "ScriptMgr.h"
#include "RatedPvp.h"
#include <boost/algorithm/string.hpp> 

class reset_commandscript : public CommandScript
{
public:
    reset_commandscript() : CommandScript("reset_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> resetCommandTable =
        {
            { "achievements",   &HandleResetAchievementsCommand,    rbac::RBAC_PERM_COMMAND_RESET_ACHIEVEMENTS, Trinity::ChatCommands::Console::Yes },
            { "honor",          &HandleResetHonorCommand,           rbac::RBAC_PERM_COMMAND_RESET_HONOR,          Trinity::ChatCommands::Console::Yes },
            { "level",          &HandleResetLevelCommand,           rbac::RBAC_PERM_COMMAND_RESET_LEVEL,          Trinity::ChatCommands::Console::Yes },
            { "spells",         &HandleResetSpellsCommand,          rbac::RBAC_PERM_COMMAND_RESET_SPELLS,         Trinity::ChatCommands::Console::Yes },
            { "stats",          &HandleResetStatsCommand,           rbac::RBAC_PERM_COMMAND_RESET_STATS,          Trinity::ChatCommands::Console::Yes },
            { "talents",        &HandleResetTalentsCommand,         rbac::RBAC_PERM_COMMAND_RESET_TALENTS,        Trinity::ChatCommands::Console::Yes },
            { "all",            &HandleResetAllCommand,             rbac::RBAC_PERM_COMMAND_RESET_ALL,            Trinity::ChatCommands::Console::Yes },
            { "pvpstat", &HandleResetPvpStat, rbac::RBAC_PERM_COMMAND_RESET_PVPSTAT, Trinity::ChatCommands::Console::No },
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "reset",          resetCommandTable,                  rbac::RBAC_PERM_COMMAND_RESET,                Trinity::ChatCommands::Console::Yes },
            { "arena", {
                { "disband", &HandleArenaDisband, rbac::RBAC_PERM_COMMAND_ARENA_DISBAND, Trinity::ChatCommands::Console::Yes },
            }, rbac::RBAC_PERM_COMMAND_ARENA, Trinity::ChatCommands::Console::Yes },
        };
        return commandTable;
    }

    static bool HandleResetAchievementsCommand(ChatHandler* handler, char const* args)
    {
        Player* target;
        ObjectGuid targetGuid;
        if (!handler->extractPlayerTarget((char*)args, &target, &targetGuid))
            return false;

        if (target)
            target->ResetAchievements();
        else
            PlayerAchievementMgr::DeleteFromDB(targetGuid.GetCounter());

        return true;
    }

    static bool HandleResetHonorCommand(ChatHandler* handler, char const* args)
    {
        Player* target;
        if (!handler->extractPlayerTarget((char*)args, &target))
            return false;

        target->SetUInt32Value(PLAYER_FIELD_YESTERDAY_HONORABLE_KILLS, 0);
        target->SetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS, 0);
        target->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_EARN_HONORABLE_KILL);

        return true;
    }

    static bool HandleResetStatsOrLevelHelper(Player* player)
    {
        ChrClassesEntry const* classEntry = sChrClassesStore.LookupEntry(player->GetClass());
        if (!classEntry)
        {
            TC_LOG_ERROR("misc", "Class %u not found in DBC (Wrong DBC files?)", player->GetClass());
            return false;
        }

        uint8 powerType = classEntry->powerType;

        // reset m_form if no aura
        if (!player->HasAuraType(SPELL_AURA_MOD_SHAPESHIFT))
            player->SetShapeshiftForm(FORM_NONE);

        player->setFactionForRace(player->GetRace());

        player->SetRace(player->GetRace());
        player->SetClass(player->GetClass());
        player->SetGender(player->GetGender());

        player->SetFieldPowerType(powerType);

        // reset only if player not in some form;
        if (player->GetShapeshiftForm() == FORM_NONE)
            player->InitDisplayIds();

        player->SetByteValue(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_PVP);

        player->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);

        //-1 is default value
        player->SetUInt32Value(PLAYER_FIELD_WATCHED_FACTION_INDEX, uint32(-1));
        return true;
    }

    static bool HandleResetLevelCommand(ChatHandler* handler, char const* args)
    {
        Player* target;
        if (!handler->extractPlayerTarget((char*)args, &target))
            return false;

        if (!HandleResetStatsOrLevelHelper(target))
            return false;

        uint8 oldLevel = target->GetLevel();

        // set starting level
        uint32 startLevel = target->GetClass() != CLASS_DEATH_KNIGHT
            ? sWorld->getIntConfig(CONFIG_START_PLAYER_LEVEL)
            : sWorld->getIntConfig(CONFIG_START_HEROIC_PLAYER_LEVEL);

        target->_ApplyAllLevelScaleItemMods(false);
        target->SetLevel(startLevel);
        target->InitRunes();
        target->InitStatsForLevel(true);
        target->InitTaxiNodesForLevel();
        target->InitGlyphsForLevel();
        target->InitTalentForLevel();
        target->SetUInt32Value(PLAYER_FIELD_XP, 0);

        target->_ApplyAllLevelScaleItemMods(true);

        // reset level for pet
        if (Pet* pet = target->GetPet())
            pet->SynchronizeLevelWithOwner();

        sScriptMgr->OnPlayerLevelChanged(target, oldLevel);

        return true;
    }

    static bool HandleResetSpellsCommand(ChatHandler* handler, char const* args)
    {
        Player* target;
        ObjectGuid targetGuid;
        std::string targetName;
        if (!handler->extractPlayerTarget((char*)args, &target, &targetGuid, &targetName))
            return false;

        if (target)
        {
            target->ResetSpells(/* bool myClassOnly */);

            ChatHandler(target->GetSession()).SendSysMessage(LANG_RESET_SPELLS);
            if (!handler->GetSession() || handler->GetSession()->GetPlayer() != target)
                handler->PSendSysMessage(LANG_RESET_SPELLS_ONLINE, handler->GetNameLink(target).c_str());
        }
        else
        {
            CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ADD_AT_LOGIN_FLAG);
            stmt->setUInt16(0, uint16(AT_LOGIN_RESET_SPELLS));
            stmt->setUInt32(1, targetGuid.GetCounter());
            CharacterDatabase.Execute(stmt);

            handler->PSendSysMessage(LANG_RESET_SPELLS_OFFLINE, targetName.c_str());
        }

        return true;
    }

    static bool HandleResetStatsCommand(ChatHandler* handler, char const* args)
    {
        Player* target;
        if (!handler->extractPlayerTarget((char*)args, &target))
            return false;

        if (!HandleResetStatsOrLevelHelper(target))
            return false;

        target->InitRunes();
        target->InitStatsForLevel(true);
        target->InitTaxiNodesForLevel();
        target->InitGlyphsForLevel();
        target->InitTalentForLevel();

        return true;
    }

    static bool HandleResetTalentsCommand(ChatHandler* handler, char const* args)
    {
        Player* target;
        ObjectGuid targetGuid;
        std::string targetName;
        if (!handler->extractPlayerTarget((char*)args, &target, &targetGuid, &targetName))
        {
            // Try reset talents as Hunter Pet
            Creature* creature = handler->getSelectedCreature();
            if (!*args && creature && creature->IsPet())
            {
                Unit* owner = creature->GetOwner();
                if (owner && owner->GetTypeId() == TYPEID_PLAYER && creature->ToPet()->IsPermanentPetFor(owner->ToPlayer()))
                {
                    ChatHandler(owner->ToPlayer()->GetSession()).SendSysMessage(LANG_RESET_PET_TALENTS);
                    if (!handler->GetSession() || handler->GetSession()->GetPlayer() != owner->ToPlayer())
                        handler->PSendSysMessage(LANG_RESET_PET_TALENTS_ONLINE, handler->GetNameLink(owner->ToPlayer()).c_str());
                }
                return true;
            }

            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (target)
        {
            target->ResetTalents(true);
            target->SendTalentsInfoData();
            ChatHandler(target->GetSession()).SendSysMessage(LANG_RESET_TALENTS);
            if (!handler->GetSession() || handler->GetSession()->GetPlayer() != target)
                handler->PSendSysMessage(LANG_RESET_TALENTS_ONLINE, handler->GetNameLink(target).c_str());

            return true;
        }
        else if (targetGuid)
        {
            CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ADD_AT_LOGIN_FLAG);
            stmt->setUInt16(0, uint16(AT_LOGIN_NONE | AT_LOGIN_RESET_PET_TALENTS));
            stmt->setUInt32(1, targetGuid.GetCounter());
            CharacterDatabase.Execute(stmt);

            std::string nameLink = handler->playerLink(targetName);
            handler->PSendSysMessage(LANG_RESET_TALENTS_OFFLINE, nameLink.c_str());
            return true;
        }

        handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
        handler->SetSentErrorMessage(true);
        return false;
    }

    static bool HandleResetAllCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        std::string caseName = args;

        AtLoginFlags atLogin;

        // Command specially created as single command to prevent using short case names
        if (caseName == "spells")
        {
            atLogin = AT_LOGIN_RESET_SPELLS;
            sWorld->SendWorldText(LANG_RESETALL_SPELLS);
            if (!handler->GetSession())
                handler->SendSysMessage(LANG_RESETALL_SPELLS);
        }
        else if (caseName == "talents")
        {
            atLogin = AtLoginFlags(AT_LOGIN_RESET_TALENTS | AT_LOGIN_RESET_PET_TALENTS);
            sWorld->SendWorldText(LANG_RESETALL_TALENTS);
            if (!handler->GetSession())
               handler->SendSysMessage(LANG_RESETALL_TALENTS);
        }
        else
        {
            handler->PSendSysMessage(LANG_RESETALL_UNKNOWN_CASE, args);
            handler->SetSentErrorMessage(true);
            return false;
        }

        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ALL_AT_LOGIN_FLAGS);
        stmt->setUInt16(0, uint16(atLogin));
        CharacterDatabase.Execute(stmt);

        std::shared_lock<std::shared_mutex> lock(*HashMapHolder<Player>::GetLock());
        HashMapHolder<Player>::MapType const& plist = ObjectAccessor::GetPlayers();
        for (HashMapHolder<Player>::MapType::const_iterator itr = plist.begin(); itr != plist.end(); ++itr)
            itr->second->SetAtLoginFlag(atLogin);

        return true;
    }

    static bool HandleResetPvpStat(ChatHandler* handler, char const* args)
    {
        std::string str = args;
        Tokenizer toks{ str, ' ' };
        if (toks.size() < 2)
            return false;

        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)toks[0], nullptr, &guid, &name))
            return false;

        uint32 type = atol(toks[1]);
        RatedPvpSlot slot;
        switch (type)
        {
            case 2: slot = RatedPvpSlot::PVP_SLOT_ARENA_2v2; break;
            case 3: slot = RatedPvpSlot::PVP_SLOT_ARENA_3v3; break;
            case 5: slot = RatedPvpSlot::PVP_SLOT_ARENA_5v5; break;
            case 10:slot = RatedPvpSlot::PVP_SLOT_RATED_BG;  break;
            default:
                return handler->SendError("Invalid value, use 2, 3, 5 for arenas or 10 for rated BG");
        }

        ResetPvpStat(handler, guid, slot);
        handler->PSendSysMessage("Pvp stat (type %u) for player %s was reseted", type, handler->playerLink(name).c_str());
        return true;
    }

    static void ResetPvpStat(ChatHandler* handler, ObjectGuid guid, RatedPvpSlot slot)
    {
        auto info = sRatedPvpMgr->GetInfo(slot, guid);
        if (info)
        {
            if (handler->GetSession())
                sLog->outCommand(handler->GetSession()->GetAccountId(), "Pvp stat (slot: %u) for %u: Rating %u, MatchmakerRating %u, SeasonGames %u, SeasonWins %u, SeasonBest %u, WeekGames %u, WeekWins %u, WeekBest %u",
                    uint32(slot), guid.GetCounter(), info->Rating, info->MatchmakerRating, info->SeasonGames, info->SeasonWins, info->SeasonBest, info->WeekGames, info->WeekWins, info->WeekBest);

            info->Rating = 0;
            info->MatchmakerRating = sWorld->getIntConfig(CONFIG_ARENA_START_MATCHMAKER_RATING);
            info->SeasonGames = 0;
            info->SeasonWins = 0;
            info->SeasonBest = 0;
            info->WeekGames = 0;
            info->WeekWins = 0;
            info->WeekBest = 0;
            info->LastWeekBest = 0;
            RatedPvpMgr::SaveToDB(info);
        }
    }

    static bool HandleArenaDisband(ChatHandler* handler, char const* args)
    {
        uint64 teamId = std::strtoull(args, nullptr, 10);
        uint64 slot = teamId / 100000000000 - 1;
        ObjectGuid guid(HighGuid::Player, uint32(teamId - 100000000000 * (slot + 1)));

        std::string name;
        std::string msg;
        if (!sObjectMgr->GetPlayerNameByGUID(guid, name) || slot > 2)
        {
            std::string msg = sObjectMgr->GetTrinityString(LANG_ARENA_ERROR_NOT_FOUND, LocaleConstant(handler->GetSessionDbLocaleIndex()));
            boost::replace_all(msg, "%u", UI64FMTD);
            return handler->SendError(msg.c_str(), teamId);
        }

        ResetPvpStat(handler, guid, RatedPvpSlot(slot));
        msg = sObjectMgr->GetTrinityString(LANG_ARENA_DISBAND, LocaleConstant(handler->GetSessionDbLocaleIndex()));
        boost::replace_all(msg, "%u", UI64FMTD);
        handler->PSendSysMessage(msg.c_str(), name.c_str(), teamId);
        return true;
    }
};

void AddSC_reset_commandscript()
{
    new reset_commandscript();
}
