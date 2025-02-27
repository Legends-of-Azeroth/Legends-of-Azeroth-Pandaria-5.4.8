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

#include "Chat.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "AchievementMgr.h"
#include "GuildMgr.h"
#include "ReputationMgr.h"
#include "QuestDef.h"
#include "RatedPvp.h"

class support_commandscript : public CommandScript
{
public:
    support_commandscript() : CommandScript("support_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> commandTable =
        {
            { "support",                SEC_ADMINISTRATOR,  true,
            {
                { "add",                SEC_ADMINISTRATOR,  false,
                {
                    { "spell",          SEC_ADMINISTRATOR,  true,   &HandleSupportAddSpellCommand,       },
                    { "skill",          SEC_ADMINISTRATOR,  true,   &HandleSupportAddSkillCommand,       },
                    { "item",           SEC_ADMINISTRATOR,  true,   &HandleSupportAddItemCommand,        },
                    { "title",          SEC_ADMINISTRATOR,  true,   &HandleSupportAddTitleCommand,       },
                    { "achievement",    SEC_ADMINISTRATOR,  false,
                    {
                        { "player",     SEC_ADMINISTRATOR,  true,   &HandleSupportAddAchPlrCommand,      },
                        { "account",    SEC_ADMINISTRATOR,  true,   &HandleSupportAddAchPlrCommand,      },
                        { "guild",      SEC_ADMINISTRATOR,  true,   &HandleSupportAddAchGuildCommand,    },
                    } },
                    { "quest",          SEC_ADMINISTRATOR,  false,
                    {
                        { "new",        SEC_ADMINISTRATOR,  true,   &HandleSupportAddQuestCommand,       },
                        { "complete",   SEC_ADMINISTRATOR,  true,   &HandleSupportAddQuestCompCommand,   },
                        { "reward",     SEC_ADMINISTRATOR,  true,   &HandleSupportAddQuestRewCommand,    },
                    } },
                } },
                { "check",              SEC_ADMINISTRATOR,  false,
                {
                    { "spell",          SEC_ADMINISTRATOR,  true,   &HandleSupportCheckSpellCommand,     },
                    { "skill",          SEC_ADMINISTRATOR,  true,   &HandleSupportCheckSkillCommand,     },
                    { "item",           SEC_ADMINISTRATOR,  true,   &HandleSupportCheckItemCommand,      },
                    { "itemacc",        SEC_ADMINISTRATOR,  true,   &HandleSupportCheckItemAccCommand,   },
                    { "title",          SEC_ADMINISTRATOR,  true,   &HandleSupportCheckTitleCommand,     },
                    { "achievement",    SEC_ADMINISTRATOR,  false,
                    {
                        { "player",     SEC_ADMINISTRATOR,  true,   &HandleSupportCheckAchPlrCommand,    },
                        { "account",    SEC_ADMINISTRATOR,  true,   &HandleSupportCheckAchAccCommand,    },
                        { "guild",      SEC_ADMINISTRATOR,  true,   &HandleSupportCheckAchGuildCommand,  },
                    } },
                    { "mail",           SEC_ADMINISTRATOR,  true,   &HandleSupportCheckMailCommand,      },
                    { "mailacc",        SEC_ADMINISTRATOR,  true,   &HandleSupportCheckMailAccCommand,   },
                    { "professions",    SEC_ADMINISTRATOR,  true,   &HandleSupportCheckProfCommand,      },
                    { "aura",           SEC_ADMINISTRATOR,  true,   &HandleSupportCheckAuraCommand,      },
                    { "auras",          SEC_ADMINISTRATOR,  true,   &HandleSupportCheckAurasCommand,     },
                    { "boost",          SEC_ADMINISTRATOR,  true,   &HandleSupportCheckBoostCommand,     },
                    { "services",       SEC_ADMINISTRATOR,  false,
                    {
                        { "active",     SEC_ADMINISTRATOR,  true,   &HandleSupportCheckServActCommand,   },
                        { "executed",   SEC_ADMINISTRATOR,  true,   &HandleSupportCheckServExectCommand, },
                    } },
                    { "quest",          SEC_ADMINISTRATOR,  false,
                    {
                        { "status",     SEC_ADMINISTRATOR,  true,   &HandleSupportCheckQuestCommand,     },
                        { "active",     SEC_ADMINISTRATOR,  true,   &HandleSupportCheckQuestActCommand,  },
                        { "rewarded",   SEC_ADMINISTRATOR,  true,   &HandleSupportCheckQuestRewCommand,  },
                    } },
                    { "money",          SEC_ADMINISTRATOR,  true,   &HandleSupportCheckMoneyCommand,     },
                    { "moneyacc",       SEC_ADMINISTRATOR,  true,   &HandleSupportCheckMoneyAccCommand,  },
                    { "moneyguild",     SEC_ADMINISTRATOR,  true,   &HandleSupportCheckMoneyGuildCommand,},
                    { "reputation",     SEC_ADMINISTRATOR,  true,   &HandleSupportCheckRepCommand,       },
                    { "reputations",    SEC_ADMINISTRATOR,  true,   &HandleSupportCheckRepsCommand,      },
                    { "pvprating",      SEC_ADMINISTRATOR,  true,   &HandleSupportCheckPVPCommand,       },
                    { "currency",       SEC_ADMINISTRATOR,  true,   &HandleSupportCheckCurrencyCommand,  },
                    { "currencies",     SEC_ADMINISTRATOR,  true,   &HandleSupportCheckCurrsCommand,     },
                } },
                { "remove",             SEC_ADMINISTRATOR,  false,
                {
                    { "spell",          SEC_ADMINISTRATOR,  true,   &HandleSupportRemoveSpellCommand,    },
                    { "skill",          SEC_ADMINISTRATOR,  true,   &HandleSupportRemoveSkillCommand,    },
                    { "item",           SEC_ADMINISTRATOR,  true,   &HandleSupportRemoveItemCommand,     },
                    { "title",          SEC_ADMINISTRATOR,  true,   &HandleSupportRemoveTitleCommand,    },
                    { "achievement",    SEC_ADMINISTRATOR,  false,
                    {
                        { "player",     SEC_ADMINISTRATOR,  true,   &HandleSupportRemAchPlrCommand,      },
                        { "account",    SEC_ADMINISTRATOR,  true,   &HandleSupportRemAchAccCommand,      },
                        { "guild",      SEC_ADMINISTRATOR,  true,   &HandleSupportRemAchGuildCommand,    },
                    } },
                    { "aura",           SEC_ADMINISTRATOR,  true,   &HandleSupportRemoveAuraCommand,     },
                    { "quest",          SEC_ADMINISTRATOR,  true,   &HandleSupportRemoveQuestCommand,    },
                    { "cache",          SEC_ADMINISTRATOR,  true,   &HandleSupportRemoveCacheCommand,    },
                } },

                { "change",             SEC_ADMINISTRATOR,  false,
                {
                    { "money",          SEC_ADMINISTRATOR,  true,   &HandleSupportChangeMoneyCommand,    },
                    { "reputation",     SEC_ADMINISTRATOR,  true,   &HandleSupportChangeRepCommand,      },
                    { "pvprating",      SEC_ADMINISTRATOR,  true,   &HandleSupportChangePVPCommand,      },
                    { "pvpmmrrating",   SEC_ADMINISTRATOR,  true,   &HandleSupportChangePVPMMRCommand,   },
                    { "currency",       SEC_ADMINISTRATOR,  true,   &HandleSupportChangeCurrencyCommand, },
                } },
            } },
        };
        return commandTable;
    }

