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

#include "LFGMgr.h"
#include "ObjectMgr.h"
#include "Group.h"
#include "Player.h"
#include "Opcodes.h"
#include "WorldPacket.h"
#include "WorldSession.h"

void WorldSession::HandleLfgJoinOpcode(WorldPacket& recvData)
{
    if (!sLFGMgr->isOptionEnabled(lfg::LFG_OPTION_ENABLE_DUNGEON_FINDER | lfg::LFG_OPTION_ENABLE_RAID_BROWSER))
    {
        recvData.rfinish();
        return;
    }
    Group* group = GetPlayer()->GetGroup();
    if (group)
    {
        // Disallow joining LFG if there are more players in the group than the limit is for dungeons/raids
        if (group->GetMembersCount() > lfg::GetGroupSize(group->isRaidGroup()))
        {
            recvData.rfinish();
            return;
        }
        if (group->isLFGGroup())
        {
            // Check LFG state of the group
            switch (sLFGMgr->GetActiveState(group->GetGUID()))
            {
                case lfg::LFG_STATE_FINISHED_DUNGEON:
                    // Allow joining LFG after the dungeon is finished
                    break;
                case lfg::LFG_STATE_DUNGEON:
                    // Allow joining LFG during the dungeon only if there are members mising
                    if (group->GetMembersCount() < lfg::GetGroupSize(group->isRaidGroup()))
                        break;
                default:
                    // Disallow in all other cases
                    recvData.rfinish();
                    return;
            }
        }
        else
        {
            // Allow joining LFG only if the packet sender is the leader of the group
            if (GetPlayer()->GetGUID() != group->GetLeaderGUID())
            {
                recvData.rfinish();
                return;
            }
        }
    }

    uint8 unk0 = recvData.read<uint8>();
    for (int32 i = 0; i < 3; ++i)
        recvData.read_skip<uint32>();
    uint32 roles =  recvData.read<uint32>();

    uint32 numDungeons = recvData.ReadBits(22);
    uint32 commentLen = recvData.ReadBits(8);
    bool unk1 = recvData.ReadBit();

    if (!numDungeons)
    {
        TC_LOG_DEBUG("network", "CMSG_LFG_JOIN {} no dungeons selected", GetPlayerInfo().c_str());
        recvData.rfinish();
        return;
    }

    lfg::LfgDungeonSet newDungeons;
    for (uint32 i = 0; i < numDungeons; ++i)
    {
        uint32 dungeon = recvData.read<uint32>();
        dungeon &= 0xFFFFFF;
        newDungeons.insert(dungeon);        // remove the type from the dungeon entry
    }

    std::string comment = recvData.ReadString(commentLen);

    TC_LOG_TRACE("network", "CMSG_LFG_JOIN {} roles: {}, Dungeons: {}, Comment: {}",
        GetPlayerInfo().c_str(), roles, uint8(newDungeons.size()), comment.c_str());

    sLFGMgr->JoinLfg(GetPlayer(), lfg::LfgRoles(roles), newDungeons, comment);
}

void WorldSession::HandleLfgLeaveOpcode(WorldPacket& recvData)
{
    ObjectGuid leaveGuid;
    Group* group = GetPlayer()->GetGroup();
    ObjectGuid guid = GetPlayer()->GetGUID();
    ObjectGuid gguid = group ? group->GetGUID() : guid;
    uint32 queueId;

    recvData.read_skip<uint32>();                          // Always 8
    recvData.read_skip<uint32>();                          // Join date
    recvData.read_skip<uint32>();                          // Always 3
    recvData >> queueId;

    leaveGuid[1] = recvData.ReadBit();
    leaveGuid[6] = recvData.ReadBit();
    leaveGuid[0] = recvData.ReadBit();
    leaveGuid[7] = recvData.ReadBit();
    leaveGuid[2] = recvData.ReadBit();
    leaveGuid[4] = recvData.ReadBit();
    leaveGuid[3] = recvData.ReadBit();
    leaveGuid[5] = recvData.ReadBit();

    recvData.ReadByteSeq(leaveGuid[4]);
    recvData.ReadByteSeq(leaveGuid[5]);
    recvData.ReadByteSeq(leaveGuid[2]);
    recvData.ReadByteSeq(leaveGuid[6]);
    recvData.ReadByteSeq(leaveGuid[1]);
    recvData.ReadByteSeq(leaveGuid[3]);
    recvData.ReadByteSeq(leaveGuid[7]);
    recvData.ReadByteSeq(leaveGuid[0]);

    TC_LOG_DEBUG("network", "CMSG_LFG_LEAVE {} in group: {} sent guid " "{}" ".",
        GetPlayerInfo().c_str(), group ? 1 : 0, uint64(leaveGuid));

    // Probably uselss
    queueId = sLFGMgr->ConvertToServerQueueId(guid, queueId);
    if (!queueId)
        return;

    // Check cheating - only leader can leave the queue
    if (!group || group->GetLeaderGUID() == guid)
        sLFGMgr->LeaveLfg(gguid, queueId);
}

void WorldSession::HandleLfgProposalResultOpcode(WorldPacket& recvData)
{
    uint32 proposalID;  // Proposal ID
    uint32 queueId;
    bool accept;

    ObjectGuid guid1;
    ObjectGuid guid2;

    recvData >> proposalID >> queueId;
    recvData.read_skip<uint32>(); // unk flags (always 3)
    recvData.read_skip<uint32>(); // join time

    accept = recvData.ReadBit();

    guid1[6] = recvData.ReadBit();
    guid1[0] = recvData.ReadBit();
    guid1[2] = recvData.ReadBit();
    guid1[4] = recvData.ReadBit();
    guid2[6] = recvData.ReadBit();
    guid2[7] = recvData.ReadBit();
    guid1[3] = recvData.ReadBit();
    guid2[4] = recvData.ReadBit();
    guid1[7] = recvData.ReadBit();
    guid2[1] = recvData.ReadBit();
    guid1[5] = recvData.ReadBit();
    guid2[0] = recvData.ReadBit();
    guid1[1] = recvData.ReadBit();
    guid2[2] = recvData.ReadBit();
    guid2[3] = recvData.ReadBit();
    guid2[5] = recvData.ReadBit();

    recvData.ReadByteSeq(guid1[3]);
    recvData.ReadByteSeq(guid1[6]);
    recvData.ReadByteSeq(guid1[4]);
    recvData.ReadByteSeq(guid1[1]);
    recvData.ReadByteSeq(guid2[7]);
    recvData.ReadByteSeq(guid2[0]);
    recvData.ReadByteSeq(guid1[7]);
    recvData.ReadByteSeq(guid2[6]);
    recvData.ReadByteSeq(guid1[5]);
    recvData.ReadByteSeq(guid2[3]);
    recvData.ReadByteSeq(guid2[1]);
    recvData.ReadByteSeq(guid2[5]);
    recvData.ReadByteSeq(guid2[4]);
    recvData.ReadByteSeq(guid1[0]);
    recvData.ReadByteSeq(guid1[2]);
    recvData.ReadByteSeq(guid2[2]);

    TC_LOG_DEBUG("network", "CMSG_LFG_PROPOSAL_RESULT {} proposal: {} accept: {}",
        GetPlayerInfo().c_str(), proposalID, accept ? 1 : 0);
    sLFGMgr->UpdateProposal(proposalID, GetPlayer()->GetGUID(), accept);
}

