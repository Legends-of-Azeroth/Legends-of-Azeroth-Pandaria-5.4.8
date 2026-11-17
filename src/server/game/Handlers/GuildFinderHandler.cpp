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

#include "WorldSession.h"
#include "WorldPacket.h"
#include "Object.h"
#include "SharedDefines.h"
#include "GuildFinderMgr.h"
#include "GuildMgr.h"
#include "AchievementMgr.h"
#include "GuildFinderPackets.h"

void WorldSession::HandleGuildFinderAddApplication(WorldPackets::GuildFinder::LFGuildAddApplication& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_LF_GUILD_ADD_APPLICATION");

    if (sGuildFinderMgr->GetAllMembershipRequestsForPlayer(GetPlayer()->GetGUID().GetCounter()).size() == 10)
        return;

    uint32 classRoles = packet.ClassRoles;
    uint32 availability = packet.Availability;
    uint32 guildInterests = packet.GuildInterests;
    ObjectGuid guid = packet.GuildGuid;
    std::string comment = packet.Comment;

    uint32 guildLowGuid = guid.GetCounter();

    if (!guid.IsGuild())
        return;
    if (!(classRoles & GUILDFINDER_ALL_ROLES) || classRoles > GUILDFINDER_ALL_ROLES)
        return;
    if (!(availability & AVAILABILITY_ALWAYS) || availability > AVAILABILITY_ALWAYS)
        return;
    if (!(guildInterests & ALL_INTERESTS) || guildInterests > ALL_INTERESTS)
        return;

    MembershipRequest request = MembershipRequest(GetPlayer()->GetGUID(), guildLowGuid, availability, classRoles, guildInterests, comment, time(NULL));
    sGuildFinderMgr->AddMembershipRequest(guildLowGuid, request);
}

void WorldSession::HandleGuildFinderBrowse(WorldPackets::GuildFinder::LFGuildBrowse& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_LF_GUILD_BROWSE");
    uint32 classRoles = packet.ClassRoles;
    uint32 availability = packet.Availability;
    uint32 guildInterests = packet.GuildInterests;
    uint32 playerLevel = packet.PlayerLevel; // Raw player level (1-85), do they use MAX_FINDER_LEVEL when on level 85 ?

    if (!(classRoles & GUILDFINDER_ALL_ROLES) || classRoles > GUILDFINDER_ALL_ROLES)
        return;
    if (!(availability & AVAILABILITY_ALWAYS) || availability > AVAILABILITY_ALWAYS)
        return;
    if (!(guildInterests & ALL_INTERESTS) || guildInterests > ALL_INTERESTS)
        return;
    if (playerLevel > sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL) || playerLevel < 1)
        return;

    Player* player = GetPlayer();

    LFGuildPlayer settings(player->GetGUID().GetCounter(), classRoles, availability, guildInterests, ANY_FINDER_LEVEL);
    LFGuildStore guildList = sGuildFinderMgr->GetGuildsMatchingSetting(settings, player->GetTeamId());
    uint32 guildCount = guildList.size();

    if (guildCount == 0)
    {
        WorldPacket packet(SMSG_LF_GUILD_BROWSE_UPDATED);
        packet.WriteBits(guildCount, 18);
        packet.FlushBits();
        player->SendDirectMessage(&packet);
        return;
    }

    ByteBuffer bufferData;
    WorldPacket data(SMSG_LF_GUILD_BROWSE_UPDATED, 3 + guildCount * 65); // Estimated size

    data.WriteBits(guildCount, 18);

    for (LFGuildStore::const_iterator itr = guildList.begin(); itr != guildList.end(); ++itr)
    {
        LFGuildSettings guildSettings = itr->second;
        Guild* guild = sGuildMgr->GetGuildById(itr->first);

        ObjectGuid guildGUID = ObjectGuid(guild->GetGUID());

        data.WriteBit(guildGUID[6]);
        data.WriteBit(guildGUID[5]);
        data.WriteBit(guildGUID[4]);
        data.WriteBit(guildGUID[0]);
        data.WriteBit(guildGUID[1]);
        data.WriteBits(guildSettings.GetComment().size(), 10);
        data.WriteBit(guildGUID[3]);
        data.WriteBits(guild->GetName().size(), 7);
        data.WriteBit(guildGUID[7]);
        data.WriteBit(guildGUID[2]);

        bufferData.WriteByteSeq(guildGUID[3]);
        bufferData << uint32(guild->GetEmblemInfo().GetBackgroundColor());
        bufferData << uint8(sGuildFinderMgr->HasRequest(player->GetGUID(), guild->GetGUID())); // Request pending
        bufferData.WriteByteSeq(guildGUID[0]);
        bufferData << uint32(guild->GetAchievementMgr().GetAchievementPoints());
        bufferData.WriteByteSeq(guildGUID[2]);
        bufferData << uint32(guildSettings.GetInterests());
        bufferData << uint32(guild->GetEmblemInfo().GetStyle());
        bufferData << uint32(guild->GetLevel());
        bufferData << uint32(guildSettings.GetAvailability());
        bufferData << uint32(guildSettings.GetClassRoles());
        bufferData.WriteByteSeq(guildGUID[5]);
        bufferData << uint32(0);                                                                  // Unk
        bufferData.WriteString(guild->GetName());
        bufferData << uint32(guild->GetEmblemInfo().GetColor());
        bufferData << uint8(0);                                                                   // Cached
        bufferData.WriteByteSeq(guildGUID[7]);
        bufferData << uint32(guild->GetEmblemInfo().GetBorderColor());
        bufferData.WriteByteSeq(guildGUID[6]);
        bufferData << uint32(50397223);                                                           // Unk Flags
        bufferData.WriteString(guildSettings.GetComment());
        bufferData << uint32(guild->GetEmblemInfo().GetBorderStyle());
        bufferData << uint32(guild->GetMembersCount());
        bufferData.WriteByteSeq(guildGUID[1]);
        bufferData.WriteByteSeq(guildGUID[4]);
    }

    data.FlushBits();
    data.append(bufferData);

    player->SendDirectMessage(&data);
}

