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

#include "ObjectMgr.h"                                      // for normalizePlayerName
#include "ChannelMgr.h"
#include "Player.h"
#include "WorldSession.h"
#include "Language.h"
#include "Chat.h"
#include "WordFilterMgr.h"
#include "../../dep/utf8cpp/utf8.h"

bool WorldSession::ChannelCheck(std::string channel)
{
    if (channel.size() >= 100)
        return false;

    if (channel.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890") != std::string::npos)
        return false;

    return true;
}

void WorldSession::HandleJoinChannel(WorldPacket& recvPacket)
{

    uint32 channelId = recvPacket.read<uint32>();
    recvPacket.ReadBit(); // unk bit
    uint32 channelLength = recvPacket.ReadBits(7);
    uint32 passLength = recvPacket.ReadBits(7);
    recvPacket.ReadBit(); // unk bit

    std::string password = recvPacket.ReadString(passLength);
    std::string channelName = recvPacket.ReadString(channelLength);

    TC_LOG_DEBUG("chat.system", "CMSG_JOIN_CHANNEL %s Channel: %u, channel: %s, password: %s",
        GetPlayerInfo().c_str(), channelId, channelName.c_str(), password.c_str());

    if (channelId)
    {
        ChatChannelsEntry const* channel = sChatChannelsStore.LookupEntry(channelId);
        if (!channel)
            return;

        AreaTableEntry const* zone = sAreaTableStore.LookupEntry(GetPlayer()->GetZoneId());
        if (!zone || !GetPlayer()->CanJoinConstantChannelInZone(channel, zone))
            return;

        if (!(channel->flags & CHANNEL_DBC_FLAG_GLOBAL))
        {
            char new_channel_name_buf[200];
            char const* currentNameExt;

            if (channel->flags & CHANNEL_DBC_FLAG_CITY_ONLY)
            {
                if (channelName.find("|") != std::string::npos || channelName.size() >= 100 || !utf8::is_valid(channelName.begin(), channelName.end()))
                {
                    char cityName[200];
                    if (!sscanf(channelName.c_str(), channel->pattern, cityName) &&
                        !sscanf(channelName.c_str(), channel->pattern, cityName) ||
                        strcmp(sObjectMgr->GetTrinityString(LANG_CHANNEL_CITY, GetSessionDbcLocale()), cityName))
                        return;
                }
                currentNameExt = sObjectMgr->GetTrinityString(LANG_CHANNEL_CITY, LocaleConstant(sWorld->getIntConfig(CONFIG_CHANNEL_CONSTANT_LOCALE)));
            }
            else
            {
                if (channelName.find("|") != std::string::npos || channelName.size() >= 100 || !utf8::is_valid(channelName.begin(), channelName.end()))
                {
                    char zoneName[200];
                    if (!sscanf(channelName.c_str(), channel->pattern, zoneName) &&
                        !sscanf(channelName.c_str(), channel->pattern, zoneName) ||
                        strcmp(zone->area_name, zoneName))
                        return;
                }
                currentNameExt = zone->area_name;
            }

            snprintf(new_channel_name_buf, 200, channel->pattern, currentNameExt);
            channelName = new_channel_name_buf;
        }
        else
            channelName = channel->pattern[sWorld->getIntConfig(CONFIG_CHANNEL_CONSTANT_LOCALE)];
    }
    else
    {
        std::wstring channelNameUpper;
        bool converted = Utf8toWStr(channelName, channelNameUpper);
        wstrToUpper(channelNameUpper);
        for (uint32 id = 1; id < sChatChannelsStore.GetNumRows(); ++id)
        {
            for (uint32 locale = 0; locale < TOTAL_LOCALES; ++locale)
            {
                if (ChatChannelsEntry const* channel = sChatChannelsStore.LookupEntry(id))
                {
                    if (channel->name && *channel->name)
                    {
                        std::string dbcName = channel->name;
                        std::wstring dbcNameUpper;
                        if (converted && Utf8toWStr(dbcName, dbcNameUpper) ? wstrToUpper(dbcNameUpper), !channelNameUpper.compare(dbcNameUpper) : !channelName.compare(dbcName))
                        {
                            if (GetSessionDbLocaleIndex() == LOCALE_ruRU)
                                ChatHandler(this).PSendSysMessage("|cFFFF0000Вы пытаетесь создать пользовательский канал чата с зарезервированным названием. Для входа в желаемый канал используйте команду \"/войти %s\".|r", channel->name);
                            else if (channel->flags & CHANNEL_DBC_FLAG_LFG)
                                ChatHandler(this).PSendSysMessage("|cFFFF0000You are trying to create a custom chat channel with a reserved name. Please use \"/join %s\" command to enter the desired channel. If you are using a patch, that translates the name of the channel to \"%s\", be aware that it is no longer required and can be deleted, but if you're still using it, the command you want to use is \"/join %s\".|r", channel->name, channel->name, channel->name);
                            else
                                ChatHandler(this).PSendSysMessage("|cFFFF0000You are trying to create a custom chat channel with a reserved name. Please use \"/join %s\" command to enter the desired channel.|r", channel->name);

                            return;
                        }
                    }
                }
            }
        }
    }

    if (!utf8::is_valid(channelName.begin(), channelName.end()))
    {
        return;
    }

    if (channelName.empty())
        return;

    if (!ChatHandler::ValidatePipeSequence(channelName))
        return;

    if (ChannelMgr* cMgr = ChannelMgr::forTeam(GetPlayer()->GetTeam()))
    {
        cMgr->setTeam(GetPlayer()->GetTeam());
        if (Channel* channel = cMgr->GetJoinChannel(channelName, channelId))
            channel->JoinChannel(GetPlayer(), password);
    }
}

void WorldSession::HandleLeaveChannel(WorldPacket& recvPacket)
{
    uint32 unk;
    std::string channelName;
    recvPacket >> unk;                                      // channel id?
    uint32 length = recvPacket.ReadBits(7);
    channelName = recvPacket.ReadString(length);

    TC_LOG_DEBUG("chat.system", "CMSG_LEAVE_CHANNEL %s Channel: %s, unk1: %u",
        GetPlayerInfo().c_str(), channelName.c_str(), unk);

    if (channelName.empty())
        return;

    if (ChannelMgr* cMgr = ChannelMgr::forTeam(GetPlayer()->GetTeam()))
    {
        if (Channel* channel = cMgr->GetChannel(channelName, GetPlayer()))
            channel->LeaveChannel(GetPlayer(), true);
        cMgr->LeftChannel(channelName);
    }
}

void WorldSession::HandleChannelList(WorldPacket& recvPacket)
{
    uint32 length = recvPacket.ReadBits(7);
    std::string channelName = recvPacket.ReadString(length);

    TC_LOG_DEBUG("chat.system", "%s %s Channel: %s",
        "CMSG_CHANNEL_LIST",
        GetPlayerInfo().c_str(), channelName.c_str()); // recvPacket.GetOpcode() == CMSG_CHANNEL_DISPLAY_LIST ? "CMSG_CHANNEL_DISPLAY_LIST" : "CMSG_CHANNEL_LIST"

    if (ChannelMgr* cMgr = ChannelMgr::forTeam(GetPlayer()->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, GetPlayer()))
            channel->List(GetPlayer());
}

void WorldSession::HandleChannelPassword(WorldPacket& recvPacket)
{
    uint32 passLength = recvPacket.ReadBits(7);
    uint32 nameLength = recvPacket.ReadBits(7);

    std::string password = recvPacket.ReadString(passLength);
    std::string channelName = recvPacket.ReadString(nameLength);

    TC_LOG_DEBUG("chat.system", "CMSG_CHANNEL_PASSWORD %s Channel: %s, Password: %s",
        GetPlayerInfo().c_str(), channelName.c_str(), password.c_str());

    if (ChannelMgr* cMgr = ChannelMgr::forTeam(GetPlayer()->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, GetPlayer()))
            channel->Password(GetPlayer(), password);
}

void WorldSession::HandleChannelSetOwner(WorldPacket& recvPacket)
{
    uint32 nameLength = recvPacket.ReadBits(8) << 1;
    nameLength += recvPacket.ReadBit();
    uint32 channelLength = recvPacket.ReadBits(7);

    std::string channelName = recvPacket.ReadString(channelLength);
    std::string targetName = recvPacket.ReadString(nameLength);

    TC_LOG_DEBUG("chat.system", "CMSG_CHANNEL_SET_OWNER %s Channel: %s, Target: %s",
        GetPlayerInfo().c_str(), channelName.c_str(), targetName.c_str());

    if (!normalizePlayerName(targetName))
        return;

    if (ChannelMgr* cMgr = ChannelMgr::forTeam(GetPlayer()->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, GetPlayer()))
            channel->SetOwner(GetPlayer(), targetName);
}

void WorldSession::HandleChannelOwner(WorldPacket& recvPacket)
{
    uint32 length = recvPacket.ReadBits(7);
    std::string channelName = recvPacket.ReadString(length);

    TC_LOG_DEBUG("chat.system", "CMSG_CHANNEL_OWNER %s Channel: %s",
        GetPlayerInfo().c_str(), channelName.c_str());

    if (ChannelMgr* cMgr = ChannelMgr::forTeam(GetPlayer()->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, GetPlayer()))
            channel->SendWhoOwner(GetPlayer()->GetGUID());
}

void WorldSession::HandleChannelModerator(WorldPacket& recvPacket)
{
    uint32 channelLength = recvPacket.ReadBits(7);
    uint32 nameLength = recvPacket.ReadBits(8) << 1;
    nameLength += recvPacket.ReadBit();

    std::string channelName = recvPacket.ReadString(channelLength);
    std::string targetName = recvPacket.ReadString(nameLength);

    TC_LOG_DEBUG("chat.system", "CMSG_CHANNEL_MODERATOR %s Channel: %s, Target: %s",
        GetPlayerInfo().c_str(), channelName.c_str(), targetName.c_str());

    if (!normalizePlayerName(targetName))
        return;

    if (ChannelMgr* cMgr = ChannelMgr::forTeam(GetPlayer()->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, GetPlayer()))
            channel->SetModerator(GetPlayer(), targetName);
}

void WorldSession::HandleChannelUnmoderator(WorldPacket& recvPacket)
{
    uint32 channelLength = recvPacket.ReadBits(7);
    uint32 nameLength = recvPacket.ReadBits(8) << 1;
    nameLength += recvPacket.ReadBit();

    std::string targetName = recvPacket.ReadString(nameLength);
    std::string channelName = recvPacket.ReadString(channelLength);

    TC_LOG_DEBUG("chat.system", "CMSG_CHANNEL_UNMODERATOR %s Channel: %s, Target: %s",
        GetPlayerInfo().c_str(), channelName.c_str(), targetName.c_str());

    if (!normalizePlayerName(targetName))
        return;

    if (ChannelMgr* cMgr = ChannelMgr::forTeam(GetPlayer()->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, GetPlayer()))
            channel->UnsetModerator(GetPlayer(), targetName);
}

void WorldSession::HandleChannelMute(WorldPacket& recvPacket)
{
    uint32 channelLength = recvPacket.ReadBits(7);
    uint32 nameLength = recvPacket.ReadBits(8) << 1;
    nameLength += recvPacket.ReadBit();

    std::string targetName = recvPacket.ReadString(nameLength);
    std::string channelName = recvPacket.ReadString(channelLength);

    TC_LOG_DEBUG("chat.system", "CMSG_CHANNEL_MUTE %s Channel: %s, Target: %s",
        GetPlayerInfo().c_str(), channelName.c_str(), targetName.c_str());

    if (!normalizePlayerName(targetName))
        return;

    if (ChannelMgr* cMgr = ChannelMgr::forTeam(GetPlayer()->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, GetPlayer()))
            channel->SetMute(GetPlayer(), targetName);
}

void WorldSession::HandleChannelUnmute(WorldPacket& recvPacket)
{
    uint32 nameLength = recvPacket.ReadBits(8) << 1;
    nameLength += recvPacket.ReadBit();
    uint32 channelLength = recvPacket.ReadBits(7);

    std::string targetName = recvPacket.ReadString(nameLength);
    std::string channelName = recvPacket.ReadString(channelLength);

    TC_LOG_DEBUG("chat.system", "CMSG_CHANNEL_UNMUTE %s Channel: %s, Target: %s",
        GetPlayerInfo().c_str(), channelName.c_str(), targetName.c_str());

    if (!normalizePlayerName(targetName))
        return;

    if (ChannelMgr* cMgr = ChannelMgr::forTeam(GetPlayer()->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, GetPlayer()))
            channel->UnsetMute(GetPlayer(), targetName);
}

void WorldSession::HandleChannelInvite(WorldPacket& recvPacket)
{
    uint32 nameLength = recvPacket.ReadBits(8) << 1;
    nameLength += recvPacket.ReadBit();
    uint32 channelLength = recvPacket.ReadBits(7);

    std::string channelName = recvPacket.ReadString(channelLength);
    std::string targetName = recvPacket.ReadString(nameLength);

    TC_LOG_DEBUG("chat.system", "CMSG_CHANNEL_INVITE %s Channel: %s, Target: %s",
        GetPlayerInfo().c_str(), channelName.c_str(), targetName.c_str());

    if (!normalizePlayerName(targetName))
        return;

    if (ChannelMgr* cMgr = ChannelMgr::forTeam(GetPlayer()->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, GetPlayer()))
            channel->Invite(GetPlayer(), targetName);
}

void WorldSession::HandleChannelKick(WorldPacket& recvPacket)
{
    uint32 channelLength = recvPacket.ReadBits(7);
    uint32 nameLength = recvPacket.ReadBits(8) << 1;
    nameLength += recvPacket.ReadBit();

    std::string channelName = recvPacket.ReadString(channelLength);
    std::string targetName = recvPacket.ReadString(nameLength);

    TC_LOG_DEBUG("chat.system", "CMSG_CHANNEL_KICK %s Channel: %s, Target: %s",
        GetPlayerInfo().c_str(), channelName.c_str(), targetName.c_str());

    if (!normalizePlayerName(targetName))
        return;

    if (ChannelMgr* cMgr = ChannelMgr::forTeam(GetPlayer()->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, GetPlayer()))
            channel->Kick(GetPlayer(), targetName);
}

void WorldSession::HandleChannelBan(WorldPacket& recvPacket)
{
    uint32 nameLength = recvPacket.ReadBits(8) << 1;
    nameLength += recvPacket.ReadBit();
    uint32 channelLength = recvPacket.ReadBits(7);

    std::string targetName = recvPacket.ReadString(nameLength);
    std::string channelName = recvPacket.ReadString(channelLength);

    TC_LOG_DEBUG("chat.system", "CMSG_CHANNEL_BAN %s Channel: %s, Target: %s",
        GetPlayerInfo().c_str(), channelName.c_str(), targetName.c_str());

    if (!normalizePlayerName(targetName))
        return;

    if (ChannelMgr* cMgr = ChannelMgr::forTeam(GetPlayer()->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, GetPlayer()))
            channel->Ban(GetPlayer(), targetName);
}

void WorldSession::HandleChannelUnban(WorldPacket& recvPacket)
{
    uint32 nameLength = recvPacket.ReadBits(8) << 1;
    nameLength += recvPacket.ReadBit();
    uint32 channelLength = recvPacket.ReadBits(7);

    std::string targetName = recvPacket.ReadString(nameLength);
    std::string channelName = recvPacket.ReadString(channelLength);

    TC_LOG_DEBUG("chat.system", "CMSG_CHANNEL_UNBAN %s Channel: %s, Target: %s",
        GetPlayerInfo().c_str(), channelName.c_str(), targetName.c_str());

    if (!normalizePlayerName(targetName))
        return;

    if (ChannelMgr* cMgr = ChannelMgr::forTeam(GetPlayer()->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, GetPlayer()))
            channel->UnBan(GetPlayer(), targetName);
}

void WorldSession::HandleChannelAnnouncements(WorldPacket& recvPacket)
{
    uint32 length = recvPacket.ReadBits(7);
    std::string channelName = recvPacket.ReadString(length);

    TC_LOG_DEBUG("chat.system", "CMSG_CHANNEL_ANNOUNCEMENTS %s Channel: %s",
        GetPlayerInfo().c_str(), channelName.c_str());

    if (ChannelMgr* cMgr = ChannelMgr::forTeam(GetPlayer()->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, GetPlayer()))
            channel->Announce(GetPlayer());
}

void WorldSession::HandleChannelDisplayListQuery(WorldPacket &recvPacket)
{
    // this should be OK because the 2 function _were_ the same
    HandleChannelList(recvPacket);
}

// not in 5.4.8
// void WorldSession::HandleGetChannelMemberCount(WorldPacket &recvPacket)
// {
//     std::string channelName;
//     recvPacket >> channelName;

//     TC_LOG_DEBUG("chat.system", "CMSG_GET_CHANNEL_MEMBER_COUNT %s Channel: %s",
//         GetPlayerInfo().c_str(), channelName.c_str());

//     if (ChannelMgr* cMgr = ChannelMgr::forTeam(GetPlayer()->GetTeam()))
//     {
//         if (Channel* channel = cMgr->GetChannel(channelName, GetPlayer()))
//         {
//             TC_LOG_DEBUG("chat.system", "SMSG_CHANNEL_MEMBER_COUNT %s Channel: %s Count: %u",
//                 GetPlayerInfo().c_str(), channelName.c_str(), channel->GetNumPlayers());

//             WorldPacket data(SMSG_CHANNEL_MEMBER_COUNT, channel->GetName().size() + 1 + 4);
//             data << channel->GetName();
//             data << uint8(channel->GetFlags());
//             data << uint32(channel->GetNumPlayers());
//             SendPacket(&data);
//         }
//     }
// }

void WorldSession::HandleSetChannelWatch(WorldPacket& recvPacket)
{
    std::string channelName;
    recvPacket >> channelName;

    TC_LOG_DEBUG("chat.system", "CMSG_SET_CHANNEL_WATCH %s Channel: %s",
        GetPlayerInfo().c_str(), channelName.c_str());

    /*
    if (ChannelMgr* cMgr = channelMgr(GetPlayer()->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, GetPlayer()))
            channel->JoinNotify(GetPlayer());
    */
}