void WorldSession::HandleLfgSetRolesOpcode(WorldPacket& recvData)
{
    uint32 roles = recvData.read<uint32>();
    recvData.read_skip<uint8>();
    ObjectGuid guid = GetPlayer()->GetGUID();
    Group* group = GetPlayer()->GetGroup();
    if (!group)
    {
        TC_LOG_DEBUG("network", "CMSG_LFG_SET_ROLES {} Not in group",
            GetPlayerInfo().c_str());
        return;
    }
    ObjectGuid gguid = group->GetGUID();
    TC_LOG_DEBUG("network", "CMSG_LFG_SET_ROLES: Group {}, Player {}, Roles: {}",
        gguid.GetCounter(), GetPlayerInfo().c_str(), roles);
    sLFGMgr->UpdateRoleCheck(gguid, guid, roles);
}

void WorldSession::HandleLfgSetBootVoteOpcode(WorldPacket& recvData)
{
    bool agree = recvData.ReadBit();                       // Agree to kick player

    ObjectGuid guid = GetPlayer()->GetGUID();
    TC_LOG_DEBUG("network", "CMSG_LFG_SET_BOOT_VOTE {} agree: {}",
        GetPlayerInfo().c_str(), agree ? 1 : 0);
    sLFGMgr->UpdateBoot(guid, agree);
}

void WorldSession::HandleLfgTeleportOpcode(WorldPacket& recvData)
{
    bool out = recvData.ReadBit();

    TC_LOG_DEBUG("network", "CMSG_LFG_TELEPORT {} out: {}",
        GetPlayerInfo().c_str(), out ? 1 : 0);
    sLFGMgr->TeleportPlayer(GetPlayer(), out, true);
}

void WorldSession::HandleLfgGetLockInfoOpcode(WorldPacket& recvData)
{
    recvData.read_skip<uint8>();
    bool forPlayer = recvData.ReadBit();
    TC_LOG_DEBUG("network", "CMSG_LFG_LOCK_INFO_REQUEST {} for {}", GetPlayerInfo().c_str(), (forPlayer ? "player" : "party"));

    if (forPlayer)
        SendLfgPlayerLockInfo();
    else
        SendLfgPartyLockInfo();
}