void WorldSession::HandleGuildFinderDeclineRecruit(WorldPackets::GuildFinder::LFGuildDeclineRecruit& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_LF_GUILD_DECLINE_RECRUIT");

    ObjectGuid playerGuid = packet.PlayerGuid;

    if (!playerGuid.IsPlayer())
        return;

    sGuildFinderMgr->RemoveMembershipRequest(playerGuid.GetCounter(), GetPlayer()->GetGuildId());
}

void WorldSession::HandleGuildFinderGetApplications(WorldPackets::GuildFinder::LFGuildGetApplications&)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_LF_GUILD_GET_APPLICATIONS"); // Empty opcode

    std::list<MembershipRequest> applicatedGuilds = sGuildFinderMgr->GetAllMembershipRequestsForPlayer(GetPlayer()->GetGUID().GetCounter());
    uint32 applicationsCount = applicatedGuilds.size();
    WorldPacket data(SMSG_LF_GUILD_MEMBERSHIP_LIST_UPDATED, 7 + 54 * applicationsCount);
    data.WriteBits(applicationsCount, 19);

    if (applicationsCount > 0)
    {
        ByteBuffer bufferData;
        for (std::list<MembershipRequest>::const_iterator itr = applicatedGuilds.begin(); itr != applicatedGuilds.end(); ++itr)
        {
            Guild* guild = sGuildMgr->GetGuildById(itr->GetGuildId());
            if (!guild) return;
            LFGuildSettings guildSettings = sGuildFinderMgr->GetGuildSettings(itr->GetGuildId());
            MembershipRequest request = *itr;
            
            
            ObjectGuid guildGuid = ObjectGuid(guild->GetGUID());

            data.WriteBit(guildGuid[0]);
            data.WriteBit(guildGuid[4]);
            data.WriteBit(guildGuid[2]);
            data.WriteBit(guildGuid[7]);
            data.WriteBits(guild->GetName().size(), 7);
            data.WriteBit(guildGuid[2]);
            data.WriteBit(guildGuid[3]);
            data.WriteBits(request.GetComment().size(), 10);
            data.WriteBit(guildGuid[6]);
            data.WriteBit(guildGuid[5]);

            bufferData << uint32(guildSettings.GetInterests());
            bufferData << uint32(50397223); // unk Flags
            bufferData.WriteString(guild->GetName());
            bufferData.WriteByteSeq(guildGuid[4]);
            bufferData << uint32(guildSettings.GetClassRoles());
            bufferData.WriteByteSeq(guildGuid[6]);
            bufferData.WriteByteSeq(guildGuid[5]);
            bufferData << uint32(time(NULL) - request.GetSubmitTime()); // Time since application (seconds)
            bufferData.WriteByteSeq(guildGuid[1]);
            bufferData.WriteByteSeq(guildGuid[3]);
            bufferData.WriteByteSeq(guildGuid[0]);
            bufferData.WriteByteSeq(guildGuid[7]);
            bufferData.WriteByteSeq(guildGuid[2]);
            bufferData << uint32(request.GetExpiryTime() - time(NULL)); // Time left to application expiry (seconds)
            bufferData << uint32(guildSettings.GetAvailability());
            bufferData.WriteString(request.GetComment());
        }

        data.FlushBits();
        data.append(bufferData);
    }
    else
        data.FlushBits();

    data << uint32(10 - sGuildFinderMgr->CountRequestsFromPlayer(GetPlayer()->GetGUID().GetCounter())); // Applications count left

    GetPlayer()->SendDirectMessage(&data);
}

