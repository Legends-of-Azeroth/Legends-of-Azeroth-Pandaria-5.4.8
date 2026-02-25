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

#include "Common.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "GuildMgr.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "DatabaseEnv.h"
#include "CellImpl.h"
#include "Chat.h"
#include "ChannelMgr.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "Guild.h"
#include "Language.h"
#include "Log.h"
#include "Opcodes.h"
#include "Player.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "Util.h"
#include "ScriptMgr.h"
#include "AccountMgr.h"
#include "WordFilterMgr.h"
#ifdef ELUNA
#include "HookMgr.h"
#endif
void WorldSession::HandleMessagechatOpcode(WorldPacket& recvData)
{
    ChatMsg type;
    uint32 lang;

    switch (recvData.GetOpcode())
    {
        case CMSG_MESSAGECHAT_SAY:
            type = CHAT_MSG_SAY;
            break;
        case CMSG_MESSAGECHAT_YELL:
            type = CHAT_MSG_YELL;
            break;
        case CMSG_MESSAGECHAT_PARTY:
            type = CHAT_MSG_PARTY;
            break;
        case CMSG_MESSAGECHAT_RAID:
            type = CHAT_MSG_RAID;
            break;
        case CMSG_MESSAGECHAT_RAID_WARNING:
            type = CHAT_MSG_RAID_WARNING;
            break;
        case CMSG_MESSAGECHAT_GUILD:
            type = CHAT_MSG_GUILD;
            break;
        case CMSG_MESSAGECHAT_OFFICER:
            type = CHAT_MSG_OFFICER;
            break;
        case CMSG_MESSAGECHAT_WHISPER:
            type = CHAT_MSG_WHISPER;
            break;
        case CMSG_MESSAGECHAT_EMOTE:
            type = CHAT_MSG_EMOTE;
            break;
        case CMSG_MESSAGECHAT_CHANNEL:
            type = CHAT_MSG_CHANNEL;
            break;
        case CMSG_MESSAGECHAT_AFK:
            type = CHAT_MSG_AFK;
            break;
        case CMSG_MESSAGECHAT_DND:
            type = CHAT_MSG_DND;
            break;
        case CMSG_MESSAGECHAT_INSTANCE:
            type = CHAT_MSG_INSTANCE_CHAT;
            break;
        /*
            case CMSG_MESSAGECHAT_BATTLEGROUND:
            type = CHAT_MSG_BATTLEGROUND;
            break;
        */
        default:
            TC_LOG_ERROR("network", "HandleMessagechatOpcode : Unknown chat opcode (%u)", recvData.GetOpcode());
            recvData.hexlike();
            return;
    }

    Player* sender = GetPlayer();

    //TC_LOG_DEBUG("misc", "CHAT: packet received. type %u, lang %u", type, lang);

    // no language sent with emote packet.
    if (type != CHAT_MSG_EMOTE && type != CHAT_MSG_AFK && type != CHAT_MSG_DND)
    {
        recvData >> lang;

        // prevent talking at unknown language (cheating)
        LanguageDesc const* langDesc = GetLanguageDescByID(lang);
        if (!langDesc)
        {
            SendNotification(LANG_UNKNOWN_LANGUAGE);
            recvData.rfinish();
            return;
        }
        if (langDesc->skill_id != 0 && !sender->HasSkill(langDesc->skill_id))
        {
            // also check SPELL_AURA_COMPREHEND_LANGUAGE (client offers option to speak in that language)
            Unit::AuraEffectList const& langAuras = sender->GetAuraEffectsByType(SPELL_AURA_COMPREHEND_LANGUAGE);
            bool foundAura = false;
            for (Unit::AuraEffectList::const_iterator i = langAuras.begin(); i != langAuras.end(); ++i)
            {
                if ((*i)->GetMiscValue() == int32(lang))
                {
                    foundAura = true;
                    break;
                }
            }
            if (!foundAura)
            {
                SendNotification(LANG_NOT_LEARNED_LANGUAGE);
                recvData.rfinish();
                return;
            }
        }

        if (lang == LANG_ADDON)
        {
            // LANG_ADDON is only valid for the following message types
            switch (type)
            {
                case CHAT_MSG_PARTY:
                case CHAT_MSG_RAID:
                case CHAT_MSG_GUILD:
                case CHAT_MSG_WHISPER:
                    if (sWorld->getBoolConfig(CONFIG_CHATLOG_ADDON))
                    {
                        std::string msg = "";
                        recvData >> msg;

                        if (msg.empty())
                            return;

                        sScriptMgr->OnPlayerChat(sender, uint32(CHAT_MSG_ADDON), lang, msg);
                    }

                    // Disabled addon channel?
                    if (!sWorld->getBoolConfig(CONFIG_ADDON_CHANNEL))
                        return;
                    break;
                default:
                    TC_LOG_ERROR("network", "Player %s (GUID: %u) sent a chatmessage with an invalid language/message type combination",
                        GetPlayer()->GetName().c_str(), GetPlayer()->GetGUID().GetCounter());

                    recvData.rfinish();
                    return;
            }
        }
        // LANG_ADDON should not be changed nor be affected by flood control
        else
        {
            // send in universal language if player in .gm on mode (ignore spell effects)
            if (sender->IsGameMaster())
                lang = LANG_UNIVERSAL;
            else
            {
                // send in universal language in two side iteration allowed mode
                if (sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHAT))
                    lang = LANG_UNIVERSAL;
                else
                {
                    switch (type)
                    {
                        case CHAT_MSG_PARTY:
                        case CHAT_MSG_PARTY_LEADER:
                        case CHAT_MSG_RAID:
                        case CHAT_MSG_RAID_LEADER:
                        case CHAT_MSG_RAID_WARNING:
                            // allow two side chat at group channel if two side group allowed
                            if (sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GROUP) || sender->GetGroup() && sender->GetGroup()->isLFGGroup() && sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_LFG))
                                lang = LANG_UNIVERSAL;
                            break;
                        case CHAT_MSG_GUILD:
                        case CHAT_MSG_OFFICER:
                            // allow two side chat at guild channel if two side guild allowed
                            if (sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GUILD))
                                lang = LANG_UNIVERSAL;
                            break;
                    }
                }

                // but overwrite it by SPELL_AURA_MOD_LANGUAGE auras (only single case used)
                Unit::AuraEffectList const& ModLangAuras = sender->GetAuraEffectsByType(SPELL_AURA_MOD_LANGUAGE);
                if (!ModLangAuras.empty())
                    lang = ModLangAuras.front()->GetMiscValue();
            }

            if (!sender->CanSpeak())
            {
                std::string timeStr = secsToTimeString(m_muteTime - time(NULL));
                SendNotification(GetTrinityString(LANG_WAIT_BEFORE_SPEAKING), timeStr.c_str());
                recvData.rfinish(); // Prevent warnings
                return;
            }
        }
    }
    else
        lang = LANG_UNIVERSAL;

    if (sender->HasAura(1852) && type != CHAT_MSG_WHISPER)
    {
        SendNotification(GetTrinityString(LANG_GM_SILENCE), sender->GetName().c_str());
        recvData.rfinish();
        return;
    }

    uint32 textLength = 0;
    uint32 receiverLength = 0;
    std::string to, channel, msg;
    bool ignoreChecks = false;
    switch (type)
    {
        case CHAT_MSG_SAY:
        case CHAT_MSG_EMOTE:
        case CHAT_MSG_YELL:
        case CHAT_MSG_PARTY:
        case CHAT_MSG_GUILD:
        case CHAT_MSG_OFFICER:
        case CHAT_MSG_RAID:
        case CHAT_MSG_RAID_WARNING:
        case CHAT_MSG_INSTANCE_CHAT:
            textLength = recvData.ReadBits(8);
            msg = recvData.ReadString(textLength);
            break;
        case CHAT_MSG_WHISPER:
            textLength = recvData.ReadBits(8);
            receiverLength = recvData.ReadBits(9);
            msg = recvData.ReadString(textLength);
            to = recvData.ReadString(receiverLength);
            break;
        case CHAT_MSG_CHANNEL:
            textLength = recvData.ReadBits(9);
            receiverLength = recvData.ReadBits(8);
            msg = recvData.ReadString(receiverLength);
            channel = recvData.ReadString(textLength);
            break;
        case CHAT_MSG_AFK:
        case CHAT_MSG_DND:
            textLength = recvData.ReadBits(8);
            msg = recvData.ReadString(textLength);
            ignoreChecks = true;
            break;
    }

    if (!ignoreChecks)
    {
        if (msg.empty())
            return;

        if (ChatHandler(this).ParseCommands(msg.c_str()))
            return;

        if (lang != LANG_ADDON)
        {
            if (sWorld->getBoolConfig(CONFIG_GM_USE_ONLINE_MUTES))
            {
                uint64 onlineMuteTimer = GetMute().Timer / 1000;
                if (onlineMuteTimer > 0)
                {
                    bool disallow = true;
                    if (GetMute().PublicChannelsOnly)
                    {
                        disallow = false;
                        if (type == CHAT_MSG_CHANNEL)
                            if (ChannelMgr* cMgr = ChannelMgr::forTeam(_player->GetTeam()))
                                if (Channel* chn = cMgr->GetChannel(channel, _player))
                                    if (chn->IsConstant())
                                        disallow = true;
                    }

                    if (disallow)
                    {
                        std::string timeStr = secsToTimeString(onlineMuteTimer);
                        ChatHandler(sender).PSendSysMessage(GetMute().PublicChannelsOnly ? LANG_CHAT_WAS_MUTED_PUBLIC : LANG_CHAT_WAS_MUTED, GetMute().Author.c_str(), GetMute().Reason.c_str());
                        ChatHandler(sender).PSendSysMessage(GetMute().PublicChannelsOnly ? LANG_WAIT_BEFORE_SPEAKING_PUBLIC : LANG_WAIT_BEFORE_SPEAKING, timeStr.c_str());
                        SendNotification(GetMute().PublicChannelsOnly ? LANG_WAIT_BEFORE_SPEAKING_PUBLIC : LANG_WAIT_BEFORE_SPEAKING, timeStr.c_str());
                        recvData.rfinish();
                        return;
                    }
                }
            }

            // Strip invisible characters for non-addon messages
            if (sWorld->getBoolConfig(CONFIG_CHAT_FAKE_MESSAGE_PREVENTING))
                stripLineInvisibleChars(msg);

            if (sWorld->getIntConfig(CONFIG_CHAT_STRICT_LINK_CHECKING_SEVERITY) && !ChatHandler(this).isValidChatMessage(msg.c_str()))
            {
                TC_LOG_ERROR("network", "Player %s (GUID: %u) sent a chatmessage with an invalid link: %s", GetPlayer()->GetName().c_str(),
                    GetPlayer()->GetGUID().GetCounter(), msg.c_str());

                if (sWorld->getIntConfig(CONFIG_CHAT_STRICT_LINK_CHECKING_KICK))
                    KickPlayer();

                return;
            }
        }
    }

    /// filtering of bad words
    if(sWorld->getBoolConfig(CONFIG_WORD_FILTER_ENABLE))
    {
        sender->m_sentMsgCache.append(msg);

        // horizontally
        std::string badWord = sWordFilterMgr->FindBadWord(msg);

        // vertically
        if(badWord.empty())
            badWord = sWordFilterMgr->FindBadWord(sender->m_sentMsgCache);

        if (!badWord.empty())
        {
            sender->m_sentMsgCache.erase();

            if(!sender->CanSpeak())
                return;

            time_t muteTime = time(NULL) + (int64)(sWorld->getIntConfig(CONFIG_WORD_FILTER_MUTE_DURATION) / 1000);
            sender->GetSession()->m_muteTime = muteTime;
            ChatHandler(sender->GetSession()).PSendSysMessage(LANG_WORD_FILTER_FOUND_BAD_WORD_IN_CHAT, badWord.c_str());
            return;
        }

        const uint8 maxSentMsgCacheSize = 64;

        if(sender->m_sentMsgCache.size() >= maxSentMsgCacheSize)
            sender->m_sentMsgCache.erase();
    }

    switch (type)
    {
        case CHAT_MSG_SAY:
        {
            // Prevent cheating
            if (!sender->IsAlive())
                return;

            if (sender->GetLevel() < sWorld->getIntConfig(CONFIG_CHAT_SAY_LEVEL_REQ))
            {
                SendNotification(GetTrinityString(LANG_SAY_REQ), sWorld->getIntConfig(CONFIG_CHAT_SAY_LEVEL_REQ));
                return;
            }
#ifdef ELUNA
            if (!sHookMgr->OnChat(sender, type, lang, msg))
                return;
#endif
            sender->Say(msg, Language(lang));
            break;
        }            
        case CHAT_MSG_EMOTE:
        {
            // Prevent cheating
            if (!sender->IsAlive())
                return;

            if (sender->GetLevel() < sWorld->getIntConfig(CONFIG_CHAT_EMOTE_LEVEL_REQ))
            {
                SendNotification(GetTrinityString(LANG_SAY_REQ), sWorld->getIntConfig(CONFIG_CHAT_EMOTE_LEVEL_REQ));
                return;
            }
#ifdef ELUNA
            if (!sHookMgr->OnChat(sender, type, lang, msg))
                return;
#endif
            sender->TextEmote(msg);
            break;
        }            
        case CHAT_MSG_YELL:
        {
            // Prevent cheating
            if (!sender->IsAlive())
                return;

            if (sender->GetLevel() < sWorld->getIntConfig(CONFIG_CHAT_YELL_LEVEL_REQ))
            {
                SendNotification(GetTrinityString(LANG_SAY_REQ), sWorld->getIntConfig(CONFIG_CHAT_YELL_LEVEL_REQ));
                return;
            }
#ifdef ELUNA
            if (!sHookMgr->OnChat(sender, type, lang, msg))
                return;
#endif
            sender->Yell(msg, Language(lang));
            break;
        }
        case CHAT_MSG_WHISPER:
        {
            if (!normalizePlayerName(to))
            {
                SendPlayerNotFoundNotice(to);
                break;
            }

            Player* receiver = ObjectAccessor::FindPlayerByName(to);
            if (!receiver || (!receiver->isAcceptWhispers() && GetSecurity() == SEC_PLAYER && !receiver->IsInWhisperWhiteList(sender->GetGUID())))
            {
                SendPlayerNotFoundNotice(to);
                return;
            }
            if (!sender->IsGameMaster() && sender->GetLevel() < sWorld->getIntConfig(CONFIG_CHAT_WHISPER_LEVEL_REQ) && !receiver->IsInWhisperWhiteList(sender->GetGUID()))
            {
                SendNotification(GetTrinityString(LANG_WHISPER_REQ), sWorld->getIntConfig(CONFIG_CHAT_WHISPER_LEVEL_REQ));
                return;
            }

            if (GetPlayer()->GetTeam() != receiver->GetTeam() && !sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHAT)
                && GetSecurity() == SEC_PLAYER && receiver->GetSession()->GetSecurity() == SEC_PLAYER && !receiver->IsInWhisperWhiteList(sender->GetGUID()))
            {
                SendPlayerNotFoundNotice(to);
                return;
            }

            if (GetPlayer()->HasAura(1852) && !receiver->IsGameMaster())
            {
                SendNotification(GetTrinityString(LANG_GM_SILENCE), GetPlayer()->GetName().c_str());
                return;
            }

            // If player is a Gamemaster and doesn't accept whisper, we auto-whitelist every player that the Gamemaster is talking to
            // We also do that if a player is under the required level for whispers.
            if (receiver->GetLevel() < sWorld->getIntConfig(CONFIG_CHAT_WHISPER_LEVEL_REQ) ||
                (GetSecurity() > SEC_PLAYER && !sender->isAcceptWhispers() && !sender->IsInWhisperWhiteList(receiver->GetGUID())))
                sender->AddWhisperWhiteList(receiver->GetGUID());

            GetPlayer()->Whisper(msg, Language(lang), receiver);
        } break;
        case CHAT_MSG_PARTY:
        case CHAT_MSG_PARTY_LEADER:
        {
            // if player is in battleground, he cannot say to battleground members by /p
            Group* group = GetPlayer()->GetOriginalGroup();
            if (!group)
            {
                group = _player->GetGroup();
                if (!group || group->isBGGroup())
                    return;
            }

            if (group->IsLeader(GetPlayer()->GetGUID()))
                type = CHAT_MSG_PARTY_LEADER;

            sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, group);