void WorldSession::SendLfgPlayerLockInfo()
{
    if (!GetPlayer())
        return;

    ObjectGuid guid = GetPlayer()->GetGUID();

    // Get Random dungeons that can be done at a certain level and expansion
    uint8 level = GetPlayer()->GetLevel();
    lfg::LfgDungeonSet const& randomDungeons = sLFGMgr->GetRandomAndSeasonalDungeons(level, GetPlayer()->GetSession()->Expansion());

    // Get player locked Dungeons
    lfg::LfgLockMap const& lockMap = sLFGMgr->GetLockedDungeons(guid);
    uint32 rsize = uint32(randomDungeons.size());
    uint32 lsize = uint32(lockMap.size());
    bool hasPlayer = true;

    TC_LOG_DEBUG("network", "SMSG_LFG_PLAYER_INFO {}", GetPlayerInfo().c_str());
    ByteBuffer lfgData;
    WorldPacket data(SMSG_LFG_PLAYER_INFO, 1 + rsize * (4 + 1 + 4 + 4 + 4 + 4 + 1 + 4 + 4 + 4) + 4 + lsize * (1 + 4 + 4 + 4 + 4 + 1 + 4 + 4 + 4));

    data.WriteBits(lockMap.size(), 20);
    data.WriteBit(hasPlayer);
    data.WriteBits(randomDungeons.size(), 17);
    for (auto&& dungeon : randomDungeons)
    {
        uint32 dungeonId = dungeon & 0x00FFFFFF;

        Quest const* quest = nullptr;
        bool done = false;
        if (lfg::LfgReward const* reward = sLFGMgr->GetRandomDungeonReward(dungeon, level))
        {
            quest = sObjectMgr->GetQuestTemplate(reward->firstQuest);
            if (quest)
            {
                done = !GetPlayer()->CanRewardQuest(quest, false);
                if (done)
                    quest = sObjectMgr->GetQuestTemplate(reward->otherQuest);
            }
        }

        Quest const* ctaRoleQuest = nullptr;

        uint32 enc_mask = 0;
        bool has_cta_reward = false;
        if (lfg::LFGDungeonData const* lfg_dungeon = sLFGMgr->GetLFGDungeon(dungeonId))
        {
            ctaRoleQuest = sObjectMgr->GetQuestTemplate(lfg_dungeon && lfg_dungeon->difficulty == RAID_DIFFICULTY_25MAN_LFR ? QUEST_LFR_SHORTAGE : QUEST_LFG_SHORTAGE);

            if (lfg_dungeon->map)
            {
                if (DungeonEncounterList const* encounters = sObjectMgr->GetDungeonEncounterList(lfg_dungeon->map, lfg_dungeon->difficulty))
                {
                    for (auto&& itr : *encounters)
                    {
                        if (itr->creditType == ENCOUNTER_CREDIT_KILL_CREATURE)
                        {
                            if ((itr->creditEntry == 60400 && GetPlayer()->HasLfrLockout(60400)) || // Will of Emperor
                                (itr->creditEntry == 71475 && GetPlayer()->HasLfrLockout(71475)))   // The Fallen Protectors
                            {
                                enc_mask |= 1 << itr->dbcEntry->encounterIndex;
                                continue;
                            }

                            if (GetPlayer()->HasLfrLockout(itr->creditEntry))
                                enc_mask |= 1 << itr->dbcEntry->encounterIndex;
                        }
                        else if (itr->creditType == ENCOUNTER_CREDIT_CAST_SPELL)
                        {
                            switch (itr->creditEntry)
                            {
                                case 104574: // Spine of Deathwing
                                    if (GetPlayer()->HasLfrLockout(210222))
                                        enc_mask |= 1 << itr->dbcEntry->encounterIndex;
                                    break;
                                case 111533: // Madness of Deathwing
                                    if (GetPlayer()->HasLfrLockout(210079))
                                        enc_mask |= 1 << itr->dbcEntry->encounterIndex;
                                    break;
                                case 123900: // Protectors of the Endless
                                    if (GetPlayer()->HasLfrLockout(60583) || GetPlayer()->HasLfrLockout(60586) || GetPlayer()->HasLfrLockout(60585))
                                        enc_mask |= 1 << itr->dbcEntry->encounterIndex;
                                    break;
                                case 123901: // Tsulong
                                    if (GetPlayer()->HasLfrLockout(62442))
                                        enc_mask |= 1 << itr->dbcEntry->encounterIndex;
                                    break;
                                case 123903: // Lei Shi
                                    if (GetPlayer()->HasLfrLockout(62983))
                                        enc_mask |= 1 << itr->dbcEntry->encounterIndex;
                                    break;
                                case 128288: // The Stone Guard
                                    if (GetPlayer()->HasLfrLockout(59915))
                                        enc_mask |= 1 << itr->dbcEntry->encounterIndex;
                                    break;
                                case 140353: // Megaera
                                    if (GetPlayer()->HasLfrLockout(70212))
                                        enc_mask |= 1 << itr->dbcEntry->encounterIndex;
                                    break;
                                case 145904: // Spoils of Pandaria
                                case 148512: // Paragons of the Klaxxi
                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                }
            }

            if ((lfg_dungeon->type == lfg::LFG_TYPE_RANDOM || lfg_dungeon->difficulty == RAID_DIFFICULTY_25MAN_LFR) && lfg_dungeon->difficulty != SCENARIO_DIFFICULTY_NORMAL && lfg_dungeon->difficulty != SCENARIO_DIFFICULTY_HEROIC)
                has_cta_reward = true;
        }

        bool cta_eligible = !GetPlayer()->GetGroup();
        uint8 cta_shortage_size = cta_eligible ? lfg::LFG_ROLE_SHORTAGE_MAX : 0;

        data.WriteBit(!done);  // First Reward
        data.WriteBit(cta_eligible);
        data.WriteBits(0, 21); // additional currency (found only for heroic scenarios)
        data.WriteBits(cta_shortage_size, 19);
        data.WriteBits(quest ? quest->GetRewItemsCount() : 0, 20);

        for (uint32 i = 0; i < cta_shortage_size; ++i)
        {
            data.WriteBits(0, 21);
            data.WriteBits(has_cta_reward && ctaRoleQuest && i == lfg::LFG_ROLE_SHORTAGE_RARE ? ctaRoleQuest->GetRewItemsCount() : 0, 20); // Call to Arms rewards size
            data.WriteBits(0, 21);
        }

        data.WriteBits(quest ? quest->GetRewCurrencyCount() : 0, 21);

        lfgData << uint32(GetPlayer()->GetLevel() == DEFAULT_MAX_LEVEL ? 0 : (quest ? quest->XPValue(GetPlayer()) : 0));

        for (uint32 i = 0; i < cta_shortage_size; ++i)
        {
            lfgData << uint32(0);

            /*for (int8 i = 0; i < 0; i++) // unk cta currency reward
            {
                lfgData << uint32(0);
                lfgData << uint32(0);
            }*/

            if (has_cta_reward && ctaRoleQuest && ctaRoleQuest->GetRewItemsCount() && i == lfg::LFG_ROLE_SHORTAGE_RARE)
            {
                for (uint8 j = 0; j < QUEST_REWARD_ITEM_COUNT; ++j)
                {
                    if (!ctaRoleQuest->RewardItemId[j])
                        continue;

                    if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(ctaRoleQuest->RewardItemId[j]))
                    {
                        lfgData << uint32(ctaRoleQuest->RewardItemId[j]);
                        lfgData << uint32(ctaRoleQuest->RewardItemCount[j]);
                        lfgData << uint32(proto->DisplayInfoID);
                    }
                }
            }

            lfgData << uint32(sLFGMgr->GetEligibleRolesForCTA(GetPlayer()->GetGUID(), dungeonId));

            /*for (int8 i = 0; i < 0; i++) // unk cta currency reward
            {
                lfgData << uint32(0);
                lfgData << uint32(0);
            }*/

            lfgData << uint32(0);
        }

        lfgData << uint32(0);

        for (int8 i = 0; i < 0; i++) // additional currency for heroic scenarios
        {
            lfgData << uint32(0); // CurrencyId
            lfgData << uint32(0); // CurrencyCount
        }

        lfgData << uint32(0);
        lfgData << uint32(quest ? quest->GetRewOrReqMoney(GetPlayer()) : 0);

        if (quest && quest->GetRewItemsCount())
        {
            for (uint8 i = 0; i < QUEST_REWARD_ITEM_COUNT; ++i)
            {
                if (!quest->RewardItemId[i])
                    continue;

                if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(quest->RewardItemId[i]))
                {
                    lfgData << uint32(quest->RewardItemCount[i]);
                    lfgData << uint32(quest->RewardItemId[i]);
                    lfgData << uint32(proto->DisplayInfoID);
                }
            }
        }

        lfgData << uint32(0);
        lfgData << uint32(0);
        lfgData << uint32(1);
        lfgData << uint32(1);
        lfgData << uint32(0);
        lfgData << uint32(dungeon);

        if (quest && quest->GetRewCurrencyCount())
        {
            for (uint8 i = 0; i < QUEST_REWARD_CURRENCY_COUNT; ++i)
            {
                if (!quest->RewardCurrencyId[i])
                    continue;

                lfgData << uint32(quest->RewardCurrencyId[i]);
                lfgData << uint32(quest->RewardCurrencyCount[i] * 100);
            }
        }

        lfgData << uint32(0);
        lfgData << int32(0); // 1388689 / -355205120 wtf?
        lfgData << uint32(0);
        lfgData << uint32(1);
        lfgData << uint32(1);
        lfgData << uint32(enc_mask); // completed encounters
        lfgData << uint32(1);
    }

    if (hasPlayer)
    {
        data.WriteBit(guid[5]);
        data.WriteBit(guid[1]);
        data.WriteBit(guid[2]);
        data.WriteBit(guid[7]);
        data.WriteBit(guid[3]);
        data.WriteBit(guid[0]);
        data.WriteBit(guid[6]);
        data.WriteBit(guid[4]);

        data.WriteByteSeq(guid[7]);
        data.WriteByteSeq(guid[2]);
        data.WriteByteSeq(guid[3]);
        data.WriteByteSeq(guid[0]);
        data.WriteByteSeq(guid[4]);
        data.WriteByteSeq(guid[5]);
        data.WriteByteSeq(guid[6]);
        data.WriteByteSeq(guid[1]);
    }

    data.FlushBits();
    data.append(lfgData);

    for (auto&& lock : lockMap)
    {
        data << uint32(lock.first);                          // Dungeon entry (id + type)
        data << uint32(lock.second.lockStatus);              // Lock status
        data << uint32(lock.second.requiredItemLevel);       // Required itemLevel
        data << uint32(lock.second.currentItemLevel);        // Current itemLevel
    }

    SendPacket(&data);
}

void WorldSession::SendLfgPartyLockInfo()
{
    uint64 guid = GetPlayer()->GetGUID();
    Group* group = GetPlayer()->GetGroup();
    if (!group)
        return;

    uint32 count = 0;
    bool hasPlayerInfo = true;

    TC_LOG_DEBUG("network", "SMSG_LFG_PARTY_INFO {}", GetPlayerInfo().c_str());
    WorldPacket data(SMSG_LFG_PARTY_INFO);
    ByteBuffer bytes;

    size_t bitpos = data.bitwpos();
    data.WriteBits(0, 22); // placeholder
    for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
    {
        ObjectGuid pguid = itr->GetSource()->GetGUID();

        if (pguid == guid)
            continue;

        data.WriteBit(hasPlayerInfo);
        if (hasPlayerInfo)
        {
            data.WriteBit(pguid[3]);
            data.WriteBit(pguid[6]);
            data.WriteBit(pguid[0]);
            data.WriteBit(pguid[5]);
            data.WriteBit(pguid[2]);
            data.WriteBit(pguid[7]);
            data.WriteBit(pguid[4]);
            data.WriteBit(pguid[1]);
        }

        lfg::LfgLockMap lockMap = sLFGMgr->GetLockedDungeons(pguid);

        data.WriteBits(lockMap.size(), 20);

        for (lfg::LfgLockMap::const_iterator it = lockMap.begin(); it != lockMap.end(); ++it)
        {
            bytes << uint32(it->second.currentItemLevel);        // Current itemLevel
            bytes << uint32(it->first);                          // Dungeon entry (id + type)
            bytes << uint32(it->second.requiredItemLevel);       // Required itemLevel
            bytes << uint32(it->second.lockStatus);              // Lock status
        }
        if (hasPlayerInfo)
        {
            bytes.WriteByteSeq(pguid[3]);
            bytes.WriteByteSeq(pguid[6]);
            bytes.WriteByteSeq(pguid[0]);
            bytes.WriteByteSeq(pguid[5]);
            bytes.WriteByteSeq(pguid[2]);
            bytes.WriteByteSeq(pguid[7]);
            bytes.WriteByteSeq(pguid[4]);
            bytes.WriteByteSeq(pguid[1]);
        }

        count++;
    }
    data.FlushBits();
    data.PutBits(bitpos, count, 22);
    if (bytes.size())
        data.append(bytes);
    SendPacket(&data);
}

void WorldSession::HandleLfrJoinOpcode(WorldPacket& recvData)
{
    uint32 entry;                                          // Raid id to search
    recvData >> entry;
    TC_LOG_DEBUG("network", "CMSG_LFG_LFR_JOIN {} dungeon entry: {}",
        GetPlayerInfo().c_str(), entry);
    //SendLfrUpdateListOpcode(entry);
}

void WorldSession::HandleLfrLeaveOpcode(WorldPacket& recvData)
{
    uint32 dungeonId;                                      // Raid id queue to leave
    recvData >> dungeonId;
    TC_LOG_DEBUG("network", "CMSG_LFG_LFR_LEAVE {} dungeonId: {}",
        GetPlayerInfo().c_str(), dungeonId);
    //sLFGMgr->LeaveLfr(GetPlayer(), dungeonId);
}

void WorldSession::HandleLfgGetStatus(WorldPacket& /*recvData*/)
{
    TC_LOG_DEBUG("network", "CMSG_LFG_GET_STATUS {}", GetPlayerInfo().c_str());

    if (!GetPlayer()->IsUsingLfg())
        return;

    if (auto queues = sLFGMgr->GetPlayerQueues(GetPlayer()->GetGUID()))
        for (auto&& itr : *queues)
            SendLfgUpdateStatus(lfg::LFG_UPDATETYPE_UPDATE_STATUS, itr.second);
}

void WorldSession::SendLfgUpdateStatus(lfg::LfgUpdateType updateType, lfg::PlayerQueueData const& queueData)
{
    bool join = false;
    bool queued = false;
  
    ObjectGuid guid = queueData.OriginalGroup ? queueData.OriginalGroup : GetPlayer()->GetGUID();

    std::string comment;    // useless, just placeholder

    switch (updateType)
    {
        case lfg::LFG_UPDATETYPE_JOIN_QUEUE_INITIAL:            // Joined queue outside the dungeon
            join = true;
            break;
        case lfg::LFG_UPDATETYPE_JOIN_QUEUE:
        case lfg::LFG_UPDATETYPE_ADDED_TO_QUEUE:                // Rolecheck Success
            join = true;
            queued = true;
            break;
        case lfg::LFG_UPDATETYPE_PROPOSAL_BEGIN:
        case lfg::LFG_UPDATETYPE_UNK_52:
            join = true;
            break;
        case lfg::LFG_UPDATETYPE_UPDATE_STATUS:
            // With multiqueue this is unknown :( but "join" causes bugs if "!queued"
            join = queueData.State == lfg::LFG_STATE_QUEUED;
            queued = queueData.State == lfg::LFG_STATE_QUEUED;
            break;
        default:
            break;
    }

    TC_LOG_DEBUG("network", "SMSG_LFG_UPDATE_STATUS {} updatetype: {}", GetPlayerInfo().c_str(), updateType);

    WorldPacket data(SMSG_LFG_UPDATE_STATUS, 1 + 8 + 3 + 2 + 1 + comment.length() + 4 + 4 + 1 + 1 + 1 + 4 + queueData.Dungeons.size());
    data.WriteBits(comment.size(), 8);
    data.WriteBit(true);                                   // always true
    data.WriteBit(join);
    data.WriteBits(queueData.Dungeons.size(), 22);

    data.WriteBit(guid[2]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[1]);
    data.WriteBit(join);                                   // always as previous bit
    data.WriteBit(guid[7]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[0]);
    data.WriteBit(true);                                   // no, not always true
    data.WriteBit(queued);
    data.WriteBits(0, 24);                                 // if party members
    data.WriteBit(guid[5]);

    data.WriteBit(guid[4]);

    data.WriteByteSeq(guid[3]);

    for (int i = 0; i < 3; ++i)
        data << uint8(0);

    data.WriteByteSeq(guid[4]);

    data.WriteByteSeq(guid[6]);

    data << uint8(updateType);
    data << uint32(queueData.Roles);
    data << uint32(queueData.ClientQueueId);

    data.WriteByteSeq(guid[5]);
    data.WriteString(comment);
    data.WriteByteSeq(guid[2]);

    for (auto&& it : queueData.Dungeons)
    {
        LFGDungeonEntry const* dungeon = sLFGDungeonStore.LookupEntry(it);
        data << uint32(dungeon ? dungeon->Entry() : 0);
    }

    uint8 category = 0;
    if (!queueData.Dungeons.empty())
        if (auto entry = sLFGDungeonStore.LookupEntry(*queueData.Dungeons.begin() & 0xFFFFFF)) // all dungeons in array have one category
            category = entry->category;

    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[1]);
    data << uint32(queueData.JoinTime);
    data << uint8(category);                               // Dungeon Category
    data << uint32(3);
    data.WriteByteSeq(guid[7]);

    SendPacket(&data);
}