    // spells support
    static bool HandleSupportAddSpellCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer spellTok{ (char*)tok[1], ',' };
        if (spellTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& str : spellTok)
            {
                uint32 spellId = handler->extractSpellIdFromLink((char*)str);
                if (!spellId)
                {
                    handler->PSendSysMessage("Cant't get spell id.");
                    continue;
                }

                if (auto spell = sSpellMgr->GetSpellInfo(spellId))
                {
                    if (!player->HasSpell(spell->Id))
                    {
                        player->LearnSpell(spell->Id, false);
                        handler->PSendSysMessage("Spell %u added to player %s (guid: %u).", spell->Id, handler->GetNameLink(player).c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) already has spell %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), spell->Id);
                }
                else
                    handler->PSendSysMessage("Error. Spell %u not found.", spellId);
            }
            player->SaveToDB();
        }
        else
        {
            CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
            for (auto&& str : spellTok)
            {
                uint32 spellId = handler->extractSpellIdFromLink((char*)str);
                if (!spellId)
                {
                    handler->PSendSysMessage("Cant't get spell id.");
                    continue;
                }

                if (auto spell = sSpellMgr->GetSpellInfo(spellId))
                {
                    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_SPELL);
                    stmt->setUInt32(0, guid.GetCounter());
                    stmt->setUInt32(1, spell->Id);
                    if (!CharacterDatabase.Query(stmt))
                    {
                        stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAR_SPELL);
                        stmt->setUInt32(0, guid.GetCounter());
                        stmt->setUInt32(1, spell->Id);
                        stmt->setUInt32(2, 1);
                        stmt->setUInt32(3, 0);
                        trans->Append(stmt);

                        handler->PSendSysMessage("Spell %u added to player %s (guid: %u).", spell->Id, name.c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid %u) already has spell %u.", name.c_str(), guid.GetCounter(), spell->Id);
                }
                else
                    handler->PSendSysMessage("Error. Spell %u not found.", spellId);
            }
            CharacterDatabase.CommitTransaction(trans);
        }
        return true;
    }

    static bool HandleSupportCheckSpellCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer spellTok{ (char*)tok[1], ',' };
        if (spellTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& itr : spellTok)
            {
                uint32 spellId = handler->extractSpellIdFromLink((char*)itr);
                if (!spellId)
                {
                    handler->PSendSysMessage("Cant't get spell id.");
                    continue;
                }

                if (auto spell = sSpellMgr->GetSpellInfo(spellId))
                {
                    if (player->HasSpell(spell->Id))
                        handler->PSendSysMessage("Player %s (guid: %u) has spell %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), spell->Id);
                    else
                        handler->PSendSysMessage("Player %s (guid: %u)  hasn't spell %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), spell->Id);
                }
                else
                    handler->PSendSysMessage("Error. Spell %u not found.", spellId);
            }
        }
        else
        {
            for (auto&& str : spellTok)
            {
                uint32 spellId = handler->extractSpellIdFromLink((char*)str);
                if (!spellId)
                {
                    handler->PSendSysMessage("Cant't get spell id.");
                    continue;
                }

                if (auto spell = sSpellMgr->GetSpellInfo(spellId))
                {
                    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_SPELL);
                    stmt->setUInt32(0, guid.GetCounter());
                    stmt->setUInt32(1, spell->Id);
                    if (CharacterDatabase.Query(stmt))
                        handler->PSendSysMessage("Player %s (guid: %u) has spell %u.", name.c_str(), guid.GetCounter(), spell->Id);
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) hasn't spell %u.", name.c_str(), guid.GetCounter(), spell->Id);
                }
                else
                    handler->PSendSysMessage("Error. Spell %u not found.", spellId);
            }
        }
        return true;
    }

    static bool HandleSupportRemoveSpellCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer spellTok{ (char*)tok[1], ',' };
        if (spellTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& str : spellTok)
            {
                uint32 spellId = handler->extractSpellIdFromLink((char*)str);
                if (!spellId)
                {
                    handler->PSendSysMessage("Cant't get spell id.");
                    continue;
                }

                if (auto spell = sSpellMgr->GetSpellInfo(spellId))
                {
                    if (player->HasSpell(spell->Id))
                    {
                        player->RemoveSpell(spell->Id, false);
                        handler->PSendSysMessage("Spell %u removed from player %s (guid: %u).", spell->Id, handler->GetNameLink(player).c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) did not know spell %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), spell->Id);
                }
                else
                    handler->PSendSysMessage("Error. Spell %u not found.", spellId);
            }
            player->SaveToDB();
        }
        else
        {
            CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
            for (auto&& str : spellTok)
            {
                uint32 spellId = handler->extractSpellIdFromLink((char*)str);
                if (!spellId)
                {
                    handler->PSendSysMessage("Cant't get spell id.");
                    continue;
                }

                if (auto spell = sSpellMgr->GetSpellInfo(spellId))
                {
                    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_SPELL);
                    stmt->setUInt32(0, guid.GetCounter());
                    stmt->setUInt32(1, spell->Id);
                    if (CharacterDatabase.Query(stmt))
                    {
                        stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_SPELL_BY_SPELL);
                        stmt->setUInt32(0, spell->Id);
                        stmt->setUInt32(1, guid.GetCounter());
                        trans->Append(stmt);

                        handler->PSendSysMessage("Spell %u added removed from player %s (guid: %u).", spell->Id, name.c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) did not know spell %u.", name.c_str(), guid.GetCounter(), spell->Id);
                }
                else
                    handler->PSendSysMessage("Error. Spell %u not found.", spellId);
            }
            CharacterDatabase.CommitTransaction(trans);
        }
        return true;
    }
    // spells support end

    // skills support
    static bool HandleSupportAddSkillCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer skillTok{ (char*)tok[1], ',' };
        if (skillTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& itr : skillTok)
            {
                Tokenizer skillData{ itr, ':' };
                uint32 skillId = 0;
                uint32 skillValue = 0;
                if (skillData.size() == 2)
                {
                    skillId = atoi(skillData[0]);
                    skillValue = atoi(skillData[1]);
                }
                else if (skillData.size() == 1)
                {
                    skillId = atoi(skillData[0]);
                    skillValue = 1;
                }
                else
                    handler->PSendSysMessage("Error. Incorrect skill values.");

                if (auto skill = sSkillLineStore.LookupEntry(skillId))
                {
                    if (!player->HasSkill(skill->id))
                    {
                        player->SetSkill(skill->id, player->GetSkillStep(skill->id), skillValue, skillValue);
                        handler->PSendSysMessage("Skill %u (value: %u) added to player %s (guid: %u).", skill->id, skillValue, handler->GetNameLink(player).c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) already has skill %u.", handler->GetNameLink(player).c_str(),guid.GetCounter(), skill->id);
                }
                else
                    handler->PSendSysMessage("Error. Skill %u not found.", skillId);
            }
            player->SaveToDB();
        }
        else
        {
            CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
            for (auto&& itr : skillTok)
            {
                Tokenizer skillData{ itr, ':' };
                uint32 skillId = 0;
                uint32 skillValue = 0;
                if (skillData.size() == 2)
                {
                    skillId = atoi(skillData[0]);
                    skillValue = atoi(skillData[1]);
                }
                else if (skillData.size() == 1)
                {
                    skillId = atoi(skillData[0]);
                    skillValue = 1;
                }
                else
                    handler->PSendSysMessage("Error. Incorrect skill values.");

                if (auto skill = sSkillLineStore.LookupEntry(skillId))
                {
                    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_SKILL_BOOST);
                    stmt->setUInt32(0, guid.GetCounter());
                    stmt->setUInt32(1, skill->id);
                    if (!CharacterDatabase.Query(stmt))
                    {
                        stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAR_SKILLS);
                        stmt->setUInt32(0, guid.GetCounter());
                        stmt->setUInt32(1, skill->id);
                        stmt->setUInt32(2, skillValue);
                        stmt->setUInt32(3, skillValue);
                        trans->Append(stmt);

                        handler->PSendSysMessage("Skill %u (value: %u) added to player %s (guid: %u).", skill->id, skillValue, name.c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) already has skill %u.", name.c_str(), guid.GetCounter(), skill->id);
                }
                else
                    handler->PSendSysMessage("Error. Skill %u not found.", skillId);
            }
            CharacterDatabase.CommitTransaction(trans);
        }
        return true;
    }

    static bool HandleSupportCheckSkillCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer skillTok{ (char*)tok[1], ',' };
        if (skillTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& skillId : skillTok)
            {
                if (auto skill = sSkillLineStore.LookupEntry(atoi(skillId)))
                {
                    if (player->HasSkill(skill->id))
                        handler->PSendSysMessage("Player %s (guid: %u) has skill %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), skill->id);
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) hasn't skill %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), skill->id);
                }
                else
                    handler->PSendSysMessage("Error. Skill %u not found.", atoi(skillId));
            }
        }
        else
        {
            for (auto&& skillId : skillTok)
            {
                if (auto skill = sSkillLineStore.LookupEntry(atoi(skillId)))
                {
                    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_SKILL_BOOST);
                    stmt->setUInt32(0, guid.GetCounter());
                    stmt->setUInt32(1, skill->id);
                    if (CharacterDatabase.Query(stmt))
                        handler->PSendSysMessage("Player %s (guid: %u) has skill %u.", name.c_str(), guid.GetCounter(), skill->id);
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) hasn't skill %u.", name.c_str(), guid.GetCounter(), skill->id);
                }
                else
                    handler->PSendSysMessage("Error. Skill %u not found.", atoi(skillId));
            }
        }
        return true;
    }

    static bool HandleSupportRemoveSkillCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer skillTok{ (char*)tok[1], ',' };
        if (skillTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& skillId : skillTok)
            {
                if (auto skill = sSkillLineStore.LookupEntry(atoi(skillId)))
                {
                    if (player->HasSkill(skill->id))
                    {
                        player->SetSkill(skill->id, 0, 0, 0);
                        handler->PSendSysMessage("Skill %u removed from player %s (guid: %u).", skill->id, handler->GetNameLink(player).c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid %u) did not has skill %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), skill->id);
                }
                else
                    handler->PSendSysMessage("Error. Skill %u not found.", atoi(skillId));
            }
            player->SaveToDB();
        }
        else
        {
            CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
            for (auto&& skillId : skillTok)
            {
                if (auto skill = sSkillLineStore.LookupEntry(atoi(skillId)))
                {
                    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_SKILL_BOOST);
                    stmt->setUInt32(0, guid.GetCounter());
                    stmt->setUInt32(1, skill->id);
                    if (CharacterDatabase.Query(stmt))
                    {
                        stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_SKILL_BY_SKILL);
                        stmt->setUInt32(0, guid.GetCounter());
                        stmt->setUInt32(1, skill->id);
                        trans->Append(stmt);

                        handler->PSendSysMessage("Skill %u added removed from player %s (guid: %u).", skill->id, name.c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid %u) did not know skill %u.", name.c_str(), guid.GetCounter(), skill->id);
                }
                else
                    handler->PSendSysMessage("Error. Skill %u not found.", atoi(skillId));
            }
            CharacterDatabase.CommitTransaction(trans);
        }
        return true;
    }
    // skills support end

    // items support
    static bool HandleSupportAddItemCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer itemTok{ (char*)tok[1], ',' };
        if (itemTok.size() == 0)
            return false;

        std::string items;
        for (auto&& itr : itemTok)
        {
            items += itr;
            items += " ";
        }

        std::string text = " \"Support\" \"Restored items.\" ";
        std::string command = ".send items " + name + text + items;
        handler->ParseCommands(command.c_str());
        return true;
    }

    static bool HandleSupportCheckItemCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer itemTok{ (char*)tok[1], ',' };
        if (itemTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& itemId : itemTok)
            {
                if (auto item = sObjectMgr->GetItemTemplate(atoi(itemId)))
                {
                    // check bank and inventory
                    if (uint32 count = player->GetItemCount(item->ItemId, true))
                        handler->PSendSysMessage("Player %s (guid %u) has item %s (%u, count: %u) in inventory or bank.", handler->GetNameLink(player).c_str(), guid.GetCounter(), item->Name1.c_str(), item->ItemId, count);
                    else
                        handler->PSendSysMessage("Player %s (guid %u) hasn't item %s (%u) in inventory.", handler->GetNameLink(player).c_str(), guid.GetCounter(), item->Name1.c_str(), item->ItemId);
                    // check mail
                    for (auto&& mail : player->GetMail())
                    {
                        for (uint8 i = 0; i < mail->items.size(); i++)
                        {
                            if (item->ItemId == mail->items[i].item_template)
                                handler->PSendSysMessage("Player %s (guid %u) has item %s (%u, guid: %u) in mail.", handler->GetNameLink(player).c_str(), guid.GetCounter(), item->Name1.c_str(), item->ItemId, mail->items[i].item_guid);
                        }
                    }
                    // check void storage
                    uint8 slot = 0;
                    if (player->GetVoidStorageItem(item->ItemId, slot))
                        handler->PSendSysMessage("Player %s (guid: %u) has item %s (%u, slot: %u) in void storage.", handler->GetNameLink(player).c_str(), guid.GetCounter(), item->Name1.c_str(), item->ItemId, slot);
                }
                else
                    handler->PSendSysMessage("Error. Item %u not found.", atoi(itemId));
            }
        }
        else
        {
            for (auto&& itemId : itemTok)
            {
                if (auto item = sObjectMgr->GetItemTemplate(atoi(itemId)))
                {
                    // check bank and inventory
                    QueryResult result = CharacterDatabase.PQuery("SELECT sum(count) FROM item_instance WHERE itemEntry = %u AND owner_guid = %u", item->ItemId, guid.GetCounter());
                    if (result && (*result)[0].GetUInt32())
                        handler->PSendSysMessage("Player %s (guid: %u) has item %s (%u, count: %u) in inventory or bank.", name.c_str(), guid.GetCounter(), item->Name1.c_str(), item->ItemId, (*result)[0].GetUInt32());
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) hasn't item %s (%u) in inventory.", name.c_str(), guid.GetCounter(), item->Name1.c_str(), item->ItemId);
                    // check mail
                    if (QueryResult resultMail = CharacterDatabase.PQuery("SELECT item_guid FROM mail_items WHERE receiver = %u", guid.GetCounter()))
                    {
                        do
                        {
                            Field* fields = resultMail->Fetch();
                            uint32 itemGuid = fields[0].GetUInt32();
                            if (QueryResult resultItem = CharacterDatabase.PQuery("SELECT itemEntry, count FROM item_instance WHERE guid = %u", itemGuid))
                                if ((*resultItem)[0].GetUInt32() == item->ItemId)
                                    handler->PSendSysMessage("Player %s (guid: %u) has item %s (%u, guid: %u, count %u) in mail.", name.c_str(), guid.GetCounter(), item->Name1.c_str(), item->ItemId, itemGuid, (*resultItem)[1].GetUInt32());
                        } while (resultMail->NextRow());
                    }
                    // check void storage
                    if (QueryResult resultVoid = CharacterDatabase.PQuery("SELECT slot FROM character_void_storage WHERE itemEntry = %u AND playerGuid = %u", guid.GetCounter()))
                        handler->PSendSysMessage("Player %s (guid: %u) has item %s (%u, slot: %u) in void storage.", name.c_str(), guid.GetCounter(), item->Name1.c_str(), item->ItemId, (*resultVoid)[0].GetUInt32());
                }
                else
                    handler->PSendSysMessage("Error. Item %u not found.", item->ItemId);
            }
        }
        return true;
    }

    static bool HandleSupportCheckItemAccCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        std::string accountName = tok[0];
        if (!AccountMgr::normalizeString(accountName))
        {
            handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, accountName.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 accountId = AccountMgr::GetId(accountName);
        if (!accountId)
        {
            handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, accountName.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        std::vector<ObjectGuid> playersList;
        if (QueryResult result = CharacterDatabase.PQuery("SELECT guid FROM characters WHERE account = %u", accountId))
        {
            do
            {
                Field* fields = result->Fetch();
                uint32 guidLow = fields[0].GetUInt32();
                playersList.emplace_back(HighGuid::Player, guidLow);
            } while (result->NextRow());
        }
        else
            handler->PSendSysMessage("Account empty.");

        Tokenizer itemTok{ (char*)tok[1], ',' };
        if (itemTok.size() == 0)
            return false;

        for (auto&& guid : playersList)
        {
            Player* player = sObjectMgr->GetPlayerByLowGUID(guid);
            std::string name = "unknown";
            sObjectMgr->GetPlayerNameByGUID(guid, name);
            if (player)
            {
                for (auto&& itemId : itemTok)
                {
                    if (auto item = sObjectMgr->GetItemTemplate(atoi(itemId)))
                    {
                        // check bank and inventory
                        if (uint32 count = player->GetItemCount(item->ItemId, true))
                            handler->PSendSysMessage("Player %s (guid %u) has item %s (%u, count: %u) in inventory or bank.", handler->GetNameLink(player).c_str(), guid.GetCounter(), item->Name1.c_str(), item->ItemId, count);
                        else
                            handler->PSendSysMessage("Player %s (guid %u) hasn't item %s (%u) in inventory.", handler->GetNameLink(player).c_str(), guid.GetCounter(), item->Name1.c_str(), item->ItemId);
                        // check mail
                        for (auto&& mail : player->GetMail())
                        {
                            for (uint8 i = 0; i < mail->items.size(); i++)
                            {
                                if (item->ItemId == mail->items[i].item_template)
                                    handler->PSendSysMessage("Player %s (guid %u) has item %s (%u, guid: %u) in mail.", handler->GetNameLink(player).c_str(), guid.GetCounter(), item->Name1.c_str(), item->ItemId, mail->items[i].item_guid);
                            }
                        }
                        // check void storage
                        uint8 slot = 0;
                        if (player->GetVoidStorageItem(item->ItemId, slot))
                            handler->PSendSysMessage("Player %s (guid: %u) has item %s (%u, slot: %u) in void storage.", handler->GetNameLink(player).c_str(), guid.GetCounter(), item->Name1.c_str(), item->ItemId, slot);
                    }
                    else
                        handler->PSendSysMessage("Error. Item %u not found.", atoi(itemId));
                }
            }
            else
            {
                for (auto&& itemId : itemTok)
                {
                    if (auto item = sObjectMgr->GetItemTemplate(atoi(itemId)))
                    {
                        // check bank and inventory
                        QueryResult result = CharacterDatabase.PQuery("SELECT sum(count) FROM item_instance WHERE itemEntry = %u AND owner_guid = %u", item->ItemId, guid.GetCounter());
                        if (result && (*result)[0].GetUInt32())
                            handler->PSendSysMessage("Player %s (guid: %u) has item %s (%u, count: %u) in inventory or bank.", name.c_str(), guid.GetCounter(), item->Name1.c_str(), item->ItemId, (*result)[0].GetUInt32());
                        else
                            handler->PSendSysMessage("Player %s (guid: %u) hasn't item %s (%u) in inventory.", name.c_str(), guid.GetCounter(), item->Name1.c_str(), item->ItemId);
                        // check mail
                        if (QueryResult resultMail = CharacterDatabase.PQuery("SELECT item_guid FROM mail_items WHERE receiver = %u", guid.GetCounter()))
                        {
                            do
                            {
                                Field* fields = resultMail->Fetch();
                                uint32 itemGuid = fields[0].GetUInt32();
                                if (QueryResult resultItem = CharacterDatabase.PQuery("SELECT itemEntry, count FROM item_instance WHERE guid = %u", itemGuid))
                                    if ((*resultItem)[0].GetUInt32() == item->ItemId)
                                        handler->PSendSysMessage("Player %s (guid: %u) has item %s (%u, guid: %u, count %u) in mail.", name.c_str(), guid.GetCounter(), item->Name1.c_str(), item->ItemId, itemGuid, (*resultItem)[1].GetUInt32());
                            } while (resultMail->NextRow());
                        }
                        // check void storage
                        if (QueryResult resultVoid = CharacterDatabase.PQuery("SELECT slot FROM character_void_storage WHERE itemEntry = %u AND playerGuid = %u", guid.GetCounter()))
                            handler->PSendSysMessage("Player %s (guid: %u) has item %s (%u, slot: %u) in void storage.", name.c_str(), guid.GetCounter(), item->Name1.c_str(), item->ItemId, (*resultVoid)[0].GetUInt32());
                    }
                    else
                        handler->PSendSysMessage("Error. Item %u not found.", item->ItemId);
                }
            }
        }
        return true;
    }

    static bool HandleSupportRemoveItemCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer itemTok{ (char*)tok[1], ',' };
        if (itemTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& itemId : itemTok)
            {
                if (auto item = sObjectMgr->GetItemTemplate(atoi(itemId)))
                {
                    // check bank and inventory
                    if (uint32 count = player->GetItemCount(item->ItemId, true))
                    {
                        player->DestroyItemCount(item->ItemId, count, true, false);
                        handler->PSendSysMessage("Item %s (%u, count %u) removed from player %s (guid: %u), source - inventory.", item->Name1.c_str(), item->ItemId, count, handler->GetNameLink(player).c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) hasn't item %s (%u) in inventory.", handler->GetNameLink(player).c_str(), guid.GetCounter(), item->Name1.c_str(), item->ItemId);
                    // check mail
                    for (auto&& mail : player->GetMail())
                    {
                        for (uint8 i = 0; i < mail->items.size(); i++)
                        {
                            if (item->ItemId == mail->items[i].item_template)
                            {
                                mail->RemoveItem(mail->items[i].item_guid);
                                handler->PSendSysMessage("Item %u (guid %u) removed from player %s (guid: %u), source - mail.", item->ItemId, mail->items[i].item_guid, handler->GetNameLink(player).c_str(), guid.GetCounter());
                            }
                        }
                    }
                    // check void storage
                    uint8 slot = 0;
                    if (player->GetVoidStorageItem(item->ItemId, slot))
                    {
                        player->DeleteVoidStorageItem(slot);
                        handler->PSendSysMessage("Item %s (%u, slot %u) removed from player %s (guid: %u), source - void storage.", item->Name1.c_str(), item->ItemId, slot, handler->GetNameLink(player).c_str(), guid.GetCounter());
                    }
                }
                else
                    handler->PSendSysMessage("Error. Item %u not found.", atoi(itemId));
            }
            player->SaveToDB();
        }
        else
        {
            for (auto&& itemId : itemTok)
            {
                if (auto item = sObjectMgr->GetItemTemplate(atoi(itemId)))
                {
                    // check bank and inventory
                    if (QueryResult result = CharacterDatabase.PQuery("SELECT guid, sum(count) FROM item_instance WHERE itemEntry = %u AND owner_guid = %u", item->ItemId, guid.GetCounter()))
                    {
                        CharacterDatabase.PExecute("DELETE FROM item_instance WHERE guid = %u", (*result)[0].GetUInt32());
                        handler->PSendSysMessage("Item %d (%s, count %u) removed from player %s (guid: %u), source - inventory.", item->ItemId, item->Name1.c_str(), (*result)[1].GetUInt32(), name.c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) hasn't item %s (%u).", name.c_str(), guid.GetCounter(), item->Name1.c_str(), item->ItemId);
                    // check mail
                    if (QueryResult resultMail = CharacterDatabase.PQuery("SELECT item_guid FROM mail_items WHERE receiver = %u", guid.GetCounter()))
                    {
                        do
                        {
                            Field* fields = resultMail->Fetch();
                            uint32 itemGuid = fields[0].GetUInt32();
                            if (QueryResult resultItem = CharacterDatabase.PQuery("SELECT itemEntry, count FROM item_instance WHERE guid = %u", itemGuid))
                            {
                                if ((*resultItem)[0].GetUInt32() == item->ItemId)
                                {
                                    CharacterDatabase.PExecute("DELETE FROM mail_items WHERE item_guid = %u AND receiver = %u", itemGuid, guid.GetCounter());
                                    CharacterDatabase.PExecute("DELETE FROM item_instance WHERE guid = %u", itemGuid);
                                    handler->PSendSysMessage("Item %s (%u, guid %u) removed from player %s (guid: %u), source - mail.", item->Name1.c_str(), item->ItemId, itemGuid, name.c_str(), guid.GetCounter());
                                }
                            }
                        } while (resultMail->NextRow());
                    }
                    // check void storage
                    if (QueryResult resultVoid = CharacterDatabase.PQuery("SELECT slot FROM character_void_storage WHERE itemEntry = %u AND playerGuid = %u", guid.GetCounter()))
                    {
                        CharacterDatabase.PExecute("DELETE FROM character_void_storage WHERE itemEntry = %u AND guid = %u AND slot = %u", item->ItemId, guid.GetCounter(), (*resultVoid)[0].GetUInt32());
                        handler->PSendSysMessage("Item %s, (%u, slot %u) removed from player %s (guid: %u), source - void storage.", item->Name1.c_str(), item->ItemId, (*resultVoid)[0].GetUInt32(), name.c_str(), guid.GetCounter());
                    }
                }
                else
                    handler->PSendSysMessage("Error. Item %u not found.", item->ItemId);
            }
        }
        return true;
    }
    // items support end

    // titles support
    static bool HasTitle(ObjectGuid guid, const CharTitlesEntry* title)
    {
        if (title->bit_index > MAX_TITLE_INDEX)
            return false;

        uint32 fieldIndexOffset = title->bit_index / 32;
        uint32 flag = 1 << (title->bit_index % 32);

        if (QueryResult result = CharacterDatabase.PQuery("SELECT knownTitles FROM characters WHERE guid = %u", guid.GetCounter()))
        {
            Tokenizer tok{ (*result)[0].GetString(), ' ' };
            if (tok.size() != 8)
                return false;

            return (atoi(tok[fieldIndexOffset]) & flag) != 0;
        }
        return false;
    }
    static bool SetTitle(ObjectGuid guid, std::string name, const CharTitlesEntry* title, ChatHandler* handler, bool lost)
    {
        uint32 fieldIndexOffset = title->bit_index / 32;
        uint32 flag = 1 << (title->bit_index % 32);

        if (QueryResult result = CharacterDatabase.PQuery("SELECT knownTitles FROM characters WHERE guid = %u", guid.GetCounter()))
        {
            Tokenizer tok{ (*result)[0].GetString(), ' ' };
            if (tok.size() != 8)
                return false;

            uint32 oldval = atoi(tok[fieldIndexOffset]);
            uint32 newval = 0;
            if (!lost)
                newval = oldval | flag;
            else
                newval = oldval & ~flag;

            tok[fieldIndexOffset] = std::to_string(newval).c_str();

            std::string titles = "\"";
            for (uint32 index = 0; index < 8; index++)
            {
                titles += tok[index];
                if (index != 7)
                    titles += " ";
                else
                    titles += "\"";
            }
            CharacterDatabase.PExecute("UPDATE characters SET knownTitles = %s, chosenTitle = 0 WHERE guid = %u", titles.c_str(), guid.GetCounter());
            return true;
        }
        return false;
    }

    static bool HandleSupportAddTitleCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer titleTok{ (char*)tok[1], ',' };
        if (titleTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& titleId : titleTok)
            {
                if (auto title = sCharTitlesStore.LookupEntry(atoi(titleId)))
                {
                    if (!player->HasTitle(title))
                    {
                        player->SetTitle(title, false);
                        handler->PSendSysMessage("Title %u added to player %s (guid: %u).", title->ID, handler->GetNameLink(player).c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid %u) already has title %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), title->ID);
                }
                else
                    handler->PSendSysMessage("Error. Title %u not found.", title->ID);
            }
            player->SaveToDB();
        }
        else
        {
            for (auto&& titleId : titleTok)
            {
                if (auto title = sCharTitlesStore.LookupEntry(atoi(titleId)))
                {
                    if (!HasTitle(guid, title))
                    {
                        if (SetTitle(guid, name, title, handler, false))
                            handler->PSendSysMessage("Title %u added to player %s (guid: %u).", title->ID, name.c_str(), guid.GetCounter());
                        else
                            handler->SendSysMessage("Unknown error.");
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid %u) already has title %u.", name.c_str(), guid.GetCounter(), title->ID);
                }
                else
                    handler->PSendSysMessage("Error. Title %u not found.", title->ID);
            }
        }
        return true;
    }

    static bool HandleSupportCheckTitleCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer titleTok{ (char*)tok[1], ',' };
        if (titleTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& titleId : titleTok)
            {
                if (auto title = sCharTitlesStore.LookupEntry(atoi(titleId)))
                {
                    if (player->HasTitle(title))
                        handler->PSendSysMessage("Player %s (guid: %u) has title %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), title->ID);
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) hasn;t title %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), title->ID);
                }
                else
                    handler->PSendSysMessage("Error. Title %u not found.", title->ID);
            }
            player->SaveToDB();
        }
        else
        {
            for (auto&& titleId : titleTok)
            {
                if (auto title = sCharTitlesStore.LookupEntry(atoi(titleId)))
                {
                    if (HasTitle(guid, title))
                        handler->PSendSysMessage("Player %s (guid: %u) has title %u.", name.c_str(), guid.GetCounter(), title->ID);
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) hasn;t title %u.", name.c_str(), guid.GetCounter(), title->ID);
                }
                else
                    handler->PSendSysMessage("Error. Title %u not found.", title->ID);
            }
        }
        return true;
    }

    static bool HandleSupportRemoveTitleCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer titleTok{ (char*)tok[1], ',' };
        if (titleTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& titleId : titleTok)
            {
                if (auto title = sCharTitlesStore.LookupEntry(atoi(titleId)))
                {
                    if (player->HasTitle(title))
                    {
                        player->SetTitle(title, true);
                        handler->PSendSysMessage("Title %u removed from player %s (guid: %u).", title->ID, handler->GetNameLink(player).c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) hasn't title %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), title->ID);
                }
                else
                    handler->PSendSysMessage("Error. Title %u not found.", title->ID);
            }
            player->SaveToDB();
        }
        else
        {
            for (auto&& titleId : titleTok)
            {
                if (auto title = sCharTitlesStore.LookupEntry(atoi(titleId)))
                {
                    if (HasTitle(guid, title))
                    {
                        if (SetTitle(guid, name, title, handler, true))
                            handler->PSendSysMessage("Title %u removed from player %s (guid: %u).", title->ID, name.c_str(), guid.GetCounter());
                        else
                            handler->SendSysMessage("Unknown error.");
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid %u) hasn't title %u.", name.c_str(), guid.GetCounter(), title->ID);
                }
                else
                    handler->PSendSysMessage("Error. Title %u not found.", title->ID);
            }
        }
        return true;
    }
    // titles support end

    // achievements support
    static bool HandleSupportAddAchPlrCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer achievTok{ (char*)tok[1], ',' };
        if (achievTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& achievId : achievTok)
            {
                if (auto ach = sAchievementStore.LookupEntry(atoi(achievId)))
                {
                    if (!player->HasAchieved(ach->ID), true)
                    {
                        player->GetAchievementMgr().CompletedAchievement(ach, player);
                        handler->PSendSysMessage("Achievement %u added to player %s (guid: %u).", ach->ID, handler->GetNameLink(player).c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) already has achievement %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), ach->ID);
                }
                else
                    handler->PSendSysMessage("Error. Achievement %u not found.", atoi(achievId));
            }
            player->SaveToDB();
        }
        else
        {
            CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
            for (auto&& achievId : achievTok)
            {
                if (auto ach = sAchievementStore.LookupEntry(atoi(achievId)))
                {
                    if (!CharacterDatabase.PQuery("SELECT * FROM character_achievement WHERE guid = %u AND achievement = %u", guid.GetCounter(), ach->ID))
                    {
                        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAR_ACHIEVEMENT);
                        stmt->setUInt32(0, guid.GetCounter());
                        stmt->setUInt32(1, ach->ID);
                        stmt->setUInt32(2, time(nullptr));
                        trans->Append(stmt);

                        handler->PSendSysMessage("Achievement %u added to player %s (guid: %u).", ach->ID, name.c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) already has achievement %u.", name.c_str(), guid.GetCounter(), ach->ID);
                }
                else
                    handler->PSendSysMessage("Error. Achievement %u not found.", atoi(achievId));
            }
            CharacterDatabase.CommitTransaction(trans);
        }
        return true;
    }

    static bool HandleSupportCheckAchPlrCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer achievTok{ (char*)tok[1], ',' };
        if (achievTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& achievId : achievTok)
            {
                if (auto ach = sAchievementStore.LookupEntry(atoi(achievId)))
                {
                    if (player->HasAchieved(ach->ID, true))
                        handler->PSendSysMessage("Player %s (guid: %u) has achievement %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), ach->ID);
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) hasn't achievement %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), ach->ID);
                }
                else
                    handler->PSendSysMessage("Error. Achievement %u not found.", atoi(achievId));
            }
        }
        else
        {
            for (auto&& achievId : achievTok)
            {
                if (auto ach = sAchievementStore.LookupEntry(atoi(achievId)))
                {
                    if (CharacterDatabase.PQuery("SELECT * FROM character_achievement WHERE guid = %u AND achievement = %u", guid.GetCounter(), ach->ID))
                        handler->PSendSysMessage("Player %s (guid: %u) has achievement %u.", name.c_str(), guid.GetCounter(), ach->ID);
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) hasn't achievement %u.", name.c_str(), guid.GetCounter(), ach->ID);
                }
                else
                    handler->PSendSysMessage("Error. Achievement %u not found.", atoi(achievId));
            }
        }
        return true;
    }

    static bool HandleSupportRemAchPlrCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer achievTok{ (char*)tok[1], ',' };
        if (achievTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& achievId : achievTok)
            {
                if (auto ach = sAchievementStore.LookupEntry(atoi(achievId)))
                {
                    if (player->HasAchieved(ach->ID, true))
                    {
                        player->GetAchievementMgr().RemoveAchievement(ach);
                        handler->PSendSysMessage("Achievement %u removed from player %s (guid: %u).", ach->ID, handler->GetNameLink(player).c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) did not know achievement %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), ach->ID);
                }
                else
                    handler->PSendSysMessage("Error. Achievement %u not found.", atoi(achievId));
            }
            player->SaveToDB();
        }
        else
        {
            CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
            for (auto&& achievId : achievTok)
            {
                if (auto ach = sAchievementStore.LookupEntry(atoi(achievId)))
                {
                    if (CharacterDatabase.PQuery("SELECT * FROM character_achievement WHERE guid = %u AND achievement = %u", guid.GetCounter(), ach->ID))
                    {
                        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_ACHIEVEMENT_BY_ACHIEVEMENT);
                        stmt->setUInt32(0, ach->ID);
                        stmt->setUInt32(1, guid.GetCounter());
                        trans->Append(stmt);

                        handler->PSendSysMessage("Achievement %u added removed from player %s (guid: %u).", ach->ID, name.c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) did not know achievement %u.", name.c_str(), guid.GetCounter(), ach->ID);
                }
                else
                    handler->PSendSysMessage("Error. Achievement %u not found.", atoi(achievId));
            }
            CharacterDatabase.CommitTransaction(trans);
        }
        return true;
    }

    static bool HandleSupportAddAchAccCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer achievTok{ (char*)tok[1], ',' };
        if (achievTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& achievId : achievTok)
            {
                if (auto ach = sAchievementStore.LookupEntry(atoi(achievId)))
                {
                    if (!player->HasAchieved(ach->ID), false)
                    {
                        player->CompletedAchievement(ach);
                        handler->PSendSysMessage("Account achievement %u added to player %s (guid: %u).", ach->ID, handler->GetNameLink(player).c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) already has achievement %u on account.", handler->GetNameLink(player).c_str(), guid.GetCounter(), ach->ID);
                }
                else
                    handler->PSendSysMessage("Error. Achievement %u not found.", atoi(achievId));
            }
            player->SaveToDB();
        }
        else
        {
            CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
            QueryResult accountCheck = CharacterDatabase.PQuery("SELECT account FROM characters WHERE name = %s", name.c_str());
            uint32 account = (*accountCheck)[0].GetUInt32();
            for (auto&& achievId : achievTok)
            {
                if (auto ach = sAchievementStore.LookupEntry(atoi(achievId)))
                {
                    if (!CharacterDatabase.PQuery("SELECT * FROM account_achievement WHERE account = %u AND achievement = %u", account, ach->ID))
                    {
                        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_ACCOUNT_ACHIEVEMENT);
                        stmt->setUInt32(0, account);
                        stmt->setUInt32(1, ach->ID);
                        stmt->setUInt32(2, time(nullptr));
                        stmt->setUInt32(3, guid.GetCounter());
                        trans->Append(stmt);

                        handler->PSendSysMessage("Account achievement %u added to player %s (guid: %u).", ach->ID, name.c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) already has achievement %u on account.", name.c_str(), guid.GetCounter(), ach->ID);
                }
                else
                    handler->PSendSysMessage("Error. Achievement %u not found.", atoi(achievId));
            }
            CharacterDatabase.CommitTransaction(trans);
        }
        return true;
    }

    static bool HandleSupportCheckAchAccCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer achievTok{ (char*)tok[1], ',' };
        if (achievTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& achievId : achievTok)
            {
                if (auto ach = sAchievementStore.LookupEntry(atoi(achievId)))
                {
                    if (player->HasAchieved(ach->ID, false))
                        handler->PSendSysMessage("Player %s (guid: %u) has achievement %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), ach->ID);
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) hasn't achievement %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), ach->ID);
                }
                else
                    handler->PSendSysMessage("Error. Achievement %u not found.", atoi(achievId));
            }
        }
        else
        {
            QueryResult accountCheck = CharacterDatabase.PQuery("SELECT account FROM characters WHERE name = %s", name.c_str());
            uint32 account = (*accountCheck)[0].GetUInt32();
            for (auto&& achievId : achievTok)
            {
                if (auto ach = sAchievementStore.LookupEntry(atoi(achievId)))
                {
                    if (CharacterDatabase.PQuery("SELECT * FROM account_achievement WHERE account = %u AND achievement = %u", account, ach->ID))
                        handler->PSendSysMessage("Player %s (guid: %u) has achievement %u on account.", name.c_str(), guid.GetCounter(), ach->ID);
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) hasn't achievement %u on account.", name.c_str(), guid.GetCounter(), ach->ID);
                }
                else
                    handler->PSendSysMessage("Error. Achievement %u not found.", atoi(achievId));
            }
        }
        return true;
    }

    static bool HandleSupportRemAchAccCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer achievTok{ (char*)tok[1], ',' };
        if (achievTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& achievId : achievTok)
            {
                if (auto ach = sAchievementStore.LookupEntry(atoi(achievId)))
                {
                    if (player->HasAchieved(ach->ID, true))
                    {
                        player->GetAchievementMgr().RemoveAchievement(ach);
                        handler->PSendSysMessage("Achievement %u removed from player %s (guid: %u).", ach->ID, handler->GetNameLink(player).c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) did not know achievement %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), ach->ID);
                }
                else
                    handler->PSendSysMessage("Error. Achievement %u not found.", atoi(achievId));
            }
            player->SaveToDB();
        }
        else
        {
            CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
            for (auto&& achievId : achievTok)
            {
                if (auto ach = sAchievementStore.LookupEntry(atoi(achievId)))
                {
                    if (CharacterDatabase.PQuery("SELECT * FROM character_achievement WHERE guid = %u AND achievement = %u", guid.GetCounter(), ach->ID))
                    {
                        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_ACHIEVEMENT_BY_ACHIEVEMENT);
                        stmt->setUInt32(0, ach->ID);
                        stmt->setUInt32(1, guid.GetCounter());
                        trans->Append(stmt);

                        handler->PSendSysMessage("Achievement %u added removed from player %s (guid: %u).", ach->ID, name.c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) did not know achievement %u.", name.c_str(), guid.GetCounter(), ach->ID);
                }
                else
                    handler->PSendSysMessage("Error. Achievement %u not found.", atoi(achievId));
            }
            CharacterDatabase.CommitTransaction(trans);
        }
        return true;
    }

    static bool HandleSupportAddAchGuildCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 3)
            return false;

        Guild* guild = sGuildMgr->GetGuildByName(tok[0]);
        if (!guild)
        {
            handler->PSendSysMessage(LANG_COMMAND_COULDNOTFIND, tok[0]);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[1], &player, &guid, &name))
            return false;

        Tokenizer achievTok{ (char*)tok[2], ',' };
        if (achievTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& achievId : achievTok)
            {
                if (auto ach = sAchievementStore.LookupEntry(atoi(achievId)))
                {
                    if (!guild->HasAchieved(ach->ID))
                    {
                        guild->GetAchievementMgr().CompletedAchievement(ach, player);
                        handler->PSendSysMessage("Achievement %u added to guild %s (guid: %u).", ach->ID, guild->GetName().c_str(), guild->GetId());
                    }
                    else
                        handler->PSendSysMessage("Guild %s (guid: %u) already has achievement %u.", guild->GetName().c_str(), guild->GetId(), ach->ID);
                }
                else
                    handler->PSendSysMessage("Error. Achievement %u not found.", atoi(achievId));
            }
            player->SaveToDB();
        }
        else
        {
            CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
            for (auto&& achievId : achievTok)
            {
                if (auto ach = sAchievementStore.LookupEntry(atoi(achievId)))
                {
                    if (!CharacterDatabase.PQuery("SELECT * FROM guild_achievement WHERE guildId = %u AND achievement = %u", guild->GetId(), ach->ID))
                    {
                        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GUILD_ACHIEVEMENT);
                        stmt->setUInt32(0, guild->GetId());
                        stmt->setUInt32(1, ach->ID);
                        stmt->setUInt32(2, time(nullptr));
                        stmt->setString(3, "");
                        trans->Append(stmt);

                        handler->PSendSysMessage("Achievement %u will be added to guild %s (guid: %u) after restart.", ach->ID, guild->GetName().c_str(), guild->GetId());
                    }
                    else
                        handler->PSendSysMessage("Guild %s (guid: %u) already has achievement %u.", guild->GetName().c_str(), guild->GetId(), ach->ID);
                }
                else
                    handler->PSendSysMessage("Error. Achievement %u not found.", atoi(achievId));
            }
            CharacterDatabase.CommitTransaction(trans);
        }
        return true;
    }

    static bool HandleSupportCheckAchGuildCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Guild* guild = sGuildMgr->GetGuildByName(tok[0]);
        if (!guild)
        {
            handler->PSendSysMessage(LANG_COMMAND_COULDNOTFIND, tok[0]);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Tokenizer achievTok{ (char*)tok[1], ',' };
        if (achievTok.size() == 0)
            return false;

        for (auto&& achievId : achievTok)
        {
            if (auto ach = sAchievementStore.LookupEntry(atoi(achievId)))
            {
                if (guild->HasAchieved(ach->ID))
                    handler->PSendSysMessage("Guild %s (guid: %u) has achievement %u.", guild->GetName().c_str(), guild->GetId(), ach->ID);
                else
                    handler->PSendSysMessage("Guild %s (guid: %u) hasn't achievement %u.", guild->GetName().c_str(), guild->GetId(), ach->ID);
            }
            else
                handler->PSendSysMessage("Error. Achievement %u not found.", atoi(achievId));
        }
        return true;
    }

    static bool HandleSupportRemAchGuildCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Guild* guild = sGuildMgr->GetGuildByName(tok[0]);
        if (!guild)
        {
            handler->PSendSysMessage(LANG_COMMAND_COULDNOTFIND, tok[0]);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Tokenizer achievTok{ (char*)tok[1], ',' };
        if (achievTok.size() == 0)
            return false;

        for (auto&& achievId : achievTok)
        {
            if (auto ach = sAchievementStore.LookupEntry(atoi(achievId)))
            {
                if (guild->HasAchieved(ach->ID))
                {
                    guild->GetAchievementMgr().RemoveAchievement(ach);
                    handler->PSendSysMessage("Achievement %u removed from guild %s (guid: %u) - NOT IMPLEMENTED.", ach->ID, guild->GetName().c_str(), guild->GetId());
                }
                else
                    handler->PSendSysMessage("Error. Guild %s (guid: %u) already has achievement %u.", guild->GetName().c_str(), guild->GetId(), ach->ID);
            }
            else
                handler->PSendSysMessage("Error. Achievement %u not found.", atoi(achievId));
        }
        return true;
    }
    // achievements support end

    // aura support
    static bool HandleSupportCheckAuraCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer auraTok{ (char*)tok[1], ',' };
        if (auraTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& str : auraTok)
            {
                uint32 spellId = handler->extractSpellIdFromLink((char*)str);
                if (!spellId)
                {
                    handler->PSendSysMessage("Cant't get spell id.");
                    continue;
                }

                if (auto spell = sSpellMgr->GetSpellInfo(spellId))
                {
                    if (player->HasAura(spell->Id))
                        handler->PSendSysMessage("Player %s (guid: %u) has aura %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), spell->Id);
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) hasn't aura %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), spell->Id);
                }
                else
                    handler->PSendSysMessage("Error. Spell %u not found.", spellId);
            }
        }
        else
        {
            for (auto&& str : auraTok)
            {
                uint32 spellId = handler->extractSpellIdFromLink((char*)str);
                if (!spellId)
                {
                    handler->PSendSysMessage("Cant't get spell id.");
                    continue;
                }

                if (auto spell = sSpellMgr->GetSpellInfo(spellId))
                {
                    if (CharacterDatabase.PQuery("SELECT * FROM character_aura WHERE spell = %u AND guid = %u", spell->Id, guid.GetCounter()))
                        handler->PSendSysMessage("Player %s (guid: %u) has spell %u.", name.c_str(), guid.GetCounter(), spell->Id);
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) hasn't spell %u.", name.c_str(), guid.GetCounter(), spell->Id);
                }
                else
                    handler->PSendSysMessage("Error. Spell %u not found.", spellId);
            }
        }
        return true;
    }

    static bool HandleSupportCheckAurasCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        if (player)
        {
            Unit::AuraApplicationMap const& auras = player->GetAppliedAuras();
            handler->PSendSysMessage("Player's %s (guid: %u) auras:", name.c_str(), guid.GetCounter());
            for (Unit::AuraApplicationMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
            {
                AuraApplication const* aurApp = itr->second;
                Aura const* aura = aurApp->GetBase();

                if (auto spell = sSpellMgr->GetSpellInfo(aura->GetId()))
                    handler->PSendSysMessage("%s (%u) in slot %u.", spell->SpellName[LOCALE_enUS], spell->Id, aurApp->GetSlot());
                else
                    handler->PSendSysMessage("Error. Spell %u not found.", aura->GetId());
            }
        }
        else
        {
            if (QueryResult result = CharacterDatabase.PQuery("SELECT spell, slot FROM character_aura WHERE guid = %u",guid.GetCounter()))
            {
                handler->PSendSysMessage("Player's %s (guid: %u) auras:", name.c_str(), guid.GetCounter());
                do
                {
                    Field* fields = result->Fetch();
                    uint32 spellId = fields[0].GetUInt32();
                    uint32 slot = fields[0].GetUInt32();

                    if (auto spell = sSpellMgr->GetSpellInfo(spellId))
                        handler->PSendSysMessage("%s (%u) in slot %u.", spell->SpellName[LOCALE_enUS], spell->Id, slot);
                    else
                        handler->PSendSysMessage("Error. Spell %u not found.", spellId);

                } while (result->NextRow());
            }
        }
        return true;
    }

    static bool HandleSupportRemoveAuraCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer auraTok{ (char*)tok[1], ',' };
        if (auraTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& str : auraTok)
            {
                uint32 spellId = handler->extractSpellIdFromLink((char*)str);
                if (!spellId)
                {
                    handler->PSendSysMessage("Cant't get spell id.");
                    continue;
                }

                if (auto spell = sSpellMgr->GetSpellInfo(spellId))
                {
                    if (player->HasAura(spell->Id))
                    {
                        player->RemoveAura(spell->Id);
                        handler->PSendSysMessage("Aura %u removed from player %s (guid: %u).", spell->Id, handler->GetNameLink(player).c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) hasn't aura %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), spell->Id);
                }
                else
                    handler->PSendSysMessage("Error. Spell %u not found.", spellId);
            }
        }
        else
        {
            for (auto&& str : auraTok)
            {
                uint32 spellId = handler->extractSpellIdFromLink((char*)str);
                if (!spellId)
                {
                    handler->PSendSysMessage("Cant't get spell id.");
                    continue;
                }

                if (auto spell = sSpellMgr->GetSpellInfo(spellId))
                {
                    if (QueryResult result = CharacterDatabase.PQuery("SELECT slot FROM character_aura WHERE spell = %u AND guid = %u", spell->Id, guid.GetCounter()))
                    {
                        CharacterDatabase.PExecute("DELETE FROM character_aura WHERE spell = %u AND guid = %u", spell->Id, guid.GetCounter());
                        CharacterDatabase.PExecute("DELETE FROM character_aura_effect WHERE slot = %u AND guid = %u", (*result)[0].GetUInt32(), guid.GetCounter());
                        handler->PSendSysMessage("Aura %u removed from player %s (guid: %u).", spell->Id, name.c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) hasn't aura %u.", name.c_str(), guid.GetCounter(), spell->Id);
                }
                else
                    handler->PSendSysMessage("Error. Spell %u not found.", spellId);
            }
        }
        return true;
    }
    // aura support end

    // quest support
    static bool AddQuest(Player* player, Quest const* quest, ChatHandler* handler)
    {
        // copypasted from HandleQuestAdd
        // check item starting quest (it can work incorrectly if added without item in inventory)
        ItemTemplateContainer const* itc = sObjectMgr->GetItemTemplateStore();
        ItemTemplateContainer::const_iterator result = find_if(itc->begin(), itc->end(), Finder<uint32, ItemTemplate>(quest->GetQuestId(), &ItemTemplate::StartQuest));

        if (result != itc->end())
        {
            handler->PSendSysMessage(LANG_COMMAND_QUEST_STARTFROMITEM, quest->GetQuestId(), result->second.ItemId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // ok, normal (creature/GO starting) quest
        if (player->CanAddQuest(quest, true))
        {
            player->AddQuest(quest, NULL);

            bool brokenQuest = sObjectMgr->IsBrokenQuest(quest->GetQuestId());
            if (player->CanCompleteQuest(quest->GetQuestId()) || brokenQuest)
                player->CompleteQuest(quest->GetQuestId(), brokenQuest, true);

            return true;
        }
        return false;
    }
    static void CompleteQuest(Player* player, Quest const* quest)
    {
        player->CompleteQuest(quest->GetQuestId(), true, true);
    }
    static bool RewardQuest(Player* player, Quest const* quest, ChatHandler* handler)
    {
        if (player->GetQuestStatus(quest->GetQuestId()) == QUEST_STATUS_NONE)
            AddQuest(player, quest, handler);
        if (player->GetQuestStatus(quest->GetQuestId()) != QUEST_STATUS_COMPLETE)
            CompleteQuest(player, quest);
        if (player->GetQuestStatus(quest->GetQuestId()) != QUEST_STATUS_COMPLETE)
        {
            handler->PSendSysMessage(LANG_COMMAND_QUEST_NOTFOUND, quest->GetQuestId());
            handler->SetSentErrorMessage(true);
            return false;
        }
        player->RewardQuest(quest, 0, player);
        return true;
    }
    static void RemoveQuest(Player* player, Quest const* quest)
    {
        // remove all quest entries for 'entry' from quest log
        for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
        {
            uint32 logQuest = player->GetQuestSlotQuestId(slot);
            if (logQuest == quest->GetQuestId())
            {
                player->SetQuestSlot(slot, 0);

                // we ignore unequippable quest items in this case, its' still be equipped
                player->TakeQuestSourceItem(logQuest, false);

                if (quest->HasFlag(QUEST_FLAGS_FLAGS_PVP))
                {
                    player->pvpInfo.IsHostile = player->pvpInfo.IsInHostileArea || player->HasPvPForcingQuest();
                    player->UpdatePvPState();
                }
            }
        }

        player->RemoveActiveQuest(quest->GetQuestId(), false);
        player->RemoveRewardedQuest(quest->GetQuestId());

        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_QUESTSTATUS_REWARDED_BY_QUEST);
        stmt->setUInt32(0, player->GetGUID().GetCounter());
        stmt->setUInt32(1, quest->GetQuestId());
        CharacterDatabase.Execute(stmt);
    }
    static bool HasQuestRewardedInDb(ObjectGuid guid, uint32 questId)
    {
        if (CharacterDatabase.PQuery("SELECT * FROM character_queststatus_rewarded WHERE guid = %u AND quest = %u", guid.GetCounter(), questId))
            return true;
        if (CharacterDatabase.PQuery("SELECT * FROM character_queststatus_daily WHERE guid = %u AND quest = %u", guid.GetCounter(), questId))
            return true;
        if (CharacterDatabase.PQuery("SELECT * FROM character_queststatus_monthly WHERE guid = %u AND quest = %u", guid.GetCounter(), questId))
            return true;
        if (CharacterDatabase.PQuery("SELECT * FROM character_queststatus_weekly WHERE guid = %u AND quest = %u", guid.GetCounter(), questId))
            return true;
        return false;
    }

    static bool HandleSupportAddQuestCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer questTok{ (char*)tok[1], ',' };
        if (questTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& str : questTok)
            {
                uint32 questId = atoi(handler->extractKeyFromLink((char*)str, "Hquest"));
                if (!questId)
                {
                    handler->PSendSysMessage("Cant't get quest id.");
                    continue;
                }

                if (auto quest = sObjectMgr->GetQuestTemplate(questId))
                {
                    if (player->GetQuestStatus(quest->GetQuestId()) == QUEST_STATUS_NONE)
                    {
                        if (AddQuest(player, quest, handler))
                            handler->PSendSysMessage("Quest %u added to player %s (guid: %u).", quest->GetQuestId(), handler->GetNameLink(player).c_str(), guid.GetCounter());
                        else
                            handler->PSendSysMessage("Error. Cant't add quest %u.", quest->GetQuestId());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) already has or done quest %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), quest->GetQuestId());
                }
                else
                    handler->PSendSysMessage("Error. Quest %u not found.", questId);
            }
            player->SaveToDB();
        }
        else
        {
            for (auto&& str : questTok)
            {
                uint32 questId = atoi(handler->extractKeyFromLink((char*)str, "Hquest"));
                if (!questId)
                {
                    handler->PSendSysMessage("Cant't get quest id.");
                    continue;
                }

                if (auto quest = sObjectMgr->GetQuestTemplate(questId))
                {
                    if (!HasQuestRewardedInDb(guid, quest->GetQuestId()))
                    {
                        CharacterDatabase.PQuery("INSERT INTO character_queststatus (guid, quest, status, explored, timer) VALUES (%u, %u, 3, 0, %u)", guid.GetCounter(), quest->GetQuestId(), time(nullptr));
                        handler->PSendSysMessage("Quest %u added to player %s (guid: %u).", quest->GetQuestId(), name.c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid %u) already has or done quest %u.", name.c_str(), guid.GetCounter(), quest->GetQuestId());
                }
                else
                    handler->PSendSysMessage("Error. Quest %u not found.", questId);
            }
        }
        return true;
    }

    static bool HandleSupportAddQuestCompCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer questTok{ (char*)tok[1], ',' };
        if (questTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& str : questTok)
            {
                uint32 questId = atoi(handler->extractKeyFromLink((char*)str, "Hquest"));
                if (!questId)
                {
                    handler->PSendSysMessage("Cant't get quest id.");
                    continue;
                }

                if (auto quest = sObjectMgr->GetQuestTemplate(questId))
                {
                    if (player->GetQuestStatus(quest->GetQuestId()) == QUEST_STATUS_INCOMPLETE)
                    {
                        CompleteQuest(player, quest);
                        handler->PSendSysMessage("Quest %u completed for player %s (guid: %u).", quest->GetQuestId(), handler->GetNameLink(player).c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) hasn't quest or already done %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), quest->GetQuestId());
                }
                else
                    handler->PSendSysMessage("Error. Quest %u not found.", questId);
            }
            player->SaveToDB();
        }
        else
        {
            for (auto&& str : questTok)
            {
                uint32 questId = atoi(handler->extractKeyFromLink((char*)str, "Hquest"));
                if (!questId)
                {
                    handler->PSendSysMessage("Cant't get quest id.");
                    continue;
                }

                if (auto quest = sObjectMgr->GetQuestTemplate(questId))
                {
                    if (CharacterDatabase.PQuery("SELECT * FROM character_queststatus WHERE guid = %u AND quest = %u", guid.GetCounter(), questId))
                    {
                        CharacterDatabase.PQuery("UPDATE character_queststatus SET status = 1 WHERE guid = %u AND quest = %u", guid.GetCounter(), quest->GetQuestId());
                        std::string items;
                        for (auto const& obj : quest->Objectives)
                        {
                            if (obj.Type == QUEST_OBJECTIVE_ITEM)
                            {
                                items += std::to_string(obj.ObjectID) + " " + std::to_string(obj.Amount) + ",";
                                continue;
                            }
                            if (obj.Type == QUEST_OBJECTIVE_CURRENCY) // do something with this?
                                continue;
                            if (obj.Type == QUEST_OBJECTIVE_MONEY) // haha
                                continue;

                            CharacterDatabase.PQuery("INSERT INTO character_queststatus_objective (guid, objectiveId, amount) VALUES (%u, %u, %u)", guid.GetCounter(), obj.ID, obj.Amount);
                        }
                        items.erase(items.end() - 1); // remove last ","
                        std::string text = " \"Support\" \"Items for quest" + std::to_string(quest->GetQuestId()) + ".\" ";
                        std::string command = ".send items " + name + text + items;
                        handler->ParseCommands(command.c_str());
                        handler->PSendSysMessage("Quest %u completed for player %s (guid: %u).", quest->GetQuestId(), name.c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid %u) hasn't quest or already done %u.", name.c_str(), guid.GetCounter(), quest->GetQuestId());
                }
                else
                    handler->PSendSysMessage("Error. Quest %u not found.", questId);
            }
        }
        return true;
    }

    static bool HandleSupportAddQuestRewCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer questTok{ (char*)tok[1], ',' };
        if (questTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& str : questTok)
            {
                uint32 questId = atoi(handler->extractKeyFromLink((char*)str, "Hquest"));
                if (!questId)
                {
                    handler->PSendSysMessage("Cant't get quest id.");
                    continue;
                }

                if (auto quest = sObjectMgr->GetQuestTemplate(questId))
                {
                    if (player->GetQuestStatus(quest->GetQuestId()) == QUEST_STATUS_NONE)
                    {
                        if (RewardQuest(player, quest, handler))
                            handler->PSendSysMessage("Quest %u completed for player %s (guid: %u).", quest->GetQuestId(), handler->GetNameLink(player).c_str(), guid.GetCounter());
                        else
                            handler->PSendSysMessage("Cant't reward quest %u.", quest->GetQuestId());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) already done quest %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), quest->GetQuestId());
                }
                else
                    handler->PSendSysMessage("Error. Quest %u not found.", questId);
            }
            player->SaveToDB();
        }
        else
            handler->PSendSysMessage("Command can be used only for online players.");
        return true;
    }

    static bool HandleSupportCheckQuestCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer questTok{ (char*)tok[1], ',' };
        if (questTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& str : questTok)
            {
                uint32 questId = atoi(handler->extractKeyFromLink((char*)str, "Hquest"));
                if (!questId)
                {
                    handler->PSendSysMessage("Cant't get quest id.");
                    continue;
                }

                if (auto quest = sObjectMgr->GetQuestTemplate(questId))
                {
                    if (player->IsMonthlyQuestDone(quest->GetQuestId()))
                        handler->PSendSysMessage("Monthly quest %u completed by player %s (guid: %u).", quest->GetQuestId(), handler->GetNameLink(player).c_str(), guid.GetCounter());
                    else if (player->IsWeeklyQuestDone(quest->GetQuestId()))
                        handler->PSendSysMessage("Weekly quest %u completed by player %s (guid: %u).", quest->GetQuestId(), handler->GetNameLink(player).c_str(), guid.GetCounter());
                    else if (player->IsDailyQuestDone(quest->GetQuestId()))
                        handler->PSendSysMessage("Daily quest %u completed by player %s (guid: %u).", quest->GetQuestId(), handler->GetNameLink(player).c_str(), guid.GetCounter());
                    else if (player->GetQuestStatus(quest->GetQuestId()) == QUEST_STATUS_REWARDED)
                        handler->PSendSysMessage("Quest %u rewarded by player %s (guid: %u).", quest->GetQuestId(), handler->GetNameLink(player).c_str(), guid.GetCounter());
                    else if (player->GetQuestStatus(quest->GetQuestId()) == QUEST_STATUS_COMPLETE)
                        handler->PSendSysMessage("Quest %u completed by player %s (guid: %u).", quest->GetQuestId(), handler->GetNameLink(player).c_str(), guid.GetCounter());
                    else if (player->GetQuestStatus(quest->GetQuestId()) == QUEST_STATUS_INCOMPLETE)
                        handler->PSendSysMessage("Player %s (guid: %u) has quest %u, did not complete it.", handler->GetNameLink(player).c_str(), guid.GetCounter(), quest->GetQuestId());
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) hasn't quest %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), quest->GetQuestId());
                }
                else
                    handler->PSendSysMessage("Error. Quest %u not found.", questId);
            }
        }
        else
        {
            for (auto&& str : questTok)
            {
                uint32 questId = atoi(handler->extractKeyFromLink((char*)str, "Hquest"));
                if (!questId)
                {
                    handler->PSendSysMessage("Cant't get quest id.");
                    continue;
                }

                if (auto quest = sObjectMgr->GetQuestTemplate(questId))
                {
                    if (CharacterDatabase.PQuery("SELECT status FROM character_queststatus_monthly WHERE guid = %u AND quest = %u", guid.GetCounter(), questId))
                        handler->PSendSysMessage("Monthly quest %u completed by player %s (guid: %u).", quest->GetQuestId(), name.c_str(), guid.GetCounter());
                    else if (CharacterDatabase.PQuery("SELECT status FROM character_queststatus_weekly WHERE guid = %u AND quest = %u", guid.GetCounter(), questId))
                        handler->PSendSysMessage("Weekly quest %u completed by player %s (guid: %u).", quest->GetQuestId(), name.c_str(), guid.GetCounter());
                    else if (CharacterDatabase.PQuery("SELECT status FROM character_queststatus_daily WHERE guid = %u AND quest = %u", guid.GetCounter(), questId))
                        handler->PSendSysMessage("Daily quest %u completed by player %s (guid: %u).", quest->GetQuestId(), name.c_str(), guid.GetCounter());
                    else if (QueryResult result = CharacterDatabase.PQuery("SELECT status FROM character_queststatus WHERE guid = %u AND quest = %u", guid.GetCounter(), questId))
                    {
                        if ((*result)[0].GetUInt32() == QUEST_STATUS_REWARDED)
                            handler->PSendSysMessage("Quest %u rewarded for player %s (guid: %u).", quest->GetQuestId(), name.c_str(), guid.GetCounter());
                        else if ((*result)[0].GetUInt32() == QUEST_STATUS_REWARDED)
                            handler->PSendSysMessage("Quest %u completed by player %s (guid: %u).", quest->GetQuestId(), name.c_str(), guid.GetCounter());
                        else if ((*result)[0].GetUInt32() == QUEST_STATUS_REWARDED)
                            handler->PSendSysMessage("Player %s (guid: %u) has quest %u, did not complete it.", name.c_str(), guid.GetCounter(), quest->GetQuestId());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid %u) hasn't quest %u.", name.c_str(), guid.GetCounter(), quest->GetQuestId());
                }
                else
                    handler->PSendSysMessage("Error. Quest %u not found.", questId);
            }
        }
        return true;
    }

    static bool HandleSupportRemoveQuestCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer questTok{ (char*)tok[1], ',' };
        if (questTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& str : questTok)
            {
                uint32 questId = atoi(handler->extractKeyFromLink((char*)str, "Hquest"));
                if (!questId)
                {
                    handler->PSendSysMessage("Cant't get quest id.");
                    continue;
                }

                if (auto quest = sObjectMgr->GetQuestTemplate(questId))
                {
                    if (player->GetQuestStatus(quest->GetQuestId()) == QUEST_STATUS_INCOMPLETE)
                    {
                        RemoveQuest(player, quest);
                        handler->PSendSysMessage("Quest %u completed for player %s (guid: %u).", quest->GetQuestId(), handler->GetNameLink(player).c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid: %u) hasn't quest as done %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), quest->GetQuestId());
                }
                else
                    handler->PSendSysMessage("Error. Quest %u not found.", questId);
            }
            player->SaveToDB();
        }
        else
        {
            for (auto&& str : questTok)
            {
                uint32 questId = atoi(handler->extractKeyFromLink((char*)str, "Hquest"));
                if (!questId)
                {
                    handler->PSendSysMessage("Cant't get quest id.");
                    continue;
                }

                if (auto quest = sObjectMgr->GetQuestTemplate(questId))
                {
                    if (quest->IsRepeatable())
                    {
                        if (quest->IsMonthly())
                            CharacterDatabase.PExecute("DELETE FROM character_queststatus_monthly WHERE guid = %u AND quest = %u", guid.GetCounter(), questId);
                        if (quest->IsWeekly())
                            CharacterDatabase.PExecute("DELETE FROM character_queststatus_weekly WHERE guid = %u AND quest = %u", guid.GetCounter(), questId);
                        if (quest->IsDaily())
                            CharacterDatabase.PExecute("DELETE FROM character_queststatus_daily WHERE guid = %u AND quest = %u", guid.GetCounter(), questId);
                    }
                    else if (CharacterDatabase.PQuery("SELECT * FROM character_queststatus_rewarded WHERE guid = %u AND quest = %u", guid.GetCounter(), questId))
                    {
                        CharacterDatabase.PExecute("DELETE FROM character_queststatus_rewarded WHERE guid = %u AND quest = %u", guid.GetCounter(), questId);
                        handler->PSendSysMessage("Quest %u completed for player %s (guid: %u).", quest->GetQuestId(), name.c_str(), guid.GetCounter());
                    }
                    else
                        handler->PSendSysMessage("Player %s (guid %u) hasn't quest or already done %u.", name.c_str(), guid.GetCounter(), quest->GetQuestId());
                }
                else
                    handler->PSendSysMessage("Error. Quest %u not found.", questId);
            }
        }
        return true;
    }

    static std::string StatusToString(uint32 status)
    {
        switch (status)
        {
            case QUEST_STATUS_NONE:
                return "StatusNone";
            case QUEST_STATUS_COMPLETE:
                return "StatusComplete";
            case QUEST_STATUS_INCOMPLETE:
                return "StatusIncomplete";
            case QUEST_STATUS_FAILED:
                return "StatusFailed";
            case QUEST_STATUS_REWARDED:
                return "StatusReward";
        }
        return "unk";
    }

    static bool HandleSupportCheckQuestActCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 1)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        if (player)
            player->SaveToDB();

        handler->PSendSysMessage("Player's %s (guid %u) active quest list:", name.c_str(), guid.GetCounter());
        if (QueryResult result = CharacterDatabase.PQuery("SELECT quest, status FROM character_queststatus WHERE guid = %u", guid.GetCounter()))
        {
            uint32 questId = (*result)[0].GetUInt32();
            uint32 status = (*result)[1].GetUInt32();
            if (auto quest = sObjectMgr->GetQuestTemplate(questId))
                handler->PSendSysMessage("%s (%u) - status %s.", quest->GetLogTitle().c_str(), questId, StatusToString(status).c_str());
            else
                handler->PSendSysMessage("Error. Quest %u not found.", questId);
        }
        else
            handler->PSendSysMessage("Player %s (guid %u) hasn't any quest.", name.c_str(), guid.GetCounter());

        return true;
    }

    static bool HandleSupportCheckQuestRewCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 1)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        if (player)
            player->SaveToDB();

        handler->PSendSysMessage("Player's %s (guid %u) rewarded quest list:", name.c_str(), guid.GetCounter());
        if (QueryResult result = CharacterDatabase.PQuery("SELECT quest FROM character_queststatus_rewarded WHERE guid = %u", guid.GetCounter()))
        {
            uint32 questId = (*result)[0].GetUInt32();
            if (auto quest = sObjectMgr->GetQuestTemplate(questId))
                handler->PSendSysMessage("%s (%u).", quest->GetLogTitle().c_str(), questId);
            else
                handler->PSendSysMessage("Error. Quest %u not found.", questId);
        }
        else
            handler->PSendSysMessage("Player %s (guid %u) hasn't any quest.", name.c_str(), guid.GetCounter());

        return true;
    }
    // quest support end

    static bool HandleSupportCheckBoostCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 1)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        if (player)
        {
            if (player->GetSession()->HasBoost())
                handler->PSendSysMessage("Player %s (guid: %u) has active boost.", handler->GetNameLink(player).c_str(), guid.GetCounter());
        }
        else
        {
            if (QueryResult charResult = CharacterDatabase.PQuery("SELECT account FROM characters WHERE guid = %u", guid.GetCounter()))
                if (QueryResult boostResult = LoginDatabase.PQuery("SELECT * FROM account_boost WHERE id = %u", (*charResult)[0].GetUInt32()))
                    handler->PSendSysMessage("Player %s (guid: %u) has active boost.", name.c_str(), guid.GetCounter());
        }
        return true;
    }

    static bool HandleSupportCheckServActCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 1)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        if (player)
        {
            handler->PSendSysMessage("Player's %s (guid: %u) active services:", handler->GetNameLink(player).c_str(), guid.GetCounter());
            if (!player->HasAtLoginFlag(AT_LOGIN_RENAME) && !player->HasAtLoginFlag(AT_LOGIN_CUSTOMIZE) && !player->HasAtLoginFlag(AT_LOGIN_CHANGE_RACE) && !player->HasAtLoginFlag(AT_LOGIN_CHANGE_FACTION))
                handler->PSendSysMessage("- no active services");
            else
            {
                if (player->HasAtLoginFlag(AT_LOGIN_RENAME))
                    handler->PSendSysMessage("- rename (1)");
                if (player->HasAtLoginFlag(AT_LOGIN_CUSTOMIZE))
                    handler->PSendSysMessage("- customize (8)");
                if (player->HasAtLoginFlag(AT_LOGIN_CHANGE_FACTION))
                    handler->PSendSysMessage("- change faction (64)");
                if (player->HasAtLoginFlag(AT_LOGIN_CHANGE_RACE))
                    handler->PSendSysMessage("- change race (128)");
                if (player->GetSession()->HasBoost())
                    handler->PSendSysMessage("- boost (in db)");
            }
        }
        else
        {
            bool activeBoost = false;
            if (QueryResult charResult = CharacterDatabase.PQuery("SELECT account FROM characters WHERE guid = %u", guid.GetCounter()))
                if (QueryResult boostResult = LoginDatabase.PQuery("SELECT * FROM account_boost WHERE id = %u", (*charResult)[0].GetUInt32()))
                    activeBoost = true;

            uint32 atLoginFlags = 0;
            if (QueryResult charResult = CharacterDatabase.PQuery("SELECT at_login FROM characters WHERE guid = %u", guid.GetCounter()))
                atLoginFlags = (*charResult)[0].GetUInt32();

            if (!atLoginFlags)
                handler->PSendSysMessage("- no active services");
            else
            {
                if (atLoginFlags & AT_LOGIN_RENAME)
                    handler->PSendSysMessage("- rename (1)");
                if (atLoginFlags & AT_LOGIN_CUSTOMIZE)
                    handler->PSendSysMessage("- customize (8)");
                if (atLoginFlags & AT_LOGIN_CHANGE_FACTION)
                    handler->PSendSysMessage("- change faction (64)");
                if (atLoginFlags & AT_LOGIN_CHANGE_RACE)
                    handler->PSendSysMessage("- change race (128)");
                if (activeBoost)
                    handler->PSendSysMessage("- boost (in db)");
            }
        }
        return true;
    }

    static bool HandleSupportCheckServExectCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 1)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        if (QueryResult charResult = CharacterDatabase.PQuery("SELECT type, old_data, new_data, execute_date FROM executed_services WHERE guid = %u", guid.GetCounter()))
        {
            do
            {
                Field* fields = charResult->Fetch();
                std::string type = fields[0].GetString();
                std::string oldData = fields[1].GetString();
                std::string newData = fields[2].GetString();
                std::string data = fields[3].GetString();

                handler->PSendSysMessage("Service %s executed for player %s (%u) at %s:", type.c_str(), name.c_str(), guid.GetCounter(), data.c_str());
                handler->PSendSysMessage("Old data: '%s', New data: '%s'.", oldData.c_str(), newData.c_str());
            } while (charResult->NextRow());
        }
        else
            handler->PSendSysMessage("Player %s (guid: %u) has no executed services.", name.c_str(), guid.GetCounter());

        return true;
    }

    static bool HandleSupportCheckMailCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 1)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        if (player)
        {
            if (!player->GetMail().empty())
            {
                handler->PSendSysMessage("Player's %s (guid: %u) mail:", handler->GetNameLink(player).c_str(), guid.GetCounter());
                for (auto&& mail : player->GetMail())
                {
                    handler->PSendSysMessage("Mail recived by player %u with subject %s (money %lu)", mail->sender, mail->subject.c_str(), mail->money);
                    if (mail->HasItems())
                    {
                        handler->SendSysMessage("Items:");
                        for (auto&& mItem : mail->items)
                            if (auto item = sObjectMgr->GetItemTemplate(mItem.item_template))
                                handler->PSendSysMessage("- %s - %u (guid: %u)", item->Name1.c_str(), item->ItemId, mItem.item_guid);
                    }
                }
            }
            else
                handler->PSendSysMessage("Player %s (guid: %u) has no mail.", name.c_str(), guid.GetCounter());
        }
        else
        {
            if (QueryResult mailResult = CharacterDatabase.PQuery("SELECT id, sender, subject, has_items, money FROM mail WHERE receiver = %u", guid.GetCounter()))
            {
                handler->PSendSysMessage("Player's %s (guid: %u) mail:", name.c_str(), guid.GetCounter());
                do
                {
                    Field* fields = mailResult->Fetch();
                    uint32 id = fields[0].GetUInt32();
                    uint32 sender = fields[1].GetUInt32();
                    std::string subject = fields[2].GetString();
                    uint8 has_items = fields[3].GetUInt32();
                    uint64 money = fields[4].GetUInt64();

                    handler->PSendSysMessage("Mail recived by player %u with subject %s (money %lu)", sender, subject.c_str(), money);
                    if (has_items)
                    {
                        handler->SendSysMessage("Items:");
                        if (QueryResult mailItemsResult = CharacterDatabase.PQuery("SELECT item_guid FROM mail_items WHERE mail_id = %u", id))
                        {
                            do
                            {
                                Field* fields = mailItemsResult->Fetch();
                                uint32 itemGuid = fields[0].GetUInt32();
                                if (QueryResult itemResult = CharacterDatabase.PQuery("SELECT itemEntry FROM item_instance WHERE guid = %u", itemGuid))
                                    if (auto item = sObjectMgr->GetItemTemplate((*itemResult)[0].GetUInt32()))
                                        handler->PSendSysMessage("- %s - %u (guid: %u)", item->Name1.c_str(), item->ItemId, itemGuid);
                            } while (mailItemsResult->NextRow());
                        }
                    }
                } while (mailResult->NextRow());
            }
            else
                handler->PSendSysMessage("Player %s (guid: %u) has no mail.", name.c_str(), guid.GetCounter());
        }
        return true;
    }

    static bool HandleSupportCheckMailAccCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        char* account = strtok((char*)args, " ");
        if (!account)
            return false;

        std::string accountName = account;
        if (!AccountMgr::normalizeString(accountName))
        {
            handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, accountName.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 accountId = AccountMgr::GetId(accountName);
        if (!accountId)
        {
            handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, accountName.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        std::vector<ObjectGuid> playersList;
        if (QueryResult result = CharacterDatabase.PQuery("SELECT guid FROM characters WHERE account = %u", accountId))
        {
            do
            {
                Field* fields = result->Fetch();
                uint32 guidLow = fields[0].GetUInt32();
                playersList.emplace_back(HighGuid::Player, guidLow);
            } while (result->NextRow());
        }
        else
            handler->PSendSysMessage("Account empty.");

        for (auto&& guid : playersList)
        {
            Player* player = sObjectMgr->GetPlayerByLowGUID(guid);
            std::string name = "unknown";
            sObjectMgr->GetPlayerNameByGUID(guid, name);
            if (player)
            {
                if (!player->GetMail().empty())
                {
                    handler->PSendSysMessage("Player's %s (guid: %u) mail:", handler->GetNameLink(player).c_str(), guid.GetCounter());
                    for (auto&& mail : player->GetMail())
                    {
                        handler->PSendSysMessage("Mail recived by player %u with subject %s (money %lu)", mail->sender, mail->subject.c_str(), mail->money);
                        if (mail->HasItems())
                        {
                            handler->SendSysMessage("Items:");
                            for (auto&& mItem : mail->items)
                                if (auto item = sObjectMgr->GetItemTemplate(mItem.item_template))
                                    handler->PSendSysMessage("- %s - %u (guid: %u)", item->Name1.c_str(), item->ItemId, mItem.item_guid);
                        }
                    }
                }
                else
                    handler->PSendSysMessage("Player %s (guid: %u) has no mail.", handler->GetNameLink(player).c_str(), guid.GetCounter());
            }
            else
            {
                if (QueryResult mailResult = CharacterDatabase.PQuery("SELECT id, sender, subject, has_items, money FROM mail WHERE receiver = %u", guid.GetCounter()))
                {
                    handler->PSendSysMessage("Player's %s (guid: %u) mail:", name.c_str(), guid.GetCounter());
                    do
                    {
                        Field* fields = mailResult->Fetch();
                        uint32 id = fields[0].GetUInt32();
                        uint32 sender = fields[1].GetUInt32();
                        std::string subject = fields[2].GetString();
                        uint32 has_items = fields[3].GetUInt32();
                        uint64 money = fields[4].GetUInt64();

                        handler->PSendSysMessage("Mail recived by player %u with subject %s (money %lu)", sender, subject.c_str(), money);
                        if (has_items)
                        {
                            handler->SendSysMessage("Items:");
                            if (QueryResult mailItemsResult = CharacterDatabase.PQuery("SELECT item_guid FROM mail_items WHERE mail_id = %u", id))
                            {
                                do
                                {
                                    Field* fields = mailItemsResult->Fetch();
                                    uint32 itemGuid = fields[0].GetUInt32();
                                    if (QueryResult itemResult = CharacterDatabase.PQuery("SELECT itemEntry FROM item_instance WHERE guid = %u", itemGuid))
                                        if (auto item = sObjectMgr->GetItemTemplate((*itemResult)[0].GetUInt32()))
                                            handler->PSendSysMessage("- %s - %u (guid: %u)", item->Name1.c_str(), item->ItemId, itemGuid);
                                } while (mailItemsResult->NextRow());
                            }
                        }
                    } while (mailResult->NextRow());
                }
                else
                    handler->PSendSysMessage("Player %s (guid: %u) has no mail.", name.c_str(), guid.GetCounter());
            }
        }
        return true;
    }

    static bool HandleSupportCheckProfCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 1)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        if (player)
        {
            bool found = false;
            handler->PSendSysMessage("Player's %s (guid: %u) professions:", handler->GetNameLink(player).c_str(), guid.GetCounter());
            for (uint32 i = 0; i < sSkillLineStore.GetNumRows(); i++)
            {
                if (auto&& skill = sSkillLineStore.LookupEntry(i))
                {
                    std::string skillName = skill->name[LOCALE_enUS];
                    std::string desc = skill->description[LOCALE_enUS];
                    if ((skill->categoryId == 9 && !desc.empty()) || skill->categoryId == 11)
                    {
                        if (player->HasSkill(skill->id))
                        {
                            handler->PSendSysMessage("%s - %u (value %u)", skillName.c_str(), skill->id, player->GetSkillValue(skill->id));
                            found = true;
                        }
                    }
                }
            }

            if (!found)
                handler->PSendSysMessage("Player %s (guid: %u) has no professions.", handler->GetNameLink(player).c_str(), guid.GetCounter());
        }
        else
        {
            if (QueryResult skillResult = CharacterDatabase.PQuery("SELECT skill, value FROM character_skills WHERE guid = %u", guid.GetCounter()))
            {
                handler->PSendSysMessage("Player's %s (guid: %u) professions:", name.c_str(), guid.GetCounter());
                do
                {
                    Field* fields = skillResult->Fetch();
                    uint32 skillId = fields[0].GetUInt32();
                    uint32 value = fields[1].GetUInt32();
                    if (auto&& skill = sSkillLineStore.LookupEntry(skillId))
                    {
                        std::string skillName = skill->name[LOCALE_enUS];
                        std::string desc = skill->description[LOCALE_enUS];
                        if ((skill->categoryId == 9 && !desc.empty()) || skill->categoryId == 11)
                            handler->PSendSysMessage("%s - %u (value %u)", skillName.c_str(), skill->id, value);
                    }
                } while (skillResult->NextRow());
            }
            else
                handler->PSendSysMessage("Player %s (guid: %u) has no professions.", name.c_str(), guid.GetCounter());
        }
        return true;
    }

    static bool HandleSupportRemoveCacheCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 1)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        if (player)
            handler->PSendSysMessage("Player must be offline.");
        else
        {
            if (QueryResult charResult = CharacterDatabase.PQuery("SELECT account FROM characters WHERE guid = %u", guid.GetCounter()))
                LoginDatabase.PExecute("DELETE FROM account_data WHERE accountId = %u", (*charResult)[0].GetUInt32());

            LoginDatabase.PExecute("DELETE FROM character_account_data WHERE guid = %u", guid.GetCounter());

            handler->PSendSysMessage("Cache for player %s (guid: %u) removed.", name.c_str(), guid.GetCounter());
        }
        return true;
    }

    // reputation support
    static bool HandleSupportChangeRepCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer reputationTok{ (char*)tok[1], ',' };
        if (reputationTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& itr : reputationTok)
            {
                Tokenizer repData{ itr, ':' };
                uint32 repId = 0;
                int32 repValue = 0;
                if (repData.size() == 2)
                {
                    repId = atoi(repData[0]);
                    repValue = atoi(repData[1]);
                }
                else if (repData.size() == 1)
                {
                    repId = atoi(repData[0]);
                    repValue = 1;
                }
                else
                    handler->PSendSysMessage("Error. Incorrect reputation values.");

                if (auto rep = sFactionStore.LookupEntry(repId))
                {
                    if (rep->reputationListID < 0)
                    {
                        handler->PSendSysMessage(LANG_COMMAND_FACTION_NOREP_ERROR, rep->name[handler->GetSessionDbcLocale()], repId);
                        handler->SetSentErrorMessage(true);
                        return false;
                    }

                    player->GetReputationMgr().SetOneFactionReputation(rep, repValue, false);
                    player->GetReputationMgr().SendState(player->GetReputationMgr().GetState(rep));
                    handler->PSendSysMessage("Reputation with faction %s (%u) changed to %u for player %s (%u).", rep->name[handler->GetSessionDbcLocale()], repId, repValue, handler->GetNameLink(player).c_str(), guid.GetCounter());
                }
                else
                    handler->PSendSysMessage("Error. Faction %u not found.", repId);
            }
            player->SaveToDB();
        }
        else
        {
            for (auto&& itr : reputationTok)
            {
                Tokenizer repData{ itr, ':' };
                uint32 repId = 0;
                int32 repValue = 0;
                if (repData.size() == 2)
                {
                    repId = atoi(repData[0]);
                    repValue = atoi(repData[1]);
                }
                else if (repData.size() == 1)
                {
                    repId = atoi(repData[0]);
                    repValue = 1;
                }
                else
                    handler->PSendSysMessage("Error. Incorrect reputation values.");

                if (auto rep = sFactionStore.LookupEntry(repId))
                {
                    if (CharacterDatabase.PQuery("SELECT faction FROM character_reputation WHERE guid = %u AND faction = %u", guid.GetCounter(), repId))
                        CharacterDatabase.PExecute("UPDATE character_reputation SET standing = %u WHERE guid = %u AND faction = %u AND flags = flags | 1", repValue, guid.GetCounter(), repId);
                    else
                        CharacterDatabase.PExecute("INSERT IGNORE INTO character_reputation (guid, faction, standing, flags) VALUES (%u, %u, %u, 17)", guid.GetCounter(), repId, repValue);
                    handler->PSendSysMessage("Reputation with faction %s (%u) changed to %u for player %s (%u).", rep->name[handler->GetSessionDbcLocale()], repId, repValue, handler->GetNameLink(player).c_str(), guid.GetCounter());
                }
                else
                    handler->PSendSysMessage("Error. Faction %u not found.", repId);
            }
        }
        return true;
    }

    static bool HandleSupportCheckRepCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer reputationTok{ (char*)tok[1], ',' };
        if (reputationTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& repStr : reputationTok)
            {
                if (auto rep = sFactionStore.LookupEntry(atoi(repStr)))
                {
                    uint32 value = player->GetReputation(rep->ID);
                    if (value >= 40000000)
                        value = 0;
                    handler->PSendSysMessage("Reputation with faction %s (%u) for player %s (%u).", rep->name[handler->GetSessionDbcLocale()], value, handler->GetNameLink(player).c_str(), guid.GetCounter());
                }
                else
                    handler->PSendSysMessage("Error. Faction %u not found.", atoi(repStr));
            }
        }
        else
        {
            for (auto&& repStr : reputationTok)
            {
                if (auto rep = sFactionStore.LookupEntry(atoi(repStr)))
                {
                    QueryResult result = CharacterDatabase.PQuery("SELECT faction FROM character_reputation WHERE guid = %u AND faction = %u", guid.GetCounter(), rep->ID);
                    int32 value = result && (*result)[0].GetUInt32();
                    handler->PSendSysMessage("Reputation with faction %s (%u) is %u for player %s (%u).", rep->name[handler->GetSessionDbcLocale()], rep->ID, value, name.c_str(), guid.GetCounter());
                }
                else
                    handler->PSendSysMessage("Error. Faction %u not found.", atoi(repStr));
            }
        }
        return true;
    }

    static bool HandleSupportCheckRepsCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 1)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        if (player)
        {
            handler->PSendSysMessage("Reputation for player %s (%u):", handler->GetNameLink(player).c_str(), guid.GetCounter());
            for (uint32 i = 0; i < sFactionStore.GetNumRows(); i++)
            {
                if (auto rep = sFactionStore.LookupEntry(i))
                {
                    uint32 value = player->GetReputation(rep->ID);
                    if (value >= 40000000)
                        value = 0;

                    if (value)
                        handler->PSendSysMessage("- faction %s (%u) - value %u", rep->name[handler->GetSessionDbcLocale()], rep->ID, value);
                }
            }
        }
        else
        {
            handler->PSendSysMessage("Reputation for player %s (%u):", name.c_str(), guid.GetCounter());

            QueryResult result = CharacterDatabase.PQuery("SELECT faction, standing FROM character_reputation WHERE guid = %u", guid.GetCounter());
            if (!result)
            {
                handler->SendSysMessage("Something gone wrong.");
                return false;
            }

            do
            {
                Field* fields = result->Fetch();
                uint32 faction = fields[0].GetUInt32();
                uint32 standing = fields[1].GetUInt32();

                if (!standing)
                    continue;

                if (auto rep = sFactionStore.LookupEntry(faction))
                    handler->PSendSysMessage("- faction %s (%u)", rep->name[handler->GetSessionDbcLocale()], standing);
                else
                    handler->PSendSysMessage("Something gone wrong 2.");
            }
            while (result->NextRow());
        }
        return true;
    }
    // reputation support end

    static bool HandleSupportCheckMoneyCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 1)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        if (player)
        {
            uint64 money = player->GetMoney();
            uint64 mailMoney = 0;
            for (auto&& mail : player->GetMail())
                mailMoney += mail->money;
            handler->PSendSysMessage("Player %s (%u) have %lu money in bag and %lu in mail.", handler->GetNameLink(player).c_str(), guid.GetCounter(), money, mailMoney);
        }
        else
        {
            QueryResult result = CharacterDatabase.PQuery("SELECT money FROM characters WHERE guid = %u", guid.GetCounter());
            uint64 money = result ? (*result)[0].GetUInt64() : 0;
            uint64 mailMoney = 0;

            QueryResult mailResult = CharacterDatabase.PQuery("SELECT money FROM mail WHERE receiver = %u", guid.GetCounter());
            if (mailResult)
            {
                do
                {
                    Field* fields = mailResult->Fetch();
                    mailMoney += fields[0].GetUInt64();
                } while (mailResult->NextRow());
            }

            handler->PSendSysMessage("Player %s (%u) have %lu money in bag and %lu in mail.", name.c_str(), guid.GetCounter(), money, mailMoney);
        }
        return true;
    }

    static bool HandleSupportCheckMoneyAccCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        char* account = strtok((char*)args, " ");
        if (!account)
            return false;

        std::string accountName = account;
        if (!AccountMgr::normalizeString(accountName))
        {
            handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, accountName.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 accountId = AccountMgr::GetId(accountName);
        if (!accountId)
        {
            handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, accountName.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        std::vector<ObjectGuid> playersList;
        if (QueryResult result = CharacterDatabase.PQuery("SELECT guid FROM characters WHERE account = %u", accountId))
        {
            do
            {
                Field* fields = result->Fetch();
                uint32 guidLow = fields[0].GetUInt32();
                playersList.emplace_back(HighGuid::Player, guidLow);
            } while (result->NextRow());
        }
        else
            handler->PSendSysMessage("Account empty.");

        for (auto&& guid : playersList)
        {
            Player* player = sObjectMgr->GetPlayerByLowGUID(guid.GetCounter());
            std::string name = "unknown";
            sObjectMgr->GetPlayerNameByGUID(guid, name);
            if (player)
            {
                uint64 money = player->GetMoney();
                uint64 mailMoney = 0;
                for (auto&& mail : player->GetMail())
                    mailMoney += mail->money;
                handler->PSendSysMessage("Player %s (%u) have %lu money in bag and %lu in mail.", handler->GetNameLink(player).c_str(), guid.GetCounter(), money, mailMoney);
            }
            else
            {
                QueryResult result = CharacterDatabase.PQuery("SELECT money FROM characters WHERE guid = %u", guid.GetCounter());
                uint64 money = result ? (*result)[0].GetUInt64() : 0;
                uint64 mailMoney = 0;

                QueryResult mailResult = CharacterDatabase.PQuery("SELECT money FROM mail WHERE receiver = %u", guid.GetCounter());
                if (mailResult)
                {
                    do
                    {
                        Field* fields = mailResult->Fetch();
                        mailMoney += fields[0].GetUInt64();
                    } while (mailResult->NextRow());
                }
                handler->PSendSysMessage("Player %s (%u) have %lu money in bag and %lu in mail.", name.c_str(), guid.GetCounter(), money, mailMoney);
            }
        }
        return true;
    }

    static bool HandleSupportCheckMoneyGuildCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 1)
            return false;

        Guild* guild = sGuildMgr->GetGuildByName(tok[0]);
        if (!guild)
        {
            handler->PSendSysMessage(LANG_COMMAND_COULDNOTFIND, tok[0]);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint64 money = guild->GetBankMoney();
        handler->PSendSysMessage("Guild %s (%u) have %lu money.", guild->GetName().c_str(), guild->GetId(), money);
        return true;
    }

    static bool HandleSupportChangeMoneyCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        int64 money = atoi(tok[1]);
        if (player)
        {
            player->ModifyMoney(money, true);
            handler->PSendSysMessage("Changed money for player %s (%u) on amount %ld.", handler->GetNameLink(player).c_str(), guid.GetCounter(), money);
            player->SaveToDB();
        }
        else
        {
            if (QueryResult result = CharacterDatabase.PQuery("SELECT money FROM characters WHERE guid = %u", guid.GetCounter()))
            {
                int64 currentAmount = (*result)[0].GetUInt64();
                if (money < 0 && (currentAmount - money) < 0)
                    money = 0;
                else
                    money = currentAmount + money;

                CharacterDatabase.PExecute("UPDATE characters SET money = %u WHERE guid = %u", money, guid.GetCounter());
                handler->PSendSysMessage("Changed money for player %s (%u) on amount %ld.", name.c_str(), guid.GetCounter(), money);
            }
        }
        return true;
    }

    static std::string SlotToString(uint32 slot)
    {
        std::string slotStr;
        switch (slot)
        {
            case PVP_SLOT_ARENA_2v2: slotStr = "2x2";      break;
            case PVP_SLOT_ARENA_3v3: slotStr = "3x3";      break;
            case PVP_SLOT_ARENA_5v5: slotStr = "solo 3x3"; break;
            case PVP_SLOT_RATED_BG:  slotStr = "rated bg"; break;
        }
        return slotStr;
    }

    static bool HandleSupportCheckPVPCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 1)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        handler->PSendSysMessage("PvP rating for Player %s (%u):", name.c_str(), guid.GetCounter());
        for (uint32 i = PVP_SLOT_ARENA_2v2; i < PVP_SLOT_MAX; ++i)
        {
            if (auto info = RatedPvpMgr::Instance()->GetInfo(RatedPvpSlot(i), guid))
            {
                handler->PSendSysMessage("Slot %s (%u): Rating - %u, Rank - %u, SeasonGames - %u, SeasonWins - %u.", SlotToString(i).c_str(), i, info->Rating, info->Rank, info->SeasonGames, info->SeasonWins);
            }
            else
                handler->PSendSysMessage("No PvP info for slot %s (%u).", SlotToString(i).c_str(), i);
        }
        return true;
    }

    static bool HandleSupportChangePVPCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 3)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        uint32 slot = atoi(tok[1]);
        if (slot >= PVP_SLOT_MAX)
            handler->PSendSysMessage("Incorrect slot %u, max %d.", slot, PVP_SLOT_MAX);

        if (auto info = RatedPvpMgr::Instance()->GetInfo(RatedPvpSlot(slot), guid))
        {
            uint32 rating = atoi(tok[2]);
            if (rating > 3500)
                handler->PSendSysMessage("Too high rating for player.");
            uint32 oldRating = info->Rating;
            info->Rating = rating;
            RatedPvpMgr::SaveToDB(info);
            handler->PSendSysMessage("Changed PvP rating for player %s (%u) to %u (from %u) slot %s (%u).", name.c_str(), guid.GetCounter(), rating, oldRating, SlotToString(slot).c_str(), slot);
        }
        else
            handler->PSendSysMessage("Has no PvP info for player %s (%u).", name.c_str(), guid.GetCounter());
        return true;
    }

    static bool HandleSupportChangePVPMMRCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 3)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        uint32 slot = atoi(tok[1]);
        if (slot >= PVP_SLOT_MAX)
            handler->PSendSysMessage("Incorrect slot %u, max %d.", slot, PVP_SLOT_MAX);

        if (auto info = RatedPvpMgr::Instance()->GetInfo(RatedPvpSlot(slot), guid))
        {
            uint32 mmrRating = atoi(tok[2]);
            if (mmrRating > 3500)
                handler->PSendSysMessage("Too high rating for player.");
            uint32 oldMmrRating = info->MatchmakerRating;
            info->MatchmakerRating = mmrRating;
            RatedPvpMgr::SaveToDB(info);
            handler->PSendSysMessage("Changed PvP MMR rating for player %s (%u) to %u (from %u) slot %s (%u).", name.c_str(), guid.GetCounter(), mmrRating, oldMmrRating, SlotToString(slot).c_str(), slot);
        }
        else
            handler->PSendSysMessage("Has no PvP info for player %s (%u).", name.c_str(), guid.GetCounter());
        return true;
    }

    static bool HandleSupportChangeCurrencyCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer currencyTok{ (char*)tok[1], ',' };
        if (currencyTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& itr : currencyTok)
            {
                Tokenizer repData{ itr, ':' };
                if (repData.size() != 2)
                {
                    handler->PSendSysMessage("Error. Incorrect currency values.");
                    return false;
                }
                uint32 currencyId = atoi(repData[0]);
                int32 currencyAmount = atoi(repData[1]);

                if (auto cur = sCurrencyTypesStore.LookupEntry(currencyId))
                {
                    int32 precision = cur->Flags & CURRENCY_FLAG_HIGH_PRECISION ? CURRENCY_PRECISION : 1;
                    player->ModifyCurrency(cur->ID, currencyAmount * precision, true, true, true);
                    handler->PSendSysMessage("Currency %s (%u) changed on %u for player %s (%u).", cur->Name[handler->GetSessionDbcLocale()], cur->ID, currencyAmount, handler->GetNameLink(player).c_str(), guid.GetCounter());
                }
                else
                    handler->PSendSysMessage("Error. Currency %u not found.", currencyId);
            }
            player->SaveToDB();
        }
        else
        {
            for (auto&& itr : currencyTok)
            {
                Tokenizer repData{ itr, ':' };
                if (repData.size() != 2)
                {
                    handler->PSendSysMessage("Error. Incorrect currency values.");
                    return false;
                }
                uint32 currencyId = atoi(repData[0]);
                int32 currencyAmount = atoi(repData[1]);
                int32 currencyAmountOut = atoi(repData[1]);

                if (auto cur = sCurrencyTypesStore.LookupEntry(currencyId))
                {
                    int32 precision = cur->Flags & CURRENCY_FLAG_HIGH_PRECISION ? CURRENCY_PRECISION : 1;
                    if (QueryResult result = CharacterDatabase.PQuery("SELECT total_count FROM character_currency WHERE guid = %u AND currency = %u", guid.GetCounter(), currencyId))
                    {
                        int32 amount = (*result)[0].GetUInt32();
                        currencyAmount *= precision;
                        if (currencyAmount < 0 && (amount - currencyAmount) < 0)
                            currencyAmount = 0;
                        else if (currencyAmount > 0 && (int32)cur->TotalCap && (amount + currencyAmount) > (int32)cur->TotalCap)
                            currencyAmount = cur->TotalCap;
                        else if (currencyAmount > 0 && (int32)cur->WeekCap && (amount + currencyAmount) > (int32)cur->WeekCap)
                        {
                            handler->PSendSysMessage("Your amount higher than week cap (%u). Are you sure?", cur->WeekCap);
                            currencyAmount = amount + currencyAmount;
                        }
                        else
                            currencyAmount = amount + currencyAmount;
                        CharacterDatabase.PExecute("UPDATE character_currency SET total_count = %u WHERE guid = %u AND currency = %u", currencyAmount, guid.GetCounter(), cur->ID);
                    }
                    else
                    {
                        if (currencyAmount < 0)
                        {
                            handler->PSendSysMessage("You trying to add new currency for player with value 0. Are you sure?");
                            currencyAmount = 0;
                        }
                        CharacterDatabase.PExecute("INSERT IGNORE INTO character_currency (guid, currency, total_count, week_count, season_count, flags) VALUES (%u, %u, %u, 0, 0, 0)", guid.GetCounter(), cur->ID, currencyAmount);
                    }
                    handler->PSendSysMessage("Currency %s (%u) changed on %u for player %s (%u).", cur->Name[handler->GetSessionDbcLocale()], cur->ID, currencyAmountOut, name.c_str(), guid.GetCounter());
                }
                else
                    handler->PSendSysMessage("Error. Currency %u not found.", currencyId);
            }
        }
        return true;
    }

    static bool HandleSupportCheckCurrencyCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 2)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        Tokenizer currencyTok{ (char*)tok[1], ',' };
        if (currencyTok.size() == 0)
            return false;

        if (player)
        {
            for (auto&& curStr : currencyTok)
            {
                if (auto cur = sCurrencyTypesStore.LookupEntry(atoi(curStr)))
                {
                    if (uint32 currencyAmount = player->GetCurrency(cur->ID, true))
                        handler->PSendSysMessage("Player %s (%u) has currency %s (%u) - amount %u.", handler->GetNameLink(player).c_str(), guid.GetCounter(), cur->Name[handler->GetSessionDbcLocale()], cur->ID, currencyAmount);
                    else
                        handler->PSendSysMessage("Player %s (%u) hasn't currency %s (%u).", handler->GetNameLink(player).c_str(), guid.GetCounter(), cur->Name[handler->GetSessionDbcLocale()], cur->ID);
                }
                else
                    handler->PSendSysMessage("Error. Currency %u not found.", atoi(curStr));
            }
            player->SaveToDB();
        }
        else
        {
            for (auto&& curStr : currencyTok)
            {
                if (auto cur = sCurrencyTypesStore.LookupEntry(atoi(curStr)))
                {
                    if (QueryResult result = CharacterDatabase.PQuery("SELECT currency FROM character_currency WHERE guid = %u AND currency = %u", guid.GetCounter(), cur->ID))
                    {
                        uint64 currencyAmount = (*result)[0].GetUInt32();
                        handler->PSendSysMessage("Player %s (%u) has currency %s (%u) - amount %lu.", name.c_str(), guid.GetCounter(), cur->Name[handler->GetSessionDbcLocale()], cur->ID, currencyAmount);
                    }
                    else
                        handler->PSendSysMessage("Player %s (%u) hasn't currency %s (%u).", name.c_str(), guid.GetCounter(), cur->Name[handler->GetSessionDbcLocale()], cur->ID);
                }
                else
                    handler->PSendSysMessage("Error. Currency %u not found.", atoi(curStr));
            }
        }
        return true;
    }

    static bool HandleSupportCheckCurrsCommand(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        Tokenizer tok{ args, ' ' };
        if (tok.size() != 1)
            return false;

        Player* player;
        ObjectGuid guid;
        std::string name;
        if (!handler->extractPlayerTarget((char*)tok[0], &player, &guid, &name))
            return false;

        if (player)
        {
            handler->PSendSysMessage("Player's %s (%u) currency:", handler->GetNameLink(player).c_str(), guid.GetCounter());
            for (uint32 i = 0; i < sCurrencyTypesStore.GetNumRows(); i++)
                if (auto cur = sCurrencyTypesStore.LookupEntry(i))
                    if (uint32 currencyAmount = player->GetCurrency(cur->ID, true))
                        handler->PSendSysMessage("%s (%u) - amount %u.", cur->Name[handler->GetSessionDbcLocale()], cur->ID, currencyAmount);
        }
        else
        {
            for (uint32 i = 0; i < sCurrencyTypesStore.GetNumRows(); i++)
                if (auto cur = sCurrencyTypesStore.LookupEntry(i))
                    if (QueryResult result = CharacterDatabase.PQuery("SELECT currency FROM character_currency WHERE guid = %u AND currency = %u", guid.GetCounter(), cur->ID))
                        handler->PSendSysMessage("%s (%u) - amount %u.", cur->Name[handler->GetSessionDbcLocale()], cur->ID, (*result)[0].GetUInt32());
        }
        return true;
    }
};

void AddSC_support_commandscript()
{
    new support_commandscript();
}