#ifdef ELUNA
            if (!sHookMgr->OnChat(sender, type, lang, msg, group))
                return;
#endif

            WorldPacket data;
            ChatHandler::BuildChatPacket(data, type, Language(lang), _player, NULL, msg);
            group->BroadcastChat(&data, GetPlayer()->GetGUID(), group->GetMemberGroup(GetPlayer()->GetGUID()));
        } break;
        case CHAT_MSG_GUILD:
        {
            if (GetPlayer()->GetGuildId())
            {
                if (Guild* guild = sGuildMgr->GetGuildById(GetPlayer()->GetGuildId()))
                {
                    sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, guild);
#ifdef ELUNA
                    if (!sHookMgr->OnChat(sender, type, lang, msg, guild))
                        return;
#endif

                    guild->BroadcastToGuild(this, false, msg, lang == LANG_ADDON ? LANG_ADDON : LANG_UNIVERSAL);
                }
            }
        } break;
        case CHAT_MSG_OFFICER:
        {
            if (GetPlayer()->GetGuildId())
            {
                if (Guild* guild = sGuildMgr->GetGuildById(GetPlayer()->GetGuildId()))
                {
                    sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, guild);
#ifdef ELUNA
                    if (!sHookMgr->OnChat(sender, type, lang, msg, guild))
                        return;
#endif

                    guild->BroadcastToGuild(this, true, msg, lang == LANG_ADDON ? LANG_ADDON : LANG_UNIVERSAL);
                }
            }
        } break;
        case CHAT_MSG_RAID:
        case CHAT_MSG_RAID_LEADER:
        {
            bool instance = false;
            Group* group = GetPlayer()->GetGroup();
            if (!group || !group->isRaidGroup())
                return;
            instance = group->isBGGroup() || group->isLFGGroup();
            if (instance)
                type = CHAT_MSG_INSTANCE_CHAT;

            if (group->IsLeader(GetPlayer()->GetGUID()))
                type = instance ? CHAT_MSG_INSTANCE_CHAT_LEADER : CHAT_MSG_RAID_LEADER;

            sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, group);