void WorldSession::SendLfgRoleChosen(ObjectGuid guid, uint8 roles)
{
    TC_LOG_DEBUG("network", "SMSG_LFG_ROLE_CHOSEN {} guid: {} roles: {}",
        GetPlayerInfo().c_str(), guid.GetCounter(), roles);

    ObjectGuid pGuid = guid;
    WorldPacket data(SMSG_LFG_ROLE_CHOSEN, 8 + 1 + 4);
    data.WriteBit(pGuid[6]);
    data.WriteBit(pGuid[2]);
    data.WriteBit(pGuid[1]);
    data.WriteBit(pGuid[7]);
    data.WriteBit(pGuid[0]);              // Guid Bits
    data.WriteBit(roles > 0);             // Ready
    data.WriteBit(pGuid[3]);
    data.WriteBit(pGuid[5]);
    data.WriteBit(pGuid[4]);              // Guid Bits

    data.FlushBits();

    data.WriteByteSeq(pGuid[0]);
    data.WriteByteSeq(pGuid[3]);
    data.WriteByteSeq(pGuid[6]);           // Guid Bytes
    data << uint32(roles);                 // Roles
    data.WriteByteSeq(pGuid[5]);
    data.WriteByteSeq(pGuid[1]);
    data.WriteByteSeq(pGuid[4]);
    data.WriteByteSeq(pGuid[2]);
    data.WriteByteSeq(pGuid[7]);           // Guid Bytes
    SendPacket(&data);
}