// Lists all recruits for a guild - Misses times
void WorldSession::HandleGuildFinderGetRecruits(WorldPackets::GuildFinder::LFGuildGetRecruits&)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_LF_GUILD_GET_RECRUITS");

    Player* player = GetPlayer();
    if (!player->GetGuildId())
        return;

    std::vector<MembershipRequest> recruitsList = sGuildFinderMgr->GetAllMembershipRequestsForGuild(player->GetGuildId());
    uint32 recruitCount = 0;

    ByteBuffer dataBuffer;
    WorldPacket data(SMSG_LF_GUILD_RECRUIT_LIST_UPDATED, 7 + 26 * recruitCount + 53 * recruitCount);
    data.WriteBits(recruitCount, 18);

    for (auto&& request : recruitsList)
    {
        auto nameData = sWorld->GetCharacterNameData(request.GetPlayerGUID());
        if (!nameData)
            continue;

        ++recruitCount;

        ObjectGuid playerGuid = request.GetPlayerGUID();

        data.WriteBit(playerGuid[7]);
        data.WriteBits(nameData->m_name.size(), 6);
        data.WriteBit(playerGuid[1]);
        data.WriteBit(playerGuid[0]);
        data.WriteBit(playerGuid[2]);
        data.WriteBit(playerGuid[4]);
        data.WriteBit(playerGuid[3]);
        data.WriteBit(playerGuid[6]);
        data.WriteBits(request.GetComment().size(), 10);
        data.WriteBit(playerGuid[5]);

        dataBuffer.WriteByteSeq(playerGuid[2]);
        dataBuffer.WriteByteSeq(playerGuid[7]);
        dataBuffer.WriteByteSeq(playerGuid[1]);
        dataBuffer.WriteString(request.GetComment());
        dataBuffer << int32(time(NULL) - request.GetSubmitTime()); // Time in seconds since application submitted.
        dataBuffer.WriteByteSeq(playerGuid[0]);
        dataBuffer << int32(0);
        dataBuffer << int32(nameData->m_class);
        dataBuffer << int32(request.GetClassRoles());
        dataBuffer.WriteByteSeq(playerGuid[4]);
        dataBuffer << int32(request.GetExpiryTime() - time(NULL)); // TIme in seconds until application expires.
        dataBuffer << int32(nameData->m_level);
        dataBuffer.WriteByteSeq(playerGuid[3]);
        dataBuffer.WriteString(nameData->m_name);
        dataBuffer << int32(time(NULL) <= request.GetExpiryTime());
        dataBuffer.WriteByteSeq(playerGuid[6]);
        dataBuffer.WriteByteSeq(playerGuid[5]);
        dataBuffer << int32(request.GetAvailability());
        dataBuffer << int32(request.GetInterests());
    }

    data.FlushBits();
    data.PutBits(0, recruitCount, 18);
    data.append(dataBuffer);
    data << uint32(time(NULL));

    player->SendDirectMessage(&data);
}