#ifdef ELUNA
            if (!sHookMgr->OnChat(sender, type, lang, msg, group))
                return;
#endif

            WorldPacket data;
            ChatHandler::BuildChatPacket(data, type, Language(lang), _player, NULL, msg);
            group->BroadcastChat(&data, GetPlayer()->GetGUID());
            break;
        }
        case CHAT_MSG_RAID_WARNING:
        {
            Group* group = GetPlayer()->GetGroup();
            if (!group || !group->isRaidGroup() || !(group->IsLeader(GetPlayer()->GetGUID()) || group->IsAssistant(GetPlayer()->GetGUID())) || group->isBGGroup())
                return;

            sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, group);
#ifdef ELUNA
            if (!sHookMgr->OnChat(sender, type, lang, msg, group))
                return;
#endif

            WorldPacket data;
            //in battleground, raid warning is sent only to players in battleground - code is ok
            ChatHandler::BuildChatPacket(data, CHAT_MSG_RAID_WARNING, Language(lang), _player, NULL, msg);
            group->BroadcastPacket(&data, false);
            break;
        }
        case CHAT_MSG_CHANNEL:
        {
            if (_player->GetLevel() < sWorld->getIntConfig(CONFIG_CHAT_CHANNEL_LEVEL_REQ))
            {
                SendNotification(GetTrinityString(LANG_CHANNEL_REQ), sWorld->getIntConfig(CONFIG_CHAT_CHANNEL_LEVEL_REQ));
                return;
            }

            if (ChannelMgr* cMgr = ChannelMgr::forTeam(_player->GetTeam()))
            {
                if (Channel* chn = cMgr->GetChannel(channel, _player))
                {
                    sScriptMgr->OnPlayerChat(_player, type, lang, msg, chn);
#ifdef ELUNA
                    if (!sHookMgr->OnChat(sender, type, lang, msg, chn))
                        return;
#endif
                    chn->Say(_player->GetGUID(), msg.c_str(), lang);
                }
            }
        } break;
        case CHAT_MSG_AFK:
        {
            if (!_player->IsInCombat())
            {
                if (_player->isAFK())                       // Already AFK
                {
                    if (msg.empty())
                        _player->ToggleAFK();               // Remove AFK
                    else
                        _player->autoReplyMsg = msg;        // Update message
                }
                else                                        // New AFK mode
                {
                    _player->autoReplyMsg = msg.empty() ? GetTrinityString(LANG_PLAYER_AFK_DEFAULT) : msg;

                    if (_player->isDND())
                        _player->ToggleDND();

                    _player->ToggleAFK();
                }

                sScriptMgr->OnPlayerChat(_player, type, lang, msg);
#ifdef ELUNA
                if (!sHookMgr->OnChat(sender, type, lang, msg))
                    return;
#endif
            }
            break;
        }
        case CHAT_MSG_DND:
        {
            if (_player->isDND())                           // Already DND
            {
                if (msg.empty())
                    _player->ToggleDND();                   // Remove DND
                else
                    _player->autoReplyMsg = msg;            // Update message
            }
            else                                            // New DND mode
            {
                _player->autoReplyMsg = msg.empty() ? GetTrinityString(LANG_PLAYER_DND_DEFAULT) : msg;

                if (_player->isAFK())
                    _player->ToggleAFK();

                _player->ToggleDND();
            }

            sScriptMgr->OnPlayerChat(_player, type, lang, msg);
#ifdef ELUNA
            if (!sHookMgr->OnChat(sender, type, lang, msg))
                return;
#endif
            break;
        }
        case CHAT_MSG_INSTANCE_CHAT:
        {
            Group* group = GetPlayer()->GetGroup(GroupSlot::Instance);
            if (!group)
                return;

            if (group->IsLeader(GetPlayer()->GetGUID()))
                type = CHAT_MSG_INSTANCE_CHAT_LEADER;

            sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, group);

            WorldPacket data;
            ChatHandler::BuildChatPacket(data, type, Language(lang), _player, NULL, msg);
            group->BroadcastChat(&data, GetPlayer()->GetGUID());
            break;
        }
        default:
            TC_LOG_ERROR("network", "CHAT: unknown message type %u, lang: %u", type, lang);
            break;
    }
}