void WorldSession::SendLfgRoleCheckUpdate(lfg::LfgRoleCheck const& roleCheck)
{
    lfg::LfgDungeonSet dungeons;
    if (roleCheck.rDungeonId)
        dungeons.insert(roleCheck.rDungeonId);
    else
        dungeons = roleCheck.dungeons;

    TC_LOG_DEBUG("network", "SMSG_LFG_ROLE_CHECK_UPDATE {}", GetPlayerInfo().c_str());

    ByteBuffer groupData;
    ObjectGuid randomDungeonGuid = ObjectGuid::Empty;
    WorldPacket data(SMSG_LFG_ROLE_CHECK_UPDATE, 4 + 1 + 1 + dungeons.size() * 4 + 1 + roleCheck.roles.size() * (8 + 1 + 4 + 1));

    data << uint8(0);                                            // Party Index
    data << uint8(roleCheck.state);                              // Check result
    data.WriteBits(roleCheck.roles.size(), 21);                  // Players in group

    if (!roleCheck.roles.empty())
    {
        // Leader info MUST be sent 1st :S
        ObjectGuid guid = roleCheck.leader;
        uint8 roles = roleCheck.roles.find(guid)->second;
        Player* player = ObjectAccessor::FindPlayer(guid);
        data.WriteBit(roles > 0);     // Ready
        data.WriteBit(guid[3]);
        data.WriteBit(guid[0]);
        data.WriteBit(guid[5]);
        data.WriteBit(guid[2]);
        data.WriteBit(guid[7]);
        data.WriteBit(guid[1]);
        data.WriteBit(guid[4]);
        data.WriteBit(guid[6]);        // Guid Bits

        groupData << uint8(player ? player->GetLevel() : 0);     // Level
        groupData.WriteByteSeq(guid[3]);
        groupData.WriteByteSeq(guid[6]);                    // Guid Bytes
        groupData << uint32(roles);                              // Roles
        groupData.WriteByteSeq(guid[2]);
        groupData.WriteByteSeq(guid[4]);
        groupData.WriteByteSeq(guid[0]);
        groupData.WriteByteSeq(guid[1]);
        groupData.WriteByteSeq(guid[5]);
        groupData.WriteByteSeq(guid[7]);        // Guid Bytes

        for (lfg::LfgRolesMap::const_iterator it = roleCheck.roles.begin(); it != roleCheck.roles.end(); ++it)
        {
            if (it->first == roleCheck.leader)
                continue;

            guid = it->first;
            roles = it->second;
            player = ObjectAccessor::FindPlayer(guid);

            data.WriteBit(roles > 0);                            // Ready
            data.WriteBit(guid[3]);
            data.WriteBit(guid[0]);
            data.WriteBit(guid[5]);
            data.WriteBit(guid[2]);
            data.WriteBit(guid[7]);
            data.WriteBit(guid[1]);
            data.WriteBit(guid[4]);
            data.WriteBit(guid[6]);    // Guid Bits

            groupData << uint8(player ? player->GetLevel() : 0); // Level
            groupData.WriteByteSeq(guid[3]);
            groupData.WriteByteSeq(guid[6]);                // Guid Bytes
            groupData << uint32(roles);                          // Roles
            groupData.WriteByteSeq(guid[2]);
            groupData.WriteByteSeq(guid[4]);
            groupData.WriteByteSeq(guid[0]);
            groupData.WriteByteSeq(guid[1]);
            groupData.WriteByteSeq(guid[5]);
            groupData.WriteByteSeq(guid[7]);    // Guid Bytes
        }
    }

    data.WriteBit(randomDungeonGuid[3]);
    data.WriteBit(randomDungeonGuid[5]);
    data.WriteBits(dungeons.size(), 22);                         // Number of dungeons
    data.WriteBit(randomDungeonGuid[0]);
    data.WriteBit(randomDungeonGuid[7]);
    data.WriteBit(randomDungeonGuid[6]);
    data.WriteBit(randomDungeonGuid[1]);
    data.WriteBit(randomDungeonGuid[4]);
    data.WriteBit(randomDungeonGuid[2]);
    data.WriteBit(roleCheck.state == lfg::LFG_ROLECHECK_INITIALITING);

    data.FlushBits();

    data.WriteByteSeq(randomDungeonGuid[0]);
    data.append(groupData);
    data.WriteByteSeq(randomDungeonGuid[1]);
    data.WriteByteSeq(randomDungeonGuid[7]);
    data.WriteByteSeq(randomDungeonGuid[6]);
    data.WriteByteSeq(randomDungeonGuid[4]);
    data.WriteByteSeq(randomDungeonGuid[3]);
    data.WriteByteSeq(randomDungeonGuid[2]);
    data.WriteByteSeq(randomDungeonGuid[5]);

    if (!dungeons.empty())
        for (lfg::LfgDungeonSet::iterator it = dungeons.begin(); it != dungeons.end(); ++it)
            data << uint32(sLFGMgr->GetLFGDungeonEntry(*it)); // Dungeon

    SendPacket(&data);
}

