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

#include "SocialMgr.h"

#include "DatabaseEnv.h"
#include "Opcodes.h"
#include "WorldPacket.h"
#include "Player.h"
#include "ObjectMgr.h"
#include "World.h"
#include "Util.h"
#include "WorldSession.h"

PlayerSocial::PlayerSocial(): m_playerGUID()
{ }

uint32 PlayerSocial::GetNumberOfSocialsWithFlag(SocialFlag flag)
{
    uint32 counter = 0;
    for (PlayerSocialMap::const_iterator itr = m_playerSocialMap.begin(); itr != m_playerSocialMap.end(); ++itr)
        if (itr->second.Flags & flag)
            ++counter;

    return counter;
}

bool PlayerSocial::AddToSocialList(ObjectGuid friendGuid, bool ignore)
{
    // check client limits
    if (ignore)
    {
        if (GetNumberOfSocialsWithFlag(SOCIAL_FLAG_IGNORED) >= SOCIALMGR_IGNORE_LIMIT)
            return false;
    }
    else
    {
        if (GetNumberOfSocialsWithFlag(SOCIAL_FLAG_FRIEND) >= SOCIALMGR_FRIEND_LIMIT)
            return false;
    }

    uint8 flag = SOCIAL_FLAG_FRIEND;
    if (ignore)
        flag = SOCIAL_FLAG_IGNORED;

    PlayerSocialMap::const_iterator itr = m_playerSocialMap.find(friendGuid);
    if (itr != m_playerSocialMap.end())
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ADD_CHARACTER_SOCIAL_FLAGS);

        stmt->setUInt8(0, flag);
        stmt->setUInt32(1, GetPlayerGUID());
        stmt->setUInt32(2, friendGuid);

        CharacterDatabase.Execute(stmt);

        m_playerSocialMap[friendGuid].Flags |= flag;
    }
    else
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHARACTER_SOCIAL);

        stmt->setUInt32(0, GetPlayerGUID());
        stmt->setUInt32(1, friendGuid);
        stmt->setUInt8(2, flag);

        CharacterDatabase.Execute(stmt);

        FriendInfo fi;
        fi.Flags |= flag;
        m_playerSocialMap[friendGuid] = fi;
    }
    return true;
}

void PlayerSocial::RemoveFromSocialList(ObjectGuid friendGuid, bool ignore)
{
    PlayerSocialMap::iterator itr = m_playerSocialMap.find(friendGuid);
    if (itr == m_playerSocialMap.end())                     // not exist
        return;

    uint8 flag = SOCIAL_FLAG_FRIEND;
    if (ignore)
        flag = SOCIAL_FLAG_IGNORED;

    itr->second.Flags &= ~flag;
    if (itr->second.Flags == 0)
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHARACTER_SOCIAL);

        stmt->setUInt32(0, GetPlayerGUID());
        stmt->setUInt32(1, friendGuid);

        CharacterDatabase.Execute(stmt);

        m_playerSocialMap.erase(itr);
    }
    else
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_REM_CHARACTER_SOCIAL_FLAGS);

        stmt->setUInt8(0, flag);
        stmt->setUInt32(1, GetPlayerGUID());
        stmt->setUInt32(2, friendGuid);

        CharacterDatabase.Execute(stmt);
    }
}

void PlayerSocial::SetFriendNote(ObjectGuid friendGuid, std::string note)
{
    PlayerSocialMap::const_iterator itr = m_playerSocialMap.find(friendGuid);
    if (itr == m_playerSocialMap.end())                     // not exist
        return;

    utf8truncate(note, 48);                                  // DB and client size limitation

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHARACTER_SOCIAL_NOTE);

    stmt->setString(0, note);
    stmt->setUInt32(1, GetPlayerGUID());
    stmt->setUInt32(2, friendGuid);

    CharacterDatabase.Execute(stmt);

    m_playerSocialMap[friendGuid].Note = note;
}

void PlayerSocial::SendSocialList(Player* player)
{
    if (!player)
        return;

    uint32 size = m_playerSocialMap.size();

    WorldPacket data(SMSG_CONTACT_LIST, 4 + 4 + (size * 33));
    data << uint32(7);                                      // 0x1 = Friendlist update. 0x2 = Ignorelist update. 0x4 = Mutelist update.
    data << uint32(size);                                   // friends count

    for (PlayerSocialMap::iterator itr = m_playerSocialMap.begin(); itr != m_playerSocialMap.end(); ++itr)
    {
        ObjectGuid guid = itr->first;
        sSocialMgr->GetFriendInfo(player, guid, itr->second);

        data << uint64(guid);                               // player guid
        data << uint32(0);
        data << uint32(0);
        data << uint32(itr->second.Flags);                  // player flag (0x1 = Friend, 0x2 = Ignored, 0x4 = Muted)
        data << itr->second.Note;                           // string note

        if (itr->second.Flags & SOCIAL_FLAG_FRIEND)         // if IsFriend()
        {
            data << uint8(itr->second.Status);              // online/offline/etc?
            if (itr->second.Status)                         // if online
            {
                data << uint32(itr->second.Area);           // player area
                data << uint32(itr->second.Level);          // player level
                data << uint32(itr->second.Class);          // player class
            }
        }
    }

    player->GetSession()->SendPacket(&data);
    TC_LOG_DEBUG("network", "WORLD: Sent SMSG_CONTACT_LIST");
}

bool PlayerSocial::HasFriend(ObjectGuid friendGuid)
{
    PlayerSocialMap::const_iterator itr = m_playerSocialMap.find(friendGuid);
    if (itr != m_playerSocialMap.end())
        return itr->second.Flags & SOCIAL_FLAG_FRIEND;
    return false;
}