void WorldSession::HandleAddonMessagechatOpcode(WorldPacket& recvData)
{
    Player* sender = GetPlayer();
    ChatMsg type;

    switch (recvData.GetOpcode())
    {
        case CMSG_MESSAGECHAT_ADDON_INSTANCE_CHAT:
            type = CHAT_MSG_INSTANCE_CHAT;
            break;
        case CMSG_MESSAGECHAT_ADDON_GUILD:
            type = CHAT_MSG_GUILD;
            break;
        case CMSG_MESSAGECHAT_ADDON_OFFICER:
            type = CHAT_MSG_OFFICER;
            break;
        case CMSG_MESSAGECHAT_ADDON_PARTY:
            type = CHAT_MSG_PARTY;
            break;
        case CMSG_MESSAGECHAT_ADDON_RAID:
            type = CHAT_MSG_RAID;
            break;
        case CMSG_MESSAGECHAT_ADDON_WHISPER:
            type = CHAT_MSG_WHISPER;
            break;
        default:
            TC_LOG_ERROR("network", "HandleAddonMessagechatOpcode: Unknown addon chat opcode (%u)", recvData.GetOpcode());
            recvData.hexlike();
            return;
    }

    std::string message;
    std::string prefix;
    std::string targetName;

    switch (type)
    {
        case CHAT_MSG_WHISPER:
        {
            uint32 targetLen = recvData.ReadBits(9);
            uint32 msgLen = recvData.ReadBits(8);
            uint32 prefixLen = recvData.ReadBits(5);
            targetName = recvData.ReadString(targetLen);
            prefix = recvData.ReadString(prefixLen);
            message = recvData.ReadString(msgLen);
            break;
        }
        case CHAT_MSG_PARTY:
        {
            uint32 msgLen = recvData.ReadBits(8);
            uint32 prefixLen = recvData.ReadBits(5);
            message = recvData.ReadString(msgLen);
            prefix = recvData.ReadString(prefixLen);
            break;
        }
        case CHAT_MSG_RAID:
        case CHAT_MSG_INSTANCE_CHAT:
        {
            uint32 prefixLen = recvData.ReadBits(5);
            uint32 msgLen = recvData.ReadBits(8);
            message = recvData.ReadString(msgLen);
            prefix = recvData.ReadString(prefixLen);
            break;
        }
        case CHAT_MSG_GUILD:
        case CHAT_MSG_OFFICER:
        {
            uint32 msgLen = recvData.ReadBits(8);
            uint32 prefixLen = recvData.ReadBits(5);
            prefix = recvData.ReadString(prefixLen);
            message = recvData.ReadString(msgLen);
            break;
        }
        default:
            break;
    }

    if (prefix.empty() || prefix.length() > 16)
        return;

    // Logging enabled?
    if (sWorld->getBoolConfig(CONFIG_CHATLOG_ADDON))
    {
        if (message.empty())
            return;

        // Weird way to log stuff...
        sScriptMgr->OnPlayerChat(sender, uint32(CHAT_MSG_ADDON), uint32(LANG_ADDON), message);
    }

    // Disabled addon channel?
    if (!sWorld->getBoolConfig(CONFIG_ADDON_CHANNEL))
        return;

    switch (type)
    {
        case CHAT_MSG_INSTANCE_CHAT:
        {
            Group* group = sender->GetGroup();
            if (!group)
                return;

            WorldPacket data;
            ChatHandler::BuildChatPacket(data, type, LANG_ADDON, sender, NULL, message, 0U, "", DEFAULT_LOCALE, prefix);
            group->BroadcastAddonMessagePacket(&data, prefix, false);
            break;
        }
        case CHAT_MSG_GUILD:
        case CHAT_MSG_OFFICER:
        {
            if (sender->GetGuildId())
                if (Guild* guild = sGuildMgr->GetGuildById(sender->GetGuildId()))
                    guild->BroadcastAddonToGuild(this, type == CHAT_MSG_OFFICER, message, prefix);
            break;
        }
        case CHAT_MSG_WHISPER:
        {
            if (!normalizePlayerName(targetName))
                break;
            Player* receiver = ObjectAccessor::FindPlayerByName(targetName.c_str());
            if (!receiver)
                break;

            sender->WhisperAddon(message, prefix, receiver);
            break;
        }
        // Messages sent to "RAID" while in a party will get delivered to "PARTY"
        case CHAT_MSG_PARTY:
        case CHAT_MSG_RAID:
        {

            Group* group = sender->GetGroup();
            if (!group || group->isBGGroup())
                break;

            WorldPacket data;
            ChatHandler::BuildChatPacket(data, type, LANG_ADDON, sender, NULL, message, 0U, "", DEFAULT_LOCALE, prefix);
            group->BroadcastAddonMessagePacket(&data, prefix, true, -1, group->GetMemberGroup(sender->GetGUID()));
            break;
        }
        default:
        {
            TC_LOG_ERROR("misc", "HandleAddonMessagechatOpcode: unknown addon message type %u", type);
            break;
        }
    }
}