void WorldSession::SendLfgJoinResult(uint32 queueId, lfg::LfgJoinResultData const& joinData)
{
    lfg::PlayerQueueData const* queueData = nullptr;
    if (queueId)
        queueData = &sLFGMgr->GetPlayerQueueData(GetPlayer()->GetGUID(), queueId);

    uint32 size = 0;
    ObjectGuid guid = queueData && queueData->OriginalGroup ? queueData->OriginalGroup : GetPlayer()->GetGUID();
    for (lfg::LfgLockPartyMap::const_iterator it = joinData.lockmap.begin(); it != joinData.lockmap.end(); ++it)
        size += 8 + 4 + uint32(it->second.size()) * (4 + 4 + 4 + 4);

    TC_LOG_DEBUG("network", "SMSG_LFG_JOIN_RESULT {} checkResult: {} checkValue: {}",
        GetPlayerInfo().c_str(), joinData.result, joinData.state);


    WorldPacket data(SMSG_LFG_JOIN_RESULT, 4 + 4 + size);

    data.WriteBit(guid[7]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[0]);
    data.WriteBits(joinData.lockmap.size(), 22);

    for (lfg::LfgLockPartyMap::const_iterator it = joinData.lockmap.begin(); it != joinData.lockmap.end(); ++it)
    {
        ObjectGuid playerGuid = it->first;
        data.WriteBit(playerGuid[3]);
        data.WriteBits(it->second.size(), 20);
        data.WriteBit(playerGuid[6]);
        data.WriteBit(playerGuid[1]);
        data.WriteBit(playerGuid[4]);
        data.WriteBit(playerGuid[7]);
        data.WriteBit(playerGuid[2]);
        data.WriteBit(playerGuid[0]);
        data.WriteBit(playerGuid[5]);
    }

    data.WriteBit(guid[5]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[2]);

    data << uint8(joinData.result);                        // Check Result

    for (lfg::LfgLockPartyMap::const_iterator it = joinData.lockmap.begin(); it != joinData.lockmap.end(); ++it)
    {
        ObjectGuid playerGuid = it->first;

        data.WriteByteSeq(playerGuid[4]);

        for (lfg::LfgLockMap::const_iterator itr = it->second.begin(); itr != it->second.end(); ++itr)
        {
            data << uint32(itr->first);                        // Dungeon entry (id + type)
            data << uint32(itr->second.lockStatus);            // Lock status
            data << uint32(0);                                 // Required itemLevel
            data << uint32(0);                                 // Current itemLevel
        }

        data.WriteByteSeq(playerGuid[1]);
        data.WriteByteSeq(playerGuid[0]);
        data.WriteByteSeq(playerGuid[5]);
        data.WriteByteSeq(playerGuid[7]);
        data.WriteByteSeq(playerGuid[3]);
        data.WriteByteSeq(playerGuid[6]);
        data.WriteByteSeq(playerGuid[2]);
    }

    data << uint8(joinData.state);

    data.WriteByteSeq(guid[2]);

    data << uint32(getMSTime());                          // Time
    data << uint32(queueData ? queueData->ClientQueueId : 0);
    data << uint32(3);                                    // Unk

    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[3]);

    SendPacket(&data);
}

void WorldSession::SendLfgQueueStatus(lfg::LfgQueueStatusData const& queueData)
{
    TC_LOG_DEBUG("network", "SMSG_LFG_QUEUE_STATUS {} dungeon: {}, waitTime: {}, "
        "avgWaitTime: %d, waitTimeTanks: %d, waitTimeHealer: %d, waitTimeDps: %d, "
        "queuedTime: %u, tanks: %u, healers: %u, dps: %u",
        GetPlayerInfo().c_str(), queueData.dungeonId, queueData.waitTime, queueData.waitTimeAvg,
        queueData.waitTimeTank, queueData.waitTimeHealer, queueData.waitTimeDps,
        queueData.queuedTime, queueData.tanks, queueData.healers, queueData.dps);

    auto& playerQueueData = sLFGMgr->GetPlayerQueueData(GetPlayer()->GetGUID(), queueData.queueId);

    LFGDungeonEntry const* dungeon = sLFGDungeonStore.LookupEntry(queueData.dungeonId);

    ObjectGuid guid = playerQueueData.OriginalGroup ? playerQueueData.OriginalGroup : _player->GetGUID();
    WorldPacket data(SMSG_LFG_QUEUE_STATUS, 4 + 4 + 4 + 4 + 4 + 4 + 1 + 1 + 1 + 4 + 4 + 4 + 4 + 8);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[7]);

    data << uint32(3);                                     // Flags ?
    data.WriteByteSeq(guid[0]);
    data << uint32(queueData.queuedTime);                  // Player wait time in queue
    data.WriteByteSeq(guid[4]);
    data << int32(queueData.waitTimeAvg);                  // Average Wait time
    data << int32(queueData.waitTimeTank);                 // Wait Tanks
    data << uint8(queueData.tanks);                        // Tanks needed
    data << int32(queueData.waitTimeHealer);               // Wait Healers
    data << uint8(queueData.healers);                      // Healers needed
    data << int32(queueData.waitTimeDps);                  // Wait Dps
    data << uint8(queueData.dps);                          // Dps needed
    data << int32(queueData.joinTime);                     // Join time
    data << uint32(playerQueueData.ClientQueueId);
    data.WriteByteSeq(guid[1]);
    data << int32(queueData.waitTime);                     // Wait Time
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[2]);
    data << uint32(dungeon ? dungeon->Entry() : 0);        // Dungeon
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[6]);

    SendPacket(&data);
}