bool PlayerSocial::HasIgnore(ObjectGuid ignore_guid)
{
    PlayerSocialMap::const_iterator itr = m_playerSocialMap.find(ignore_guid);
    if (itr != m_playerSocialMap.end())
        return itr->second.Flags & SOCIAL_FLAG_IGNORED;
    return false;
}

SocialMgr::SocialMgr() { }

SocialMgr::~SocialMgr() { }

void SocialMgr::GetFriendInfo(Player* player, ObjectGuid friendGUID, FriendInfo& friendInfo)
{
    if (!player)
        return;

    friendInfo.Status = FRIEND_STATUS_OFFLINE;
    friendInfo.Area = 0;
    friendInfo.Level = 0;
    friendInfo.Class = 0;

    Player* target = ObjectAccessor::FindConnectedPlayer(friendGUID);
    if (!target)
        return;

    PlayerSocialMap::iterator itr = player->GetSocial()->m_playerSocialMap.find(friendGUID);
    if (itr != player->GetSocial()->m_playerSocialMap.end())
        friendInfo.Note = itr->second.Note;

    // PLAYER see his team only and PLAYER can't see MODERATOR, GAME MASTER, ADMINISTRATOR characters
    // MODERATOR, GAME MASTER, ADMINISTRATOR can see all

    if (player->GetSession()->GetSecurity() == SEC_PLAYER && target->GetSession()->GetSecurity() > AccountTypes(sWorld->getIntConfig(CONFIG_GM_LEVEL_IN_WHO_LIST)))
        return;

    // player can see member of other team only if CONFIG_ALLOW_TWO_SIDE_WHO_LIST
    if (target->GetTeam() != player->GetTeam() && player->GetSession()->GetSecurity() == SEC_PLAYER)
        return;

    if (target->IsVisibleGloballyFor(player))
    {
        if (target->isDND())
            friendInfo.Status = FRIEND_STATUS_DND;
        else if (target->isAFK())
            friendInfo.Status = FRIEND_STATUS_AFK;
        else
            friendInfo.Status = FRIEND_STATUS_ONLINE;

        friendInfo.Area = target->GetZoneId();
        friendInfo.Level = target->GetLevel();
        friendInfo.Class = target->GetClass();
    }
}

void SocialMgr::MakeFriendStatusPacket(FriendsResult result, ObjectGuid guid, WorldPacket* data)
{
    data->Initialize(SMSG_FRIEND_STATUS, 9);
    *data << uint8(result);
    *data << uint64(guid);
}

void SocialMgr::SendFriendStatus(Player* player, FriendsResult result, ObjectGuid friendGuid, bool broadcast)
{
    FriendInfo fi;

    WorldPacket data;
    MakeFriendStatusPacket(result, friendGuid, &data);
    GetFriendInfo(player, friendGuid, fi);
    switch (result)
    {
        case FRIEND_ADDED_OFFLINE:
        case FRIEND_ADDED_ONLINE:
            data << fi.Note;
            break;
        default:
            break;
    }

    switch (result)
    {
        case FRIEND_ADDED_ONLINE:
        case FRIEND_ONLINE:
            data << uint8(fi.Status);
            data << uint32(fi.Area);
            data << uint32(fi.Level);
            data << uint32(fi.Class);
            break;
        default:
            break;
    }

    if (broadcast)
        BroadcastToFriendListers(player, &data);
    else
        player->GetSession()->SendPacket(&data);
}

void SocialMgr::BroadcastToFriendListers(Player* player, WorldPacket* packet)
{
    if (!player)
        return;

    AccountTypes gmSecLevel = AccountTypes(sWorld->getIntConfig(CONFIG_GM_LEVEL_IN_WHO_LIST));
    for (SocialMap::const_iterator itr = m_socialMap.begin(); itr != m_socialMap.end(); ++itr)
    {
        PlayerSocialMap::const_iterator itr2 = itr->second.m_playerSocialMap.find(player->GetGUID());
        if (itr2 != itr->second.m_playerSocialMap.end() && (itr2->second.Flags & SOCIAL_FLAG_FRIEND))
        {
            Player* target = ObjectAccessor::FindPlayer(itr->first);
            if (!target || !target->IsInWorld())
                continue;

            WorldSession* session = target->GetSession();
            if (player->GetSession()->GetSecurity() > gmSecLevel)
                continue;

            if (target->GetTeam() != player->GetTeam() && !session->GetSecurity() == SEC_PLAYER)
                continue;

            if (player->IsVisibleGloballyFor(target))
                session->SendPacket(packet);
        }
    }
}

PlayerSocial* SocialMgr::LoadFromDB(PreparedQueryResult result, ObjectGuid guid)
{
    PlayerSocial *social = &m_socialMap[guid];
    social->SetPlayerGUID(guid);

    if (!result)
        return social;

    ObjectGuid friendGuid = ObjectGuid::Empty;
    uint8 flags = 0;
    std::string note = "";

    do
    {
        Field* fields = result->Fetch();

        friendGuid = ObjectGuid(HighGuid::Player, fields[0].GetUInt32());
        flags = fields[1].GetUInt8();
        note = fields[2].GetString();

        social->m_playerSocialMap[friendGuid] = FriendInfo(flags, note);

        // client's friends list and ignore list limit
        if (social->m_playerSocialMap.size() >= (SOCIALMGR_FRIEND_LIMIT + SOCIALMGR_IGNORE_LIMIT))
            break;
    }
    while (result->NextRow());

    return social;
}