void WorldSession::HandleEmoteOpcode(WorldPacket& recvData)
{
    if (!GetPlayer()->IsAlive() || GetPlayer()->HasUnitState(UNIT_STATE_DIED))
    {
        recvData.rfinish();
        return;
    }

    uint32 emote;
    recvData >> emote;
    sScriptMgr->OnPlayerEmote(GetPlayer(), emote);
    GetPlayer()->HandleSwitchEmoteCommand(emote);
}

namespace Trinity
{
    class EmoteChatBuilder
    {
        public:
            EmoteChatBuilder(Player const& player, uint32 text_emote, uint32 emote_num, Unit const* target)
                : i_player(player), i_text_emote(text_emote), i_emote_num(emote_num), i_target(target) { }

            void operator()(WorldPacket& data, LocaleConstant loc_idx)
            {
                ObjectGuid PlayerGuid = i_player.GetGUID();
                ObjectGuid TargetGuid = i_target ? i_target->GetGUID() : ObjectGuid::Empty;

                data.Initialize(SMSG_TEXT_EMOTE, 2 * (8 + 1) + 4 + 4);

                data.WriteBit(PlayerGuid[1]);
                data.WriteBit(TargetGuid[7]);
                data.WriteBit(PlayerGuid[6]);
                data.WriteBit(TargetGuid[5]);
                data.WriteBit(PlayerGuid[3]);
                data.WriteBit(TargetGuid[6]);
                data.WriteBit(TargetGuid[2]);
                data.WriteBit(PlayerGuid[7]);
                data.WriteBit(TargetGuid[0]);
                data.WriteBit(TargetGuid[1]);
                data.WriteBit(PlayerGuid[4]);
                data.WriteBit(PlayerGuid[2]);
                data.WriteBit(TargetGuid[3]);
                data.WriteBit(TargetGuid[4]);
                data.WriteBit(PlayerGuid[0]);
                data.WriteBit(PlayerGuid[5]);

                data.WriteByteSeq(TargetGuid[2]);
                data.WriteByteSeq(TargetGuid[1]);
                data.WriteByteSeq(PlayerGuid[7]);
                data.WriteByteSeq(PlayerGuid[4]);
                data.WriteByteSeq(TargetGuid[7]);
                data.WriteByteSeq(PlayerGuid[5]);
                data.WriteByteSeq(PlayerGuid[2]);

                data << uint32(i_text_emote);

                data.WriteByteSeq(PlayerGuid[6]);
                data.WriteByteSeq(TargetGuid[0]);
                data.WriteByteSeq(PlayerGuid[3]);
                data.WriteByteSeq(PlayerGuid[1]);
                data.WriteByteSeq(TargetGuid[6]);
                data.WriteByteSeq(PlayerGuid[0]);
                data.WriteByteSeq(TargetGuid[3]);
                data.WriteByteSeq(TargetGuid[5]);
                data.WriteByteSeq(TargetGuid[4]);

                data << uint32(i_emote_num);
            }