void WorldSession::SendLfgPlayerReward(lfg::LfgPlayerRewardData const& rewardData)
{
    if (!rewardData.rdungeonEntry || !rewardData.sdungeonEntry)
        return;

    TC_LOG_DEBUG("network", "SMSG_LFG_PLAYER_REWARD {} rdungeonEntry: {}, sdungeonEntry: {}, done: {}",
        GetPlayerInfo().c_str(), rewardData.rdungeonEntry, rewardData.sdungeonEntry, rewardData.done);

    uint8 itemNum = rewardData.quest ? (rewardData.quest->GetRewItemsCount() + rewardData.quest->GetRewCurrencyCount()) : 0;
    uint8 CTAitemNum = rewardData.ctaQuest ? rewardData.ctaQuest->GetRewItemsCount() : 0;

    ByteBuffer bytereward;
    WorldPacket data(SMSG_LFG_PLAYER_REWARD);
    data << uint32(rewardData.quest ? rewardData.quest->GetRewOrReqMoney(GetPlayer()) : 0);
    data << uint32(rewardData.rdungeonEntry);                               // Random Dungeon Finished
    data << uint32(rewardData.quest ? rewardData.quest->XPValue(GetPlayer()) : 0);
    data << uint32(rewardData.sdungeonEntry);                               // Dungeon Finished
    data.WriteBits(itemNum + CTAitemNum, 20);
    if (rewardData.quest && itemNum)
    {
        for (uint8 i = 0; i < QUEST_REWARD_CURRENCY_COUNT; ++i)
        {
            if (uint32 currencyId = rewardData.quest->RewardCurrencyId[i])
            {
                data.WriteBit(true);                                    // Is currency

                bytereward << uint32(currencyId);
                bytereward << uint32(0);                                // Currency Flags
                bytereward << uint32(0);
                bytereward << uint32(rewardData.quest->RewardCurrencyCount[i]);
            }
        }

        for (uint8 i = 0; i < QUEST_REWARD_ITEM_COUNT; ++i)
        {
            if (uint32 itemId = rewardData.quest->RewardItemId[i])
            {
                ItemTemplate const* item = sObjectMgr->GetItemTemplate(itemId);

                data.WriteBit(false);                                   // Is currency

                bytereward << uint32(itemId);
                bytereward << uint32(0);
                bytereward << uint32(item ? item->DisplayInfoID : 0);
                bytereward << uint32(rewardData.quest->RewardItemCount[i]);
            }
        }
    }
    if (rewardData.ctaQuest && CTAitemNum)
    {
        for (uint8 i = 0; i < QUEST_REWARD_ITEM_COUNT; ++i)
        {
            if (uint32 itemId = rewardData.ctaQuest->RewardItemId[i])
            {
                ItemTemplate const* item = sObjectMgr->GetItemTemplate(itemId);

                data.WriteBit(false);                                   // Is currency

                bytereward << uint32(itemId);
                bytereward << uint32(0);
                bytereward << uint32(item ? item->DisplayInfoID : 0);
                bytereward << uint32(rewardData.ctaQuest->RewardItemCount[i]);
            }
        }
    }

    data.FlushBits();

    data.append(bytereward);

    SendPacket(&data);

}

void WorldSession::SendLfgBootProposalUpdate(lfg::LfgPlayerBoot const& boot)
{
    ObjectGuid guid = GetPlayer()->GetGUID();
    uint32 queueId = sLFGMgr->GetActiveQueueId(guid);
    ObjectGuid gguid = sLFGMgr->GetGroup(guid, queueId);
    lfg::LfgAnswer playerVote = boot.votes.find(guid)->second;
    uint8 votesNum = 0;
    uint8 agreeNum = 0;
    uint32 secsleft = uint8((boot.cancelTime - time(NULL)) / 1000);
    for (lfg::LfgAnswerContainer::const_iterator it = boot.votes.begin(); it != boot.votes.end(); ++it)
    {
        if (it->second != lfg::LFG_ANSWER_PENDING)
        {
            ++votesNum;
            if (it->second == lfg::LFG_ANSWER_AGREE)
                ++agreeNum;
        }
    }
    TC_LOG_DEBUG("network", "SMSG_LFG_BOOT_PROPOSAL_UPDATE {} inProgress: {} - "
        "didVote: %u - agree: %u - victim: %u votes: %u - agrees: %u - left: %u - "
        "needed: %u - reason %s",
        GetPlayerInfo().c_str(), uint8(boot.inProgress), uint8(playerVote != lfg::LFG_ANSWER_PENDING),
        uint8(playerVote == lfg::LFG_ANSWER_AGREE), boot.victim.GetCounter(), votesNum, agreeNum,
        secsleft, lfg::LFG_GROUP_KICK_VOTES_NEEDED, boot.reason.c_str());
    
    ObjectGuid pguid = boot.victim;
    WorldPacket data(SMSG_LFG_BOOT_PROPOSAL_UPDATE);
    data.WriteBit(boot.reason.empty());
    data.WriteBit(pguid[3]);
    data.WriteBit(playerVote != lfg::LFG_ANSWER_PENDING);           // Did Vote
    data.WriteBit(agreeNum >= sLFGMgr->GetBootVotesNeeded(gguid));  // Did succeed
    data.WriteBit(playerVote == lfg::LFG_ANSWER_AGREE);             // Agree
    data.WriteBit(pguid[6]);
    if (!boot.reason.empty())
        data.WriteBits(boot.reason.size(), 8);
    data.WriteBit(boot.inProgress);                                 // Vote in progress
    data.WriteBit(pguid[1]);
    data.WriteBit(pguid[7]);
    data.WriteBit(pguid[5]);
    data.WriteBit(pguid[2]);
    data.WriteBit(pguid[0]);
    data.WriteBit(pguid[4]);

    data.FlushBits();
    
    data.WriteByteSeq(pguid[2]);
    data.WriteByteSeq(pguid[4]);
    data.WriteByteSeq(pguid[3]);
    data.WriteByteSeq(pguid[6]);
    data << uint32(sLFGMgr->GetBootVotesNeeded(gguid));             // Needed Votes
    data << uint32(secsleft);                                       // Time Left
    if (!boot.reason.empty())
        data.WriteString(boot.reason);
    data.WriteByteSeq(pguid[5]);
    data.WriteByteSeq(pguid[0]);
    data << uint32(agreeNum);                                       // Agree Count
    data.WriteByteSeq(pguid[7]);
    data << uint32(votesNum);                                       // Total Votes
    data.WriteByteSeq(pguid[1]);

    SendPacket(&data);
}