void WorldSession::HandleGuildFinderPostRequest(WorldPackets::GuildFinder::LFGuildPostRequest&)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_LF_GUILD_POST_REQUEST"); // Empty opcode

    Player* player = GetPlayer();

    if (!player->GetGuildId()) // Player must be in guild
        return;

    bool isGuildMaster = true;
    if (Guild* guild = sGuildMgr->GetGuildById(player->GetGuildId()))
        if (guild->GetLeaderGUID() != player->GetGUID())
            isGuildMaster = false;

    LFGuildSettings settings = sGuildFinderMgr->GetGuildSettings(player->GetGuildId());

    WorldPacket data(SMSG_LF_GUILD_POST_UPDATED, 35);
    data.WriteBit(isGuildMaster); // Guessed

    if (isGuildMaster)
    {
        data.WriteBit(settings.IsListed());
        data.WriteBits(settings.GetComment().size(), 10);

        data.FlushBits();

        data.WriteString(settings.GetComment());
        data << uint32(settings.GetInterests());
        data << uint32(settings.GetClassRoles());
        data << uint32(settings.GetLevel());
        data << uint32(0); // Unk Int32
        data << uint32(settings.GetAvailability());
    }
    else
        data.FlushBits();

    player->SendDirectMessage(&data);
}

void WorldSession::HandleGuildFinderRemoveApplication(WorldPackets::GuildFinder::LFGuildRemoveApplication& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_LF_GUILD_REMOVE_APPLICATION");

    ObjectGuid guildGuid = packet.GuildGuid;

    if (!guildGuid.IsGuild())
        return;

    sGuildFinderMgr->RemoveMembershipRequest(GetPlayer()->GetGUID().GetCounter(), guildGuid.GetCounter());
}

// Sent any time a guild master sets an option in the interface and when listing / unlisting his guild
void WorldSession::HandleGuildFinderSetGuildPost(WorldPackets::GuildFinder::LFGuildSetGuildPost& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_LF_GUILD_SET_GUILD_POST");

    uint32 classRoles = packet.ClassRoles;
    uint32 availability = packet.Availability;
    uint32 guildInterests = packet.GuildInterests;
    uint32 level = packet.Level;

    // Level sent is zero if untouched, force to any (from interface). Idk why
    if (!level)
        level = ANY_FINDER_LEVEL;

    bool listed = packet.Listed;
    std::string comment = packet.Comment;

    if (!(classRoles & GUILDFINDER_ALL_ROLES) || classRoles > GUILDFINDER_ALL_ROLES)
        return;
    if (!(availability & AVAILABILITY_ALWAYS) || availability > AVAILABILITY_ALWAYS)
        return;
    if (!(guildInterests & ALL_INTERESTS) || guildInterests > ALL_INTERESTS)
        return;
    if (!(level & ALL_GUILDFINDER_LEVELS) || level > ALL_GUILDFINDER_LEVELS)
        return;

    Player* player = GetPlayer();

    if (!player->GetGuildId()) // Player must be in guild
        return;

    if (Guild* guild = sGuildMgr->GetGuildById(player->GetGuildId())) // Player must be guild master
        if (guild->GetLeaderGUID() != player->GetGUID())
            return;

    LFGuildSettings settings(listed, player->GetTeamId(), player->GetGuildId(), classRoles, availability, guildInterests, level, comment);
    sGuildFinderMgr->SetGuildSettings(player->GetGuildId(), settings);
}