        private:
            Player const& i_player;
            uint32        i_text_emote;
            uint32        i_emote_num;
            Unit const*   i_target;
    };
}                                                           // namespace Trinity

void WorldSession::HandleTextEmoteOpcode(WorldPacket& recvData)
{
    if (!GetPlayer()->IsAlive())
    {
        recvData.rfinish();
        return;
    }

    if (!GetPlayer()->CanSpeak())
    {
        std::string timeStr = secsToTimeString(m_muteTime - time(NULL));
        SendNotification(GetTrinityString(LANG_WAIT_BEFORE_SPEAKING), timeStr.c_str());
        recvData.rfinish();
        return;
    }

    ObjectGuid guid;
    uint32 text_emote, emoteNum;

    recvData >> text_emote;
    recvData >> emoteNum;

    guid[6] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[6]);

    sScriptMgr->OnPlayerTextEmote(GetPlayer(), text_emote, emoteNum, guid);

    EmotesTextEntry const* em = sEmotesTextStore.LookupEntry(text_emote);
    if (!em)
        return;

    uint32 emote_anim = em->textid;

    switch (emote_anim)
    {
        case EMOTE_STATE_SLEEP:
        case EMOTE_STATE_SIT:
        case EMOTE_STATE_KNEEL:
        case EMOTE_ONESHOT_NONE:
            break;
        default:
            // Only allow text-emotes for "dead" entities (feign death included)
            if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
                break;
            GetPlayer()->HandleSwitchEmoteCommand(emote_anim);
            break;
    }

    Unit* unit = ObjectAccessor::GetUnit(*_player, guid);

    CellCoord p = Trinity::ComputeCellCoord(GetPlayer()->GetPositionX(), GetPlayer()->GetPositionY());

    Cell cell(p);
    cell.SetNoCreate();

    Trinity::EmoteChatBuilder emote_builder(*GetPlayer(), text_emote, emoteNum, unit);
    Trinity::LocalizedPacketDo<Trinity::EmoteChatBuilder > emote_do(emote_builder);
    Trinity::PlayerDistWorker<Trinity::LocalizedPacketDo<Trinity::EmoteChatBuilder > > emote_worker(GetPlayer(), sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_TEXTEMOTE), emote_do);
    TypeContainerVisitor<Trinity::PlayerDistWorker<Trinity::LocalizedPacketDo<Trinity::EmoteChatBuilder> >, WorldTypeMapContainer> message(emote_worker);
    cell.Visit(p, message, *GetPlayer()->GetMap(), *GetPlayer(), sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_TEXTEMOTE));

    // FIXME: I don't know why we need unit here...may be TARGET_ZONE is SOURCE_ZONE and vice versa
    GetPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_DO_EMOTE, text_emote, 0, 0, unit ? unit : GetPlayer());

    //Send scripted event call
    if (unit && unit->GetTypeId() == TYPEID_UNIT && ((Creature*)unit)->AI())
        ((Creature*)unit)->AI()->ReceiveEmote(GetPlayer(), text_emote);
}

