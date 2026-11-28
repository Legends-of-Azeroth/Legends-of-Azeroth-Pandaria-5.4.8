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

#include "ChatPackets.h"

void WorldPackets::Chat::EmoteClient::Read()
{
    _worldPacket >> EmoteID;
}

void WorldPackets::Chat::CTextEmote::Read()
{
    _worldPacket >> EmoteID;
    _worldPacket >> SoundIndex;

    Target[6] = _worldPacket.ReadBit();
    Target[7] = _worldPacket.ReadBit();
    Target[3] = _worldPacket.ReadBit();
    Target[2] = _worldPacket.ReadBit();
    Target[0] = _worldPacket.ReadBit();
    Target[5] = _worldPacket.ReadBit();
    Target[1] = _worldPacket.ReadBit();
    Target[4] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Target[0]);
    _worldPacket.ReadByteSeq(Target[5]);
    _worldPacket.ReadByteSeq(Target[1]);
    _worldPacket.ReadByteSeq(Target[4]);
    _worldPacket.ReadByteSeq(Target[2]);
    _worldPacket.ReadByteSeq(Target[3]);
    _worldPacket.ReadByteSeq(Target[7]);
    _worldPacket.ReadByteSeq(Target[6]);
}

void WorldPackets::Chat::ChatReportIgnored::Read()
{
    _worldPacket >> Reason;

    IgnoredGUID[5] = _worldPacket.ReadBit();
    IgnoredGUID[0] = _worldPacket.ReadBit();
    IgnoredGUID[1] = _worldPacket.ReadBit();
    IgnoredGUID[3] = _worldPacket.ReadBit();
    IgnoredGUID[6] = _worldPacket.ReadBit();
    IgnoredGUID[7] = _worldPacket.ReadBit();
    IgnoredGUID[4] = _worldPacket.ReadBit();
    IgnoredGUID[2] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(IgnoredGUID[2]);
    _worldPacket.ReadByteSeq(IgnoredGUID[0]);
    _worldPacket.ReadByteSeq(IgnoredGUID[3]);
    _worldPacket.ReadByteSeq(IgnoredGUID[4]);
    _worldPacket.ReadByteSeq(IgnoredGUID[7]);
    _worldPacket.ReadByteSeq(IgnoredGUID[6]);
    _worldPacket.ReadByteSeq(IgnoredGUID[1]);
    _worldPacket.ReadByteSeq(IgnoredGUID[5]);
}

ChatMsg WorldPackets::Chat::ChatMessage::OpcToChatMsg(OpcodeClient opcode)
{
    switch (opcode)
    {
        case CMSG_MESSAGECHAT_SAY:            return CHAT_MSG_SAY;
        case CMSG_MESSAGECHAT_YELL:           return CHAT_MSG_YELL;
        case CMSG_MESSAGECHAT_PARTY:          return CHAT_MSG_PARTY;
        case CMSG_MESSAGECHAT_RAID:           return CHAT_MSG_RAID;
        case CMSG_MESSAGECHAT_RAID_WARNING:   return CHAT_MSG_RAID_WARNING;
        case CMSG_MESSAGECHAT_GUILD:          return CHAT_MSG_GUILD;
        case CMSG_MESSAGECHAT_OFFICER:        return CHAT_MSG_OFFICER;
        case CMSG_MESSAGECHAT_WHISPER:        return CHAT_MSG_WHISPER;
        case CMSG_MESSAGECHAT_EMOTE:          return CHAT_MSG_EMOTE;
        case CMSG_MESSAGECHAT_CHANNEL:        return CHAT_MSG_CHANNEL;
        case CMSG_MESSAGECHAT_AFK:            return CHAT_MSG_AFK;
        case CMSG_MESSAGECHAT_DND:            return CHAT_MSG_DND;
        case CMSG_MESSAGECHAT_INSTANCE:       return CHAT_MSG_INSTANCE_CHAT;
        case CMSG_MESSAGECHAT_ADDON_GUILD:    return CHAT_MSG_ADDON;
        case CMSG_MESSAGECHAT_ADDON_INSTANCE_CHAT: return CHAT_MSG_ADDON;
        case CMSG_MESSAGECHAT_ADDON_OFFICER:  return CHAT_MSG_ADDON;
        case CMSG_MESSAGECHAT_ADDON_PARTY:    return CHAT_MSG_ADDON;
        case CMSG_MESSAGECHAT_ADDON_RAID:     return CHAT_MSG_ADDON;
        case CMSG_MESSAGECHAT_ADDON_WHISPER:  return CHAT_MSG_ADDON;
        default:                              return CHAT_MSG_SAY;
    }
}

void WorldPackets::Chat::ChatMessage::Read()
{
    bool noLang = (SlashCmd == CHAT_MSG_EMOTE || SlashCmd == CHAT_MSG_AFK || SlashCmd == CHAT_MSG_DND);
    if (!noLang)
    {
        uint32 lang = 0;
        _worldPacket >> lang;
        Language = static_cast<::Language>(lang);
    }

    if (!noLang && Language == LANG_ADDON)
    {
        _worldPacket >> Text;
        return;
    }

    uint32 textLength = 0;
    uint32 receiverLength = 0;
    switch (SlashCmd)
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
            textLength = _worldPacket.ReadBits(8);
            Text = _worldPacket.ReadString(textLength);
            break;
        case CHAT_MSG_WHISPER:
            textLength = _worldPacket.ReadBits(8);
            receiverLength = _worldPacket.ReadBits(9);
            Text = _worldPacket.ReadString(textLength);
            Target = _worldPacket.ReadString(receiverLength);
            break;
        case CHAT_MSG_CHANNEL:
            textLength = _worldPacket.ReadBits(9);
            receiverLength = _worldPacket.ReadBits(8);
            Text = _worldPacket.ReadString(receiverLength);
            Target = _worldPacket.ReadString(textLength);
            break;
        case CHAT_MSG_AFK:
        case CHAT_MSG_DND:
            textLength = _worldPacket.ReadBits(8);
            Text = _worldPacket.ReadString(textLength);
            break;
        default:
            break;
    }
}