void WorldSession::SendLfgUpdateProposal(lfg::LfgProposal const& proposal)
{
    ObjectGuid guid = GetPlayer()->GetGUID();
    ObjectGuid gguid = proposal.players.find(guid)->second.group;
    bool silent = !proposal.isNew && gguid && gguid == proposal.group;
    uint32 dungeonEntry = proposal.dungeonId;
    uint32 serverQueueId = proposal.players.find(guid)->second.queueId;
    auto& queueData = sLFGMgr->GetPlayerQueueData(_player->GetGUID(), serverQueueId);

    TC_LOG_DEBUG("network", "SMSG_LFG_PROPOSAL_UPDATE {} state: {}",
        GetPlayerInfo().c_str(), proposal.state);

    // Send random dungeon for this player even if group is directly registered to proposal.dungeonId
    if (queueData.RandomDungeon)
        dungeonEntry = queueData.RandomDungeon;

    dungeonEntry = sLFGMgr->GetLFGDungeonEntry(dungeonEntry);

    ByteBuffer roleData;
    WorldPacket data(SMSG_LFG_PROPOSAL_UPDATE, 4 + 1 + 4 + 4 + 1 + 1 + proposal.players.size() * (4 + 1 + 1 + 1 + 1 + 1));

    ObjectGuid guid1 = queueData.OriginalGroup ? queueData.OriginalGroup : guid;
    ObjectGuid guid2 = ObjectGuid(uint64(dungeonEntry) | (uint64(0x1F45) << 48));

    data.WriteBit(guid2[6]);
    data.WriteBit(guid2[0]);
    data.WriteBit(guid1[1]);
    data.WriteBit(guid1[7]);
    data.WriteBit(guid1[5]);
    data.WriteBit(guid2[5]);
    data.WriteBit(guid1[4]);
    data.WriteBit(silent);
    data.WriteBit(guid2[2]);
    data.WriteBit(guid1[6]);
    data.WriteBit(guid2[3]);
    data.WriteBit(guid2[7]);
    data.WriteBit(guid1[3]);

    data.WriteBits(proposal.players.size(), 21);

    for (lfg::LfgProposalPlayerContainer::const_iterator it = proposal.players.begin(); it != proposal.players.end(); ++it)
    {
        lfg::LfgProposalPlayer const& player = it->second;

        data.WriteBit(player.group && !proposal.isNew && player.group == proposal.group);      // Is group in dungeon
        data.WriteBit(it->first == guid);
        data.WriteBit(player.accept != lfg::LFG_ANSWER_PENDING);
        data.WriteBit(player.accept == lfg::LFG_ANSWER_AGREE);
        data.WriteBit(player.group && player.group == gguid);               // Same group as the player

        uint8 role = it->second.role;
        if (proposal.leader != it->first)
            role &= ~lfg::PLAYER_ROLE_LEADER;

        roleData << uint32(role);
    }

    data.WriteBit(guid1[2]);
    data.WriteBit(guid2[4]);
    data.WriteBit(0);                                      // unk
    data.WriteBit(guid1[0]);
    data.WriteBit(guid2[1]);

    data.WriteByteSeq(guid2[1]);
    data.WriteByteSeq(guid1[4]);
    data.WriteByteSeq(guid2[4]);
    data.WriteByteSeq(guid1[7]);
    data.WriteByteSeq(guid1[2]);
    data.WriteByteSeq(guid1[0]);
    data << uint32(dungeonEntry);
    data << uint8(proposal.state);
    data << uint32(queueData.ClientQueueId);
    data.WriteByteSeq(guid2[6]);
    data << uint32(proposal.id);
    data.WriteByteSeq(guid1[5]);
    data.WriteByteSeq(guid1[3]);
    data << uint32(queueData.JoinTime);
    data.WriteByteSeq(guid2[5]);
    data.WriteByteSeq(guid1[6]);

    data.append(roleData);

    data << uint32(proposal.encounters);                   // Encounters done
    data.WriteByteSeq(guid2[7]);
    data.WriteByteSeq(guid1[1]);
    data.WriteByteSeq(guid2[0]);
    data.WriteByteSeq(guid2[2]);
    data << uint32(3);                                     // Always 3 flags?
    data.WriteByteSeq(guid2[3]);

    SendPacket(&data);
}

void WorldSession::SendLfgDisabled()
{
    TC_LOG_DEBUG("network", "SMSG_LFG_DISABLED {}", GetPlayerInfo().c_str());
    WorldPacket data(SMSG_LFG_DISABLED, 0);
    SendPacket(&data);
}

void WorldSession::SendLfgOfferContinue(uint32 dungeonEntry)
{
    TC_LOG_DEBUG("network", "SMSG_LFG_OFFER_CONTINUE {} dungeon entry: {}",
        GetPlayerInfo().c_str(), dungeonEntry);
    WorldPacket data(SMSG_LFG_OFFER_CONTINUE, 4);
    data << uint32(dungeonEntry);
    SendPacket(&data);
}

void WorldSession::SendLfgTeleportError(uint8 err)
{
    TC_LOG_DEBUG("network", "SMSG_LFG_TELEPORT_DENIED {} reason: {}",
        GetPlayerInfo().c_str(), err);
    WorldPacket data(SMSG_LFG_TELEPORT_DENIED, 4);
    data.WriteBits(err, 4);     // Error
    data.FlushBits();
    SendPacket(&data);
}

void WorldSession::HandleSetLfgBonusFactionId(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "CMSG_SET_LFG_BONUS_FACTION_ID {}", GetPlayerInfo().c_str());
    
    uint32 bonusFactionId = recvData.read<uint32>();
    GetPlayer()->SetUInt32Value(PLAYER_FIELD_LFG_BONUS_FACTION_ID, bonusFactionId);
}