void WorldSession::HandleChatIgnoredOpcode(WorldPacket& recvData)
{
    ObjectGuid guid;
    uint8 unk;
    //TC_LOG_DEBUG("network", "WORLD: Received CMSG_CHAT_IGNORED");

    recvData >> unk;                                       // probably related to spam reporting
    guid[5] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[5]);

    Player* player = ObjectAccessor::FindPlayer(guid);
    if (!player || !player->GetSession())
        return;

    WorldPacket data;
    ChatHandler::BuildChatPacket(data, CHAT_MSG_IGNORED, LANG_UNIVERSAL, _player, _player, GetPlayer()->GetName());
    player->GetSession()->SendPacket(&data);
}

void WorldSession::HandleChannelDeclineInvite(WorldPacket &recvPacket)
{
    TC_LOG_DEBUG("network", "Opcode %u", recvPacket.GetOpcode());
}

void WorldSession::SendPlayerNotFoundNotice(std::string const& name)
{
    WorldPacket data(SMSG_CHAT_PLAYER_NOT_FOUND, name.size()+2);
    data.WriteBits(name.size(), 9);
    data.FlushBits();
    data << name;
    SendPacket(&data);
}

void WorldSession::SendPlayerAmbiguousNotice(std::string const& name)
{
    WorldPacket data(SMSG_CHAT_PLAYER_AMBIGUOUS, name.size()+2);
    data.WriteBits(name.size(), 9);
    data.FlushBits();
    data << name;
    SendPacket(&data);
}

void WorldSession::SendChatRestrictedNotice(ChatRestrictionType restriction)
{
    WorldPacket data(SMSG_CHAT_RESTRICTED, 1);
    data << uint8(restriction);
    SendPacket(&data);
}
