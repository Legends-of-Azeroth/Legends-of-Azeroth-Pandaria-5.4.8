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
#include "Opcodes.h"
#include "WorldPacket.h"
#include "PartyPackets.h"
#include "WorldSession.h"
#include "Player.h"
#include "World.h"
#include "ObjectMgr.h"
#include "GroupMgr.h"
#include "Group.h"
#include "Formulas.h"
#include "ObjectAccessor.h"
#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "MapManager.h"
#include "InstanceSaveMgr.h"
#include "MapInstanced.h"
#include "Util.h"
#include "LFGMgr.h"
#include "UpdateFieldFlags.h"
#include "GuildMgr.h"
#include "AchievementMgr.h"
#include "SocialMgr.h"
#include "Config.h"
#include <filesystem>

Roll::Roll(ObjectGuid lootGuid, LootItem const& li) : lootGUID(lootGuid), itemid(li.itemid),
itemRandomPropId(li.randomPropertyId), itemRandomSuffix(li.randomSuffix), itemCount(li.count),
totalPlayersRolling(0), totalNeed(0), totalGreed(0), totalPass(0), itemSlot(0),
rollVoteMask(ROLL_ALL_TYPE_NO_DISENCHANT) { }

Roll::~Roll() { }

void Roll::setLoot(Loot* pLoot)
{
    link(pLoot, this);
}

Loot* Roll::getLoot()
{
    return getTarget();
}

Group::Group() : m_leaderGuid(), m_leaderName(""), m_groupType(GROUPTYPE_NORMAL),
m_dungeonDifficulty(DUNGEON_DIFFICULTY_NORMAL), m_raidDifficulty(RAID_DIFFICULTY_10MAN_NORMAL),
m_bgGroup(NULL), m_bfGroup(NULL), m_lootMethod(FREE_FOR_ALL), m_lootThreshold(ITEM_QUALITY_UNCOMMON), m_looterGuid(),
m_subGroupsCounts(NULL), m_guid(), m_counter(0), m_maxEnchantingLevel(0), m_dbStoreId(0), m_slot(GroupSlot::Max)
{
    for (uint8 i = 0; i < TARGETICONCOUNT; ++i)
        m_targetIcons[i].Clear();
}

Group::~Group()
{
    if (m_bgGroup)
    {
        TC_LOG_DEBUG("bg.battleground", "Group::~Group: battleground group being deleted.");
        if (m_bgGroup->GetBgRaid(ALLIANCE) == this) m_bgGroup->SetBgRaid(ALLIANCE, NULL);
        else if (m_bgGroup->GetBgRaid(HORDE) == this) m_bgGroup->SetBgRaid(HORDE, NULL);
        else TC_LOG_ERROR("misc", "Group::~Group: battleground group is not linked to the correct battleground.");
    }
    Rolls::iterator itr;
    while (!RollId.empty())
    {
        itr = RollId.begin();
        Roll *r = *itr;
        RollId.erase(itr);
        delete (r);
    }

    // it is undefined whether objectmgr (which stores the groups) or instancesavemgr
    // will be unloaded first so we must be prepared for both cases
    // this may unload some instance saves
    for (uint8 i = 0; i < MAX_DIFFICULTY; ++i)
        for (BoundInstancesMap::iterator itr2 = m_boundInstances[i].begin(); itr2 != m_boundInstances[i].end(); ++itr2)
            itr2->second.save->RemoveGroup(this);

    // Sub group counters clean up
    delete [] m_subGroupsCounts;

}

bool Group::Create(Player* leader)
{
    ObjectGuid leaderGuid = leader->GetGUID();
    ObjectGuid::LowType lowguid = sGroupMgr->GenerateGroupId();

    m_guid = ObjectGuid(HighGuid::Group, lowguid);
    m_leaderGuid = leaderGuid;
    m_leaderName = leader->GetName();

    leader->SetFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_GROUP_LEADER);

    if (isBGGroup() || isBFGroup())
        m_groupType = GROUPTYPE_BGRAID;

    if (m_groupType & GROUPTYPE_RAID)
        _initRaidSubGroupsCounter();

    if (!isLFGGroup())
        m_lootMethod = GROUP_LOOT;

    m_slot = isBGGroup() || isBGGroup() || isLFGGroup() ? GroupSlot::Instance : GroupSlot::Original;

    m_lootThreshold = ITEM_QUALITY_UNCOMMON;
    m_looterGuid = leaderGuid;

    m_dungeonDifficulty = DUNGEON_DIFFICULTY_NORMAL;
    m_raidDifficulty = RAID_DIFFICULTY_10MAN_NORMAL;

    if (!isBGGroup() && !isBFGroup())
    {
        m_dungeonDifficulty = leader->GetDungeonDifficulty();
        m_raidDifficulty = leader->GetRaidDifficulty();

        m_dbStoreId = sGroupMgr->GenerateNewGroupDbStoreId();

        sGroupMgr->RegisterGroupDbStoreId(m_dbStoreId, this);

        // Store group in database
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GROUP);

        uint8 index = 0;

        stmt->setUInt32(index++, m_dbStoreId);
        stmt->setUInt32(index++, m_leaderGuid.GetCounter());
        stmt->setUInt8(index++, uint8(m_lootMethod));
        stmt->setUInt32(index++, m_looterGuid.GetCounter());
        stmt->setUInt8(index++, uint8(m_lootThreshold));
        stmt->setUInt32(index++, uint32(m_targetIcons[0]));
        stmt->setUInt32(index++, uint32(m_targetIcons[1]));
        stmt->setUInt32(index++, uint32(m_targetIcons[2]));
        stmt->setUInt32(index++, uint32(m_targetIcons[3]));
        stmt->setUInt32(index++, uint32(m_targetIcons[4]));
        stmt->setUInt32(index++, uint32(m_targetIcons[5]));
        stmt->setUInt32(index++, uint32(m_targetIcons[6]));
        stmt->setUInt32(index++, uint32(m_targetIcons[7]));
        stmt->setUInt8(index++, uint8(m_groupType));
        stmt->setUInt32(index++, uint8(m_dungeonDifficulty));
        stmt->setUInt32(index++, uint8(m_raidDifficulty));
        stmt->setUInt8 (index++, uint8(m_slot));

        CharacterDatabase.Execute(stmt);


        ASSERT(AddMember(leader)); // If the leader can't be added to a new group because it appears full, something is clearly wrong.

        if (!isLFGGroup())
            Player::ConvertInstancesToGroup(leader, this, false);
    }
    else if (!AddMember(leader))
        return false;

    sGroupMgr->AddGroup(this);

    return true;
}

void Group::LoadGroupFromDB(Field* fields)
{
    m_dbStoreId = fields[15].GetUInt32();
    m_guid = ObjectGuid(HighGuid::Group, sGroupMgr->GenerateGroupId());
    m_leaderGuid = ObjectGuid(HighGuid::Player, fields[0].GetUInt32());

    // group leader not exist
    if (!sObjectMgr->GetPlayerNameByGUID(m_leaderGuid, m_leaderName))
        return;

    m_lootMethod = LootMethod(fields[1].GetUInt8());
    m_looterGuid = ObjectGuid(HighGuid::Player, fields[2].GetUInt32());
    m_lootThreshold = ItemQualities(fields[3].GetUInt8());

    for (uint8 i = 0; i < TARGETICONCOUNT; ++i)
        m_targetIcons[i].Set(fields[4+i].GetUInt64());

    m_groupType  = GroupType(fields[12].GetUInt8());
    if (m_groupType & GROUPTYPE_RAID)
        _initRaidSubGroupsCounter();

    uint32 diff = fields[13].GetUInt8();
    if (diff >= MAX_DUNGEON_DIFFICULTY)
        m_dungeonDifficulty = DUNGEON_DIFFICULTY_NORMAL;
    else
        m_dungeonDifficulty = Difficulty(diff);

    uint32 r_diff = fields[14].GetUInt8();
    if (r_diff >= MAX_RAID_DIFFICULTY)
       m_raidDifficulty = RAID_DIFFICULTY_10MAN_NORMAL;
    else
       m_raidDifficulty = Difficulty(r_diff);

    if (m_groupType & GROUPTYPE_LFG)
        sLFGMgr->LoadFromDB(fields, GetGUID());

    m_logResumeOnLogin = isRaidGroup() && !isBGGroup();

    m_slot = GroupSlot(fields[18].GetUInt8());
    if (m_slot >= GroupSlot::Max)
    {
        TC_LOG_ERROR("shitlog", "Group::LoadGroupFromDB invalid group slot (%u) for group %u", fields[13].GetUInt8(), m_dbStoreId);
        m_slot = GroupSlot::Original;
    }
    if (m_slot == GroupSlot::Original && isLFGGroup())
        TC_LOG_ERROR("shitlog", "Group::LoadGroupFromDB group is original group but it is lfg group %u", m_dbStoreId);
    if (m_slot == GroupSlot::Instance && !isLFGGroup()) // Only lfg groups are stored to db
        TC_LOG_ERROR("shitlog", "Group::LoadGroupFromDB group is instance group but it is not lfg group %u", m_dbStoreId);
}

void Group::LoadMemberFromDB(ObjectGuid::LowType guidLow, uint8 memberFlags, uint8 subgroup, uint8 roles)
{
    MemberSlot member;
    member.guid = ObjectGuid(HighGuid::Player, guidLow);

    // skip non-existed member
    if (!sObjectMgr->GetPlayerNameByGUID(member.guid, member.name))
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GROUP_MEMBER);
        stmt->setUInt32(0, m_dbStoreId);
        stmt->setUInt32(1, guidLow);
        CharacterDatabase.Execute(stmt);
        return;
    }

    member.group = subgroup;
    member.flags = memberFlags;
    member.roles = roles;

    m_memberSlots.push_back(member);

    SubGroupCounterIncrease(subgroup);

    if (sLFGMgr->GetActiveState(GetGUID()) != lfg::LFG_STATE_NONE)
        sLFGMgr->SetupGroupMember(member.guid, GetGUID());
    sGroupMgr->BindGroupToPlayer(member.guid, this);
}

void Group::SaveRolesToDB()
{
    for (member_witerator itr = m_memberSlots.begin(); itr != m_memberSlots.end(); ++itr)
        CharacterDatabase.PExecute("UPDATE group_member SET roles='%u' WHERE guid='%u' AND memberGuid='%u'", itr->roles, m_dbStoreId, itr->guid.GetCounter());
}

void Group::ChangeFlagEveryoneAssistant(bool apply)
{
    if (apply)
        m_groupType = GroupType(m_groupType | GROUPTYPE_EVERYONE_IS_ASSISTANT);
    else
        m_groupType = GroupType(m_groupType &~GROUPTYPE_EVERYONE_IS_ASSISTANT);

    this->SendUpdate();
}

void Group::ConvertToLFG(bool flex)
{
    m_groupType = GroupType(m_groupType | GROUPTYPE_LFG | GROUPTYPE_UNK1);
    m_lootMethod = NEED_BEFORE_GREED;
    m_flex = flex;
    if (!isBGGroup() && !isBFGroup())
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GROUP_TYPE);

        stmt->setUInt8(0, uint8(m_groupType));
        stmt->setUInt32(1, m_dbStoreId);

        CharacterDatabase.Execute(stmt);
    }

    SendUpdate();
}

void Group::ConvertToRaid()
{
    m_groupType = GroupType(m_groupType | GROUPTYPE_RAID);

    _initRaidSubGroupsCounter();

    if (!isBGGroup() && !isBFGroup())
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GROUP_TYPE);

        stmt->setUInt8(0, uint8(m_groupType));
        stmt->setUInt32(1, m_dbStoreId);

        CharacterDatabase.Execute(stmt);
    }

    SendUpdate();

    // update quest related GO states (quest activity dependent from raid membership)
    for (member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
        if (Player* player = ObjectAccessor::FindPlayer(citr->guid))
            player->UpdateForQuestWorldObjects();

}

void Group::ConvertToGroup()
{
    if (m_memberSlots.size() > 5)
        return; // What message error should we send?

    m_groupType = GroupType(GROUPTYPE_NORMAL);

    if (m_subGroupsCounts)
    {
        delete [] m_subGroupsCounts;
        m_subGroupsCounts = NULL;
    }

    if (!isBGGroup() && !isBFGroup())
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GROUP_TYPE);

        stmt->setUInt8(0, uint8(m_groupType));
        stmt->setUInt32(1, m_dbStoreId);

        CharacterDatabase.Execute(stmt);
    }

    SendUpdate();

    // update quest related GO states (quest activity dependent from raid membership)
    for (member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
        if (Player* player = ObjectAccessor::FindPlayer(citr->guid))
            player->UpdateForQuestWorldObjects();
}

bool Group::AddInvite(Player* player)
{
    if (!player || player->GetGroupInvite())
        return false;
    Group* group = player->GetGroup();
    if (group && (group->isBGGroup() || group->isBFGroup()))
        group = player->GetOriginalGroup();
    if (group)
        return false;

    RemoveInvite(player);

    m_invitees.insert(player);

    player->SetGroupInvite(this);

    sScriptMgr->OnGroupInviteMember(this, player->GetGUID());

    return true;
}

bool Group::AddLeaderInvite(Player* player)
{
    if (!AddInvite(player))
        return false;

    m_leaderGuid = player->GetGUID();
    m_leaderName = player->GetName();
    player->SetFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_GROUP_LEADER);
    return true;
}

void Group::RemoveInvite(Player* player)
{
    if (player)
    {
        m_invitees.erase(player);
        player->SetGroupInvite(NULL);
    }
}

void Group::RemoveAllInvites()
{
    for (InvitesList::iterator itr=m_invitees.begin(); itr != m_invitees.end(); ++itr)
        if (*itr)
            (*itr)->SetGroupInvite(NULL);

    m_invitees.clear();
}

Player* Group::GetInvited(uint64 guid) const
{
    for (InvitesList::const_iterator itr = m_invitees.begin(); itr != m_invitees.end(); ++itr)
    {
        if ((*itr) && (*itr)->GetGUID() == guid)
            return (*itr);
    }
    return NULL;
}

Player* Group::GetInvited(const std::string& name) const
{
    for (InvitesList::const_iterator itr = m_invitees.begin(); itr != m_invitees.end(); ++itr)
    {
        if ((*itr) && (*itr)->GetName() == name)
            return (*itr);
    }
    return NULL;
}

bool Group::AddMember(Player* player)
{
    // Get first not-full group
    uint8 subGroup = 0;
    if (m_subGroupsCounts)
    {
        bool groupFound = false;
        for (; subGroup < MAX_RAID_SUBGROUPS; ++subGroup)
        {
            if (m_subGroupsCounts[subGroup] < MAXGROUPSIZE)
            {
                groupFound = true;
                break;
            }
        }
        // We are raid group and no one slot is free
        if (!groupFound)
            return false;
    }

    MemberSlot member;
    member.guid      = player->GetGUID();
    member.name      = player->GetName();
    member.group     = subGroup;
    member.flags     = 0;
    member.roles     = 0;
    m_memberSlots.push_back(member);

    SubGroupCounterIncrease(subGroup);

    player->SetGroupInvite(nullptr);

    if (player->GetGroup(m_slot))
    {
        TC_LOG_ERROR("shitlog", "Group::AddMember player %s already in group of this type (%u)", player->GetSession()->GetPlayerInfo().c_str(), uint32(m_slot));
        return false;
    }

    player->SetGroup(m_slot, this, subGroup);

    // if the same group invites the player back, cancel the homebind timer
    InstanceGroupBind* bind = GetBoundInstance(player);
    if (bind && bind->save->GetInstanceId() == player->GetInstanceId())
        player->m_InstanceValid = true;

    //if (!isRaidGroup())                                      // reset targetIcons for non-raid-groups
    //{
    //    for (uint8 i = 0; i < TARGETICONCOUNT; ++i)
    //        m_targetIcons[i].Clear();
    //}

    // insert into the table if we're not a battleground group
    if (!isBGGroup() && !isBFGroup())
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GROUP_MEMBER);

        stmt->setUInt32(0, m_dbStoreId);
        stmt->setUInt32(1, member.guid.GetCounter());
        stmt->setUInt8(2, member.flags);
        stmt->setUInt8(3, member.group);
        stmt->setUInt8(4, member.roles);

        CharacterDatabase.Execute(stmt);

    }

    SendUpdate();
    sScriptMgr->OnGroupAddMember(this, player->GetGUID());

    if (!IsLeader(player->GetGUID()) && !isBGGroup() && !isBFGroup())
    {
        // reset the new member's instances, unless he is currently in one of them
        // including raid/heroic instances that they are not permanently bound to!
        player->ResetInstances(INSTANCE_RESET_GROUP_JOIN, false);
        player->ResetInstances(INSTANCE_RESET_GROUP_JOIN, true);

        if (player->GetLevel() >= LEVELREQUIREMENT_HEROIC)
        {
            if (player->GetDungeonDifficulty() != GetDungeonDifficulty())
            {
                player->SetDungeonDifficulty(GetDungeonDifficulty());
                player->SendDungeonDifficulty(GetDungeonDifficulty());
            }
            if (player->GetRaidDifficulty() != GetRaidDifficulty())
            {
                player->SetRaidDifficulty(GetRaidDifficulty());
                player->SendRaidDifficulty(GetRaidDifficulty());
            }
        }
    }
    player->SetGroupUpdateFlag(GROUP_UPDATE_FULL);
    UpdatePlayerOutOfRange(player);

    // quest related GO state dependent from raid membership
    if (isRaidGroup())
        player->UpdateForQuestWorldObjects();

    {
        // Broadcast new player group member fields to rest of the group
        player->SetFieldNotifyFlag(UF_FLAG_PARTY_MEMBER);

        UpdateData groupData(player->GetMapId());
        WorldPacket groupDataPacket;

        // Broadcast group members' fields to player
        for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
        {
            if (itr->GetSource() == player)
                continue;

            if (Player* member = itr->GetSource())
            {
                if (player->HaveAtClient(member))
                {
                    member->SetFieldNotifyFlag(UF_FLAG_PARTY_MEMBER);
                    member->BuildValuesUpdateBlockForPlayer(&groupData, player);
                    member->RemoveFieldNotifyFlag(UF_FLAG_PARTY_MEMBER);
                }

                if (member->HaveAtClient(player))
                {
                    UpdateData newData(player->GetMapId());
                    WorldPacket newDataPacket;
                    player->BuildValuesUpdateBlockForPlayer(&newData, member);
                    if (newData.HasData())
                    {
                        newData.BuildPacket(&newDataPacket);
                        member->SendDirectMessage(&newDataPacket);
                    }
                }
            }
        }

        if (groupData.HasData())
        {
            groupData.BuildPacket(&groupDataPacket);
            player->SendDirectMessage(&groupDataPacket);
        }

        player->RemoveFieldNotifyFlag(UF_FLAG_PARTY_MEMBER);
    }

    if (m_maxEnchantingLevel < player->GetSkillValue(SKILL_ENCHANTING))
        m_maxEnchantingLevel = player->GetSkillValue(SKILL_ENCHANTING);

    sGroupMgr->BindGroupToPlayer(player->GetGUID(), this);

    for (auto&& player : *this)
        sBattlegroundMgr->RemovePlayerFromArenaQeueus(player);

    return true;
}

bool Group::RemoveMember(ObjectGuid guid, const RemoveMethod& method /*= GROUP_REMOVEMETHOD_DEFAULT*/, ObjectGuid kicker /*= 0*/, const char* reason /*= NULL*/)
{
    BroadcastGroupUpdate();

    sScriptMgr->OnGroupRemoveMember(this, guid, method, kicker, reason);

    // LFG group vote kick handled in scripts
    if (isLFGGroup() && method == GROUP_REMOVEMETHOD_KICK && !IsFlex())
        return m_memberSlots.size();

    for (auto&& player : *this)
        sBattlegroundMgr->RemovePlayerFromArenaQeueus(player);

    // remove member and change leader (if need) only if strong more 2 members _before_ member remove (BG/BF allow 1 member group)
    if (GetMembersCount() > ((isBGGroup() || isLFGGroup() || isBFGroup()) ? 1u : 2u))
    {
        Player* player = ObjectAccessor::FindConnectedPlayer(guid);
        if (player)
        {
            // Don't homebind if player was removed from the original group but still has instance group
            if (player->GetGroup() == this)
                HomebindIfInstance(player);

            player->SetGroup(m_slot, nullptr);

            // quest related GO state dependent from raid membership
            player->UpdateForQuestWorldObjects();

            WorldPacket data;

            if (method == GROUP_REMOVEMETHOD_KICK || method == GROUP_REMOVEMETHOD_KICK_LFG)
            {
                data.Initialize(SMSG_GROUP_UNINVITE, 0);
                player->GetSession()->SendPacket(&data);
            }

            SendGroupRemoved(guid);

            player->RemoveFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_GROUP_LEADER);
        }

        // Remove player from group in DB
        if (!isBGGroup() && !isBFGroup())
        {
            CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GROUP_MEMBER);
            stmt->setUInt32(0, m_dbStoreId);
            stmt->setUInt32(1, guid.GetCounter());
            CharacterDatabase.Execute(stmt);
            DelinkMember(guid);
        }

        // Reevaluate group enchanter if the leaving player had enchanting skill or the player is offline
        if ((player && player->GetSkillValue(SKILL_ENCHANTING)) || !player)
            ResetMaxEnchantingLevel();

        // Remove player from loot rolls
        for (Rolls::iterator it = RollId.begin(); it != RollId.end(); ++it)
        {
            Roll* roll = *it;
            Roll::PlayerVote::iterator itr2 = roll->playerVote.find(guid);
            if (itr2 == roll->playerVote.end())
                continue;

            if (itr2->second == GREED || itr2->second == DISENCHANT)
                --roll->totalGreed;
            else if (itr2->second == NEED)
                --roll->totalNeed;
            else if (itr2->second == PASS)
                --roll->totalPass;

            if (itr2->second != NOT_VALID)
                --roll->totalPlayersRolling;

            roll->playerVote.erase(itr2);

            CountRollVote(roll->lootGUID, guid, roll->itemSlot, MAX_ROLL_TYPE);
        }

        // Update subgroups
        member_witerator slot = _getMemberWSlot(guid);
        if (slot != m_memberSlots.end())
        {
            SubGroupCounterDecrease(slot->group);
            m_memberSlots.erase(slot);
        }

        sGroupMgr->UnbindGroupFromPlayer(guid, this);

        // Pick new leader if necessary
        if (m_leaderGuid == guid)
            FindNewLeader();

        SendUpdate();

        if (isLFGGroup() && GetMembersCount() == 1)
        {
            Player* leader = ObjectAccessor::FindPlayer(GetLeaderGUID());
            uint32 mapId = sLFGMgr->GetDungeonMapId(GetGUID());
            if (!mapId || !leader || (leader->IsAlive() && leader->GetMapId() != mapId))
            {
                Disband();
                return false;
            }
        }

        if (guid == ReadyCheckInitiator())
        {
            if (player)
                player->ReadyCheckComplete();
            ReadyCheck(ObjectGuid::Empty);
        }

        if (m_memberSlots.size() < ((isLFGGroup() || isBGGroup()) ? 1u : 2u))
            Disband();

        return true;
    }
    // If group size before player removal <= 2 then disband it
    else
    {
        Disband();
        return false;
    }
}

void Group::ChangeLeader(ObjectGuid newLeaderGuid)
{
    member_witerator slot = _getMemberWSlot(newLeaderGuid);

    if (slot == m_memberSlots.end())
        return;

    Player* newLeader = ObjectAccessor::FindPlayer(slot->guid);

    // Don't allow switching leader to offline players
    if (!newLeader)
        return;

    sScriptMgr->OnGroupChangeLeader(this, newLeaderGuid, m_leaderGuid);

    if (!isBGGroup() && !isBFGroup())
    {
        CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

        // Remove the groups permanent instance bindings
        for (uint8 i = 0; i < MAX_DIFFICULTY; ++i)
        {
            for (BoundInstancesMap::iterator itr = m_boundInstances[i].begin(); itr != m_boundInstances[i].end();)
            {
                // Do not unbind saves of instances that already had map created (a newLeader entered)
                // forcing a new instance with another leader requires group disbanding (confirmed on retail)
                if (itr->second.perm && !sMapMgr->FindMap(itr->first, itr->second.save->GetInstanceId()))
                {
                    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GROUP_INSTANCE_PERM_BINDING);
                    stmt->setUInt32(0, m_dbStoreId);
                    stmt->setUInt32(1, itr->second.save->GetInstanceId());
                    trans->Append(stmt);

                    itr->second.save->RemoveGroup(this);
                    m_boundInstances[i].erase(itr++);
                }
                else
                    ++itr;
            }
        }

        // Copy the permanent binds from the new leader to the group
        if (!isLFGGroup())
            Player::ConvertInstancesToGroup(newLeader, this, true);

        // Update the group leader
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GROUP_LEADER);

        stmt->setUInt32(0, newLeader->GetGUID().GetCounter());
        stmt->setUInt32(1, m_dbStoreId);

        trans->Append(stmt);

        CharacterDatabase.CommitTransaction(trans);
    }

    uint64 oldLeaderGuid = m_leaderGuid;

    if (Player* oldLeader = ObjectAccessor::FindPlayer(m_leaderGuid))
        oldLeader->RemoveFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_GROUP_LEADER);

    m_leaderGuid = newLeader->GetGUID();
    m_leaderName = newLeader->GetName();
    ToggleGroupMemberFlag(slot, MEMBER_FLAG_ASSISTANT, false);

    newLeader->SetFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_GROUP_LEADER);

    uint8 leaderNameLen = m_leaderName.size();

    WorldPacket data(SMSG_GROUP_SET_LEADER, 1 + 1 + leaderNameLen);
    data << uint8(0);

    data.WriteBits(leaderNameLen, 6);
    data.FlushBits();

    data.WriteString(m_leaderName);

    BroadcastPacket(&data, true);

}

void Group::Disband(bool hideDestroy /* = false */)
{
    sScriptMgr->OnGroupDisband(this);

    Player* player;
    for (member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        sGroupMgr->UnbindGroupFromPlayer(citr->guid, this);

        player = ObjectAccessor::FindConnectedPlayer(citr->guid);
        if (!player)
            continue;

        if (GetGroupSlot() == GroupSlot::Instance || (GetGroupSlot() == GroupSlot::Original && player->GetGroup(GroupSlot::Instance)))
            SendGroupRemoved(player->GetGUID());

        if (player->GetGroup() == this)
            HomebindIfInstance(player);

        player->SetGroup(m_slot, nullptr);

        // quest related GO state dependent from raid membership
        if (isRaidGroup())
            player->UpdateForQuestWorldObjects();

        WorldPacket data;
        if (!hideDestroy)
        {
            data.Initialize(SMSG_GROUP_DESTROYED, 0);
            player->GetSession()->SendPacket(&data);
        }

        //we already removed player from group and in player->GetGroup() is his original group, send update
        if (Group* group = player->GetGroup())
            group->SendUpdate();
        else
            SendGroupRemoved(player->GetGUID());
    }
    RollId.clear();
    m_memberSlots.clear();

    RemoveAllInvites();

    if (!isBGGroup() && !isBFGroup())
    {
        CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GROUP);
        stmt->setUInt32(0, m_dbStoreId);
        trans->Append(stmt);

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GROUP_MEMBER_ALL);
        stmt->setUInt32(0, m_dbStoreId);
        trans->Append(stmt);

        CharacterDatabase.CommitTransaction(trans);

        ResetInstances(INSTANCE_RESET_GROUP_DISBAND, false, NULL);
        ResetInstances(INSTANCE_RESET_GROUP_DISBAND, true, NULL);

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_LFG_DATA);
        stmt->setUInt32(0, m_dbStoreId);
        CharacterDatabase.Execute(stmt);

        sGroupMgr->FreeGroupDbStoreId(this);
    }

    sGroupMgr->RemoveGroup(this);
    delete this;
}

/*********************************************************/
/***                   LOOT SYSTEM                     ***/
/*********************************************************/

static void BuildStartLootRollPacket(WorldPacket& data, uint32 countDown, uint32 mapid, Roll const& roll, bool canNeed)
{
    ObjectGuid lootedGuid = roll.lootGUID;

    uint8 unk56 = 0;

    data.WriteBits(7, 3);                       // 7 from parser
    data.WriteBit(roll.itemSlot == 0);
    data.WriteBit(0);
    data.WriteBit(lootedGuid[3]);
    data.WriteBit(lootedGuid[1]);
    data.WriteBit(lootedGuid[7]);
    data.WriteBit(lootedGuid[6]);
    data.WriteBit(lootedGuid[2]);
    data.WriteBit(lootedGuid[4]);
    data.WriteBit(lootedGuid[5]);
    data.WriteBit(lootedGuid[0]);
    data.WriteBit(!unk56);
    data.WriteBits(3, 2);                       // 3 from parser. With 0 shows {null} instead item link in chat log (and does not show roll control)
    data.WriteByteSeq(lootedGuid[7]);
    data << uint32(roll.itemRandomPropId);
    data.WriteByteSeq(lootedGuid[5]);
    data << uint32(mapid);
    data << uint32(roll.itemRandomSuffix);
    if (roll.itemSlot)
        data << uint8(roll.itemSlot);
    data.WriteByteSeq(lootedGuid[4]);
    data.WriteByteSeq(lootedGuid[0]);
    data.WriteByteSeq(lootedGuid[3]);
    data.WriteByteSeq(lootedGuid[2]);
    data << uint32(0);                          // unk counter
    data << uint32(roll.itemid);
    uint8 voteMask = roll.rollVoteMask;
    if (!canNeed)
        voteMask &= ~ROLL_FLAG_TYPE_NEED;
    data << uint8(voteMask);
    data << uint32(roll.itemCount);
    data << uint8(0);                           // I dunno. 3 or 4 from parser. if 2, then shows loot window to some guy in party, even if it is not enabled.
    data << uint32(countDown);
    if (unk56)
        data << uint8(0);
    data.WriteByteSeq(lootedGuid[6]);
    if (auto proto = sObjectMgr->GetItemTemplate(roll.itemid))
        data << uint32(proto->DisplayInfoID);
    else
        data << uint32(0);
    data.WriteByteSeq(lootedGuid[1]);
}

void Group::SendLootStartRoll(uint32 countDown, uint32 mapid, Roll const& roll)
{
    WorldPacket data(SMSG_LOOT_START_ROLL);

    BuildStartLootRollPacket(data, countDown, mapid, roll, true);

    for (auto&& it : roll.playerVote)
        if (Player* player = ObjectAccessor::FindPlayer(it.first))
            if (it.second == NOT_EMITED_YET)
                player->GetSession()->SendPacket(&data);
}

void Group::SendLootStartRollToPlayer(uint32 countDown, uint32 mapId, Player* player, bool canNeed, Roll const& roll)
{
    if (!player)
        return;

    WorldPacket data(SMSG_LOOT_START_ROLL);
    BuildStartLootRollPacket(data, countDown, mapId, roll, canNeed);
    player->GetSession()->SendPacket(&data);
}

void Group::SendLootRoll(ObjectGuid targetGuid, int32 rollNumber, uint8 rollType, Roll const& roll)
{
    ObjectGuid lootedGuid = roll.lootGUID;

    uint8 byte0x30 = 0;

    WorldPacket data(SMSG_LOOT_ROLL, (8+4+8+4+4+4+1+1+1));
    data.WriteBit(lootedGuid[6]);
    data.WriteBit(targetGuid[6]);
    data.WriteBit(lootedGuid[5]);
    data.WriteBit(lootedGuid[7]);
    data.WriteBit(byte0x30 == 0);       // 0x30
    data.WriteBit(targetGuid[3]);
    data.WriteBit(targetGuid[7]);
    data.WriteBit(lootedGuid[4]);
    data.WriteBit(roll.itemSlot == 0);
    data.WriteBit(lootedGuid[0]);
    data.WriteBit(lootedGuid[2]);
    data.WriteBit(targetGuid[1]);
    data.WriteBit(targetGuid[0]);
    data.WriteBit(0);                   // 0x3C
    data.WriteBits(3, 2);               // 0x34
    data.WriteBit(lootedGuid[3]);
    data.WriteBit(0);                   // 0x64 
    data.WriteBit(targetGuid[5]);
    data.WriteBits(3, 3);               // 0x38
    data.WriteBit(targetGuid[4]);
    data.WriteBit(targetGuid[2]);
    data.WriteBit(lootedGuid[1]);
    data.WriteByteSeq(lootedGuid[7]);
    data.WriteByteSeq(targetGuid[6]);
    data.WriteByteSeq(lootedGuid[0]);
    data.WriteByteSeq(lootedGuid[5]);
    data.WriteByteSeq(lootedGuid[3]);
    if (roll.itemSlot)
        data << uint8(roll.itemSlot);
    if (auto proto = sObjectMgr->GetItemTemplate(roll.itemid))
        data << uint32(proto->DisplayInfoID);
    else
        data << uint32(0);
    data << uint32(roll.itemRandomPropId);
    data << uint32(roll.itemRandomSuffix);
    data << uint32(0);                  // unk counter (0)
    data.WriteByteSeq(lootedGuid[1]);
    data.WriteByteSeq(lootedGuid[4]);
    data.WriteByteSeq(targetGuid[2]);
    data.WriteByteSeq(lootedGuid[2]);
    data << uint32(roll.itemid);
    data.WriteByteSeq(targetGuid[4]);
    data << uint32(rollNumber);
    if (byte0x30)
        data << byte0x30;
    data.WriteByteSeq(lootedGuid[6]);
    data.WriteByteSeq(targetGuid[1]);
    data.WriteByteSeq(targetGuid[0]);
    data << uint8(rollType);
    data.WriteByteSeq(targetGuid[7]);
    data << uint32(roll.itemCount);
    data.WriteByteSeq(targetGuid[5]);
    data.WriteByteSeq(targetGuid[3]);

    for (auto&& it : roll.playerVote)
        if (Player* player = ObjectAccessor::FindPlayer(it.first))
            if (it.second != NOT_VALID)
                player->GetSession()->SendPacket(&data);
}

void Group::SendLootRollWon(ObjectGuid targetGuid, int32 rollNumber, uint8 rollType, Roll const& roll)
{
    WorldPacket data(SMSG_LOOT_ROLL_WON, (8+4+4+4+4+8+1+1));
    ObjectGuid lootedGuid = roll.lootGUID;

    uint8 byte0x2C = 0, byte0x2D = 0, byte0x38 = 0;

    data.WriteBit(targetGuid[0]);
    data.WriteBit(lootedGuid[3]);
    data.WriteBits(0, 2);               // 0x30
    data.WriteBit(targetGuid[3]);
    data.WriteBit(targetGuid[2]);
    data.WriteBit(targetGuid[6]);
    data.WriteBit(targetGuid[7]);
    data.WriteBit(byte0x2C == 0);
    data.WriteBit(lootedGuid[7]);
    data.WriteBit(lootedGuid[1]);
    data.WriteBit(targetGuid[4]);
    data.WriteBit(targetGuid[1]);
    data.WriteBit(lootedGuid[4]);
    data.WriteBit(targetGuid[5]);
    data.WriteBits(0, 3);               // 0x34
    data.WriteBit(lootedGuid[0]);
    data.WriteBit(lootedGuid[2]);
    data.WriteBit(roll.itemSlot == 0);
    data.WriteBit(byte0x38 != 0);
    data.WriteBit(lootedGuid[6]);
    data.WriteBit(lootedGuid[5]);
    data << uint32(0);                  // unk counter (0)
    if (auto proto = sObjectMgr->GetItemTemplate(roll.itemid))
        data << uint32(proto->DisplayInfoID);
    else
        data << uint32(0);
    data.WriteByteSeq(targetGuid[2]);
    data.WriteByteSeq(lootedGuid[7]);
    data << uint32(roll.itemRandomPropId);
    data.WriteByteSeq(targetGuid[5]);
    data.WriteByteSeq(lootedGuid[3]);
    data.WriteByteSeq(targetGuid[7]);
    data.WriteByteSeq(lootedGuid[1]);
    data.WriteByteSeq(lootedGuid[2]);
    data.WriteByteSeq(lootedGuid[0]);
    data.WriteByteSeq(targetGuid[3]);
    data << int32(rollNumber);
    data << uint32(roll.itemRandomSuffix);
    data.WriteByteSeq(lootedGuid[6]);
    data.WriteByteSeq(targetGuid[1]);
    data.WriteByteSeq(targetGuid[4]);
    data << uint8(rollType);
    data.WriteByteSeq(targetGuid[6]);
    if (roll.itemSlot)
        data << uint8(roll.itemSlot);
    data.WriteByteSeq(lootedGuid[4]);
    data << uint32(roll.itemid);
    data.WriteByteSeq(lootedGuid[5]);
    data.WriteByteSeq(targetGuid[0]);
    data << uint32(roll.itemCount);
    if (byte0x2C)
        data << uint8(byte0x2C);

    WorldPacket data2(SMSG_LOOT_ROLLS_COMPLETE);
    data2.WriteBit(lootedGuid[6]);
    data2.WriteBit(lootedGuid[5]);
    data2.WriteBit(lootedGuid[2]);
    data2.WriteBit(lootedGuid[3]);
    data2.WriteBit(lootedGuid[7]);
    data2.WriteBit(lootedGuid[0]);
    data2.WriteBit(lootedGuid[1]);
    data2.WriteBit(lootedGuid[4]);
    data2.WriteByteSeq(lootedGuid[1]);
    data2.WriteByteSeq(lootedGuid[0]);
    data2.WriteByteSeq(lootedGuid[2]);
    data2 << uint8(roll.itemSlot);
    data2.WriteByteSeq(lootedGuid[7]);
    data2.WriteByteSeq(lootedGuid[4]);
    data2.WriteByteSeq(lootedGuid[6]);
    data2.WriteByteSeq(lootedGuid[3]);
    data2.WriteByteSeq(lootedGuid[5]);

    for (auto&& it : roll.playerVote)
    {
        if (Player* player = ObjectAccessor::FindPlayer(it.first))
        {
            if (it.second != NOT_VALID)
            {
                player->GetSession()->SendPacket(&data);
                player->GetSession()->SendPacket(&data2);
            }
        }
    }
}

void Group::SendLootAllPassed(Roll const& roll)
{
    ObjectGuid lootedGuid = roll.lootGUID;

    uint8 byte2C = 0;

    WorldPacket data(SMSG_LOOT_ALL_PASSED, (8+4+4+4+4));
    data.WriteBit(roll.itemSlot == 0); // or may be not
    data.WriteBit(byte2C == 0);
    data.WriteBit(lootedGuid[2]);
    data.WriteBit(lootedGuid[4]);
    data.WriteBit(lootedGuid[3]);
    data.WriteBit(lootedGuid[1]);
    data.WriteBit(0);
    data.WriteBits(3, 2);
    data.WriteBits(3, 3);
    data.WriteBit(lootedGuid[6]);
    data.WriteBit(lootedGuid[5]);
    data.WriteBit(lootedGuid[7]);
    data.WriteBit(lootedGuid[0]);
    data.WriteByteSeq(lootedGuid[6]);
    data.WriteByteSeq(lootedGuid[5]);
    data.WriteByteSeq(lootedGuid[0]);
    data << uint32(0);
    data.WriteByteSeq(lootedGuid[3]);
    data.WriteByteSeq(lootedGuid[2]);
    if (byte2C)
        data << uint8(byte2C);
    if (auto proto = sObjectMgr->GetItemTemplate(roll.itemid))
        data << uint32(proto->DisplayInfoID);
    else
        data << uint32(0);
    data.WriteByteSeq(lootedGuid[1]);
    data << uint32(0);
    data << uint32(0);
    data.WriteByteSeq(lootedGuid[4]);
    data << uint32(0);
    data << uint32(roll.itemid);
    if (roll.itemSlot)
        data << uint8(roll.itemSlot);
    data.WriteByteSeq(lootedGuid[7]);

    WorldPacket data2(SMSG_LOOT_ROLLS_COMPLETE);
    data2.WriteBit(lootedGuid[6]);
    data2.WriteBit(lootedGuid[5]);
    data2.WriteBit(lootedGuid[2]);
    data2.WriteBit(lootedGuid[3]);
    data2.WriteBit(lootedGuid[7]);
    data2.WriteBit(lootedGuid[0]);
    data2.WriteBit(lootedGuid[1]);
    data2.WriteBit(lootedGuid[4]);
    data2.WriteByteSeq(lootedGuid[1]);
    data2.WriteByteSeq(lootedGuid[0]);
    data2.WriteByteSeq(lootedGuid[2]);
    data2 << uint8(roll.itemSlot);
    data2.WriteByteSeq(lootedGuid[7]);
    data2.WriteByteSeq(lootedGuid[4]);
    data2.WriteByteSeq(lootedGuid[6]);
    data2.WriteByteSeq(lootedGuid[3]);
    data2.WriteByteSeq(lootedGuid[5]);

    for (auto&& it : roll.playerVote)
    {
        if (Player* player = ObjectAccessor::FindPlayer(it.first))
        {
            if (it.second != NOT_VALID)
            {
                player->GetSession()->SendPacket(&data);
                player->GetSession()->SendPacket(&data2);
            }
        }
    }
}

// notify group members which player is the allowed looter for the given creature
void Group::SendLooter(Creature* creature, Player* pLooter)
{
    ASSERT(creature);

    ObjectGuid creatureGuid = creature->GetGUID();
    ObjectGuid looterGuid;
    ObjectGuid masterLooterGuid;
    if (GetLootMethod() == MASTER_LOOT)
        masterLooterGuid = GetLooterGuid();
    else
        looterGuid = pLooter ? pLooter->GetGUID() : ObjectGuid::Empty;
    WorldPacket data(SMSG_LOOT_LIST);

    data.WriteBit(creatureGuid[5]);
    data.WriteBit(masterLooterGuid != (uint64)0);
    if (masterLooterGuid)
    {
        data.WriteBit(masterLooterGuid[4]);
        data.WriteBit(masterLooterGuid[6]);
        data.WriteBit(masterLooterGuid[0]);
        data.WriteBit(masterLooterGuid[7]);
        data.WriteBit(masterLooterGuid[5]);
        data.WriteBit(masterLooterGuid[2]);
        data.WriteBit(masterLooterGuid[3]);
        data.WriteBit(masterLooterGuid[1]);
    }
    data.WriteBit(creatureGuid[1]);
    data.WriteBit(looterGuid != (uint64)0);
    data.WriteBit(creatureGuid[4]);
    data.WriteBit(creatureGuid[3]);
    data.WriteBit(creatureGuid[2]);
    if (looterGuid)
    {
        data.WriteBit(looterGuid[2]);
        data.WriteBit(looterGuid[3]);
        data.WriteBit(looterGuid[4]);
        data.WriteBit(looterGuid[5]);
        data.WriteBit(looterGuid[6]);
        data.WriteBit(looterGuid[0]);
        data.WriteBit(looterGuid[1]);
        data.WriteBit(looterGuid[7]);
    }
    data.WriteBit(creatureGuid[7]);
    data.WriteBit(creatureGuid[0]);
    data.WriteBit(creatureGuid[6]);
    data.FlushBits();

    if (looterGuid)
    {
        data.WriteByteSeq(looterGuid[7]);
        data.WriteByteSeq(looterGuid[1]);
        data.WriteByteSeq(looterGuid[0]);
        data.WriteByteSeq(looterGuid[6]);
        data.WriteByteSeq(looterGuid[5]);
        data.WriteByteSeq(looterGuid[3]);
        data.WriteByteSeq(looterGuid[4]);
        data.WriteByteSeq(looterGuid[2]);
    }

    if (masterLooterGuid)
    {
        data.WriteByteSeq(masterLooterGuid[4]);
        data.WriteByteSeq(masterLooterGuid[5]);
        data.WriteByteSeq(masterLooterGuid[6]);
        data.WriteByteSeq(masterLooterGuid[3]);
        data.WriteByteSeq(masterLooterGuid[2]);
        data.WriteByteSeq(masterLooterGuid[7]);
        data.WriteByteSeq(masterLooterGuid[0]);
        data.WriteByteSeq(masterLooterGuid[1]);
    }

    data.WriteByteSeq(creatureGuid[5]);
    data.WriteByteSeq(creatureGuid[1]);
    data.WriteByteSeq(creatureGuid[6]);
    data.WriteByteSeq(creatureGuid[2]);
    data.WriteByteSeq(creatureGuid[3]);
    data.WriteByteSeq(creatureGuid[0]);
    data.WriteByteSeq(creatureGuid[7]);
    data.WriteByteSeq(creatureGuid[4]);
    
    BroadcastPacket(&data, false);
}

void Group::GroupLoot(Loot* loot, WorldObject* pLootedObject)
{
    std::vector<LootItem>::iterator i;
    ItemTemplate const* item;
    uint8 itemSlot = 0;

    for (i = loot->items.begin(); i != loot->items.end(); ++i, ++itemSlot)
    {
        if (i->freeforall || i->currency)
            continue;

        item = sObjectMgr->GetItemTemplate(i->itemid);
        if (!item)
        {
            //TC_LOG_DEBUG("misc", "Group::GroupLoot: missing item prototype for item with id: %d", i->itemid);
            continue;
        }

        //roll for over-threshold item if it's one-player loot
        if (item->Quality >= uint32(m_lootThreshold))
        {
            Roll* r = new Roll(loot->GetGUID(), *i);

            //a vector is filled with only near party members
            for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* member = itr->GetSource();
                if (!member || !member->GetSession())
                    continue;
                if (i->AllowedForPlayer(member, pLootedObject))
                {
                    if (member->CanRollForLootIn(pLootedObject))
                    {
                        r->totalPlayersRolling++;

                        if (member->GetPassOnGroupLoot())
                        {
                            r->playerVote[member->GetGUID()] = PASS;
                            r->totalPass++;
                            // can't broadcast the pass now. need to wait until all rolling players are known.
                        }
                        else
                            r->playerVote[member->GetGUID()] = NOT_EMITED_YET;
                    }
                }
            }

            if (r->totalPlayersRolling > 0)
            {
                r->setLoot(loot);
                r->itemSlot = itemSlot;
                if (item->DisenchantID && m_maxEnchantingLevel >= item->RequiredDisenchantSkill)
                    r->rollVoteMask |= ROLL_FLAG_TYPE_DISENCHANT;

                loot->items[itemSlot].is_blocked = true;

                // If there is any "auto pass", broadcast the pass now.
                if (r->totalPass)
                {
                    for (Roll::PlayerVote::const_iterator itr=r->playerVote.begin(); itr != r->playerVote.end(); ++itr)
                    {
                        Player* p = ObjectAccessor::FindPlayer(itr->first);
                        if (!p || !p->GetSession())
                            continue;

                        if (itr->second == PASS)
                            SendLootRoll(p->GetGUID(), 128, ROLL_PASS, *r);
                    }
                }

                SendLootStartRoll(60000, pLootedObject->GetMapId(), *r);

                RollId.push_back(r);

                if (Creature* creature = pLootedObject->ToCreature())
                {
                    creature->m_groupLootTimer = 60000;
                    creature->lootingGroupLowGUID = GetLowGUID();
                }
                else if (GameObject* go = pLootedObject->ToGameObject())
                {
                    go->m_groupLootTimer = 60000;
                    go->lootingGroupLowGUID = GetLowGUID();
                }
            }
            else
                delete r;
        }
        else
            i->is_underthreshold = true;
    }

    for (i = loot->quest_items.begin(); i != loot->quest_items.end(); ++i, ++itemSlot)
    {
        if (!i->follow_loot_rules)
            continue;

        item = sObjectMgr->GetItemTemplate(i->itemid);
        if (!item)
        {
            //TC_LOG_DEBUG("misc", "Group::GroupLoot: missing item prototype for item with id: %d", i->itemid);
            continue;
        }

        Roll* r = new Roll(pLootedObject->GetGUID(), *i);

        //a vector is filled with only near party members
        for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* member = itr->GetSource();
            if (!member || !member->GetSession())
                continue;

            if (i->AllowedForPlayer(member, pLootedObject))
            {
                if (member->CanRollForLootIn(pLootedObject))
                {
                    r->totalPlayersRolling++;
                    r->playerVote[member->GetGUID()] = NOT_EMITED_YET;
                }
            }
        }

        if (r->totalPlayersRolling > 0)
        {
            r->setLoot(loot);
            r->itemSlot = itemSlot;

            loot->quest_items[itemSlot - loot->items.size()].is_blocked = true;

            SendLootStartRoll(60000, pLootedObject->GetMapId(), *r);

            RollId.push_back(r);

            if (Creature* creature = pLootedObject->ToCreature())
            {
                creature->m_groupLootTimer = 60000;
                creature->lootingGroupLowGUID = GetLowGUID();
            }
            else if (GameObject* go = pLootedObject->ToGameObject())
            {
                go->m_groupLootTimer = 60000;
                go->lootingGroupLowGUID = GetLowGUID();
            }
        }
        else
            delete r;
    }
}

void Group::NeedBeforeGreed(Loot* loot, WorldObject* lootedObject)
{
    ItemTemplate const* item;
    uint8 itemSlot = 0;
    for (std::vector<LootItem>::iterator i = loot->items.begin(); i != loot->items.end(); ++i, ++itemSlot)
    {
        if (i->freeforall || i->currency)
            continue;

        item = sObjectMgr->GetItemTemplate(i->itemid);

        //roll for over-threshold item if it's one-player loot
        if (item->Quality >= uint32(m_lootThreshold))
        {
            Roll* r = new Roll(lootedObject->GetGUID(), *i);

            for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* playerToRoll = itr->GetSource();
                if (!playerToRoll || !playerToRoll->GetSession())
                    continue;

                ObjectGuid playerGuid = playerToRoll->GetGUID();

                if (i->AllowedForPlayer(playerToRoll, lootedObject))
                {
                    if (playerToRoll->CanRollForLootIn(lootedObject))
                    {
                        r->totalPlayersRolling++;
                        if (playerToRoll->GetPassOnGroupLoot())
                        {
                            r->playerVote[playerGuid] = PASS;
                            r->totalPass++;
                            // can't broadcast the pass now. need to wait until all rolling players are known.
                        }
                        else
                        {
                            r->playerVote[playerGuid] = NOT_EMITED_YET;
                        }
                    }
                }
            }

            if (r->totalPlayersRolling > 0)
            {
                r->setLoot(loot);
                r->itemSlot = itemSlot;
                if (item->DisenchantID && m_maxEnchantingLevel >= item->RequiredDisenchantSkill)
                    r->rollVoteMask |= ROLL_FLAG_TYPE_DISENCHANT;

                if (item->Flags2 & ITEM_FLAGS_EXTRA_NEED_ROLL_DISABLED)
                    r->rollVoteMask &= ~ROLL_FLAG_TYPE_NEED;

                loot->items[itemSlot].is_blocked = true;

                //Broadcast Pass and Send Rollstart
                for (Roll::PlayerVote::const_iterator itr = r->playerVote.begin(); itr != r->playerVote.end(); ++itr)
                {
                    Player* p = ObjectAccessor::FindPlayer(itr->first);
                    if (!p || !p->GetSession())
                        continue;

                    if (itr->second == PASS)
                        SendLootRoll(p->GetGUID(), 128, ROLL_PASS, *r);
                    else
                        SendLootStartRollToPlayer(60000, lootedObject->GetMapId(), p, p->CanRollForItemInLFG(item, lootedObject) == EQUIP_ERR_OK, *r);
                }

                RollId.push_back(r);

                if (Creature* creature = lootedObject->ToCreature())
                {
                    creature->m_groupLootTimer = 60000;
                    creature->lootingGroupLowGUID = GetLowGUID();
                }
                else if (GameObject* go = lootedObject->ToGameObject())
                {
                    go->m_groupLootTimer = 60000;
                    go->lootingGroupLowGUID = GetLowGUID();
                }
            }
            else
                delete r;
        }
        else
            i->is_underthreshold = true;
    }

    for (std::vector<LootItem>::iterator i = loot->quest_items.begin(); i != loot->quest_items.end(); ++i, ++itemSlot)
    {
        if (!i->follow_loot_rules)
            continue;

        item = sObjectMgr->GetItemTemplate(i->itemid);
        Roll* r = new Roll(lootedObject->GetGUID(), *i);

        for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* playerToRoll = itr->GetSource();
            if (!playerToRoll || !playerToRoll->GetSession())
                continue;

            if (i->AllowedForPlayer(playerToRoll, lootedObject))
            {
                if (playerToRoll->CanRollForLootIn(lootedObject))
                {
                    r->totalPlayersRolling++;
                    r->playerVote[playerToRoll->GetGUID()] = NOT_EMITED_YET;
                }
            }
        }

        if (r->totalPlayersRolling > 0)
        {
            r->setLoot(loot);
            r->itemSlot = itemSlot;

            loot->quest_items[itemSlot - loot->items.size()].is_blocked = true;

            //Broadcast Pass and Send Rollstart
            for (Roll::PlayerVote::const_iterator itr = r->playerVote.begin(); itr != r->playerVote.end(); ++itr)
            {
                Player* p = ObjectAccessor::FindPlayer(itr->first);
                if (!p || !p->GetSession())
                    continue;

                if (itr->second == PASS)
                    SendLootRoll(p->GetGUID(), 128, ROLL_PASS, *r);
                else
                    SendLootStartRollToPlayer(60000, lootedObject->GetMapId(), p, p->CanRollForItemInLFG(item, lootedObject) == EQUIP_ERR_OK, *r);
            }

            RollId.push_back(r);

            if (Creature* creature = lootedObject->ToCreature())
            {
                creature->m_groupLootTimer = 60000;
                creature->lootingGroupLowGUID = GetLowGUID();
            }
            else if (GameObject* go = lootedObject->ToGameObject())
            {
                go->m_groupLootTimer = 60000;
                go->lootingGroupLowGUID = GetLowGUID();
            }
        }
        else
            delete r;
    }
}

void Group::MasterLoot(Loot* loot, WorldObject* pLootedObject)
{
    TC_LOG_DEBUG("network", "Group::MasterLoot (SMSG_LOOT_MASTER_LIST)");

    for (auto&& it : loot->items)
        if (!it.freeforall && (it.follow_loot_rules || it.conditions.empty()))
            it.is_blocked = !it.is_underthreshold;

    uint32 realCount = 0;
    ObjectGuid lootedGuid = loot->GetGUID();
    WorldPacket data(SMSG_LOOT_MASTER_LIST);

    data.WriteBit(lootedGuid[1]);

    size_t pos = data.bitwpos();
    data.WriteBits(realCount, 24);

    data.WriteBit(lootedGuid[5]);
    for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* looter = itr->GetSource();
        if (!looter->IsInWorld())
            continue;

        if (looter->CanRollForLootIn(pLootedObject))
        {
            ObjectGuid looterGuid = looter->GetGUID();
            data.WriteBit(looterGuid[4]);
            data.WriteBit(looterGuid[1]);
            data.WriteBit(looterGuid[2]);
            data.WriteBit(looterGuid[5]);
            data.WriteBit(looterGuid[3]);
            data.WriteBit(looterGuid[7]);
            data.WriteBit(looterGuid[0]);
            data.WriteBit(looterGuid[6]);
            ++realCount;
        }
    }

    data.WriteBit(lootedGuid[2]);
    data.WriteBit(lootedGuid[0]);
    data.WriteBit(lootedGuid[6]);
    data.WriteBit(lootedGuid[7]);
    data.WriteBit(lootedGuid[3]);
    data.WriteBit(lootedGuid[4]);

    data.FlushBits();
    data.PutBits(pos, realCount, 24);

    data.WriteByteSeq(lootedGuid[3]);
    data.WriteByteSeq(lootedGuid[0]);
    data.WriteByteSeq(lootedGuid[2]);

    for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* looter = itr->GetSource();
        if (!looter->IsInWorld())
            continue;

        if (looter->CanRollForLootIn(pLootedObject))
        {
            ObjectGuid looterGuid = looter->GetGUID();
            data.WriteByteSeq(looterGuid[1]);
            data.WriteByteSeq(looterGuid[3]);
            data.WriteByteSeq(looterGuid[7]);
            data.WriteByteSeq(looterGuid[2]);
            data.WriteByteSeq(looterGuid[0]);
            data.WriteByteSeq(looterGuid[6]);
            data.WriteByteSeq(looterGuid[4]);
            data.WriteByteSeq(looterGuid[5]);
        }
    }

    data.WriteByteSeq(lootedGuid[7]);
    data.WriteByteSeq(lootedGuid[1]);
    data.WriteByteSeq(lootedGuid[6]);
    data.WriteByteSeq(lootedGuid[4]);
    data.WriteByteSeq(lootedGuid[5]);

    for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* looter = itr->GetSource();
        if (looter->CanRollForLootIn(pLootedObject))
            looter->GetSession()->SendPacket(&data);
    }
}

void Group::CountRollVote(ObjectGuid lootGuid, ObjectGuid playerGUID, uint8 slot, uint8 choice)
{
    Rolls::iterator rollI = GetRoll(lootGuid, slot);
    if (rollI == RollId.end())
        return;

    Roll* roll = *rollI;
    Roll::PlayerVote::iterator itr = roll->playerVote.find(playerGUID);
    // this condition means that player joins to the party after roll begins
    if (itr == roll->playerVote.end())
        return;

    if (roll->getLoot())
        if (roll->getLoot()->items.empty())
            return;

    switch (choice)
    {
        case ROLL_PASS:                                     // Player choose pass
            SendLootRoll(playerGUID, -1, choice, *roll);
            ++roll->totalPass;
            itr->second = PASS;
            break;
        case ROLL_NEED:                                     // player choose Need
            SendLootRoll(playerGUID, 0, choice, *roll);  // there is must be rolled number, but it seems to work fine
            ++roll->totalNeed;
            itr->second = NEED;
            break;
        case ROLL_GREED:                                    // player choose Greed
            SendLootRoll(playerGUID, -7, choice, *roll);
            ++roll->totalGreed;
            itr->second = GREED;
            break;
        case ROLL_DISENCHANT:                               // player choose Disenchant
            SendLootRoll(playerGUID, -8, choice, *roll);
            ++roll->totalGreed;
            itr->second = DISENCHANT;
            break;
    }

    if (roll->totalPass + roll->totalNeed + roll->totalGreed >= roll->totalPlayersRolling)
        CountTheRoll(rollI);
}

//called when roll timer expires
void Group::EndRoll(Loot* pLoot)
{
    for (Rolls::iterator itr = RollId.begin(); itr != RollId.end();)
    {
        if ((*itr)->getLoot() == pLoot)
        {
            CountTheRoll(itr);           //i don't have to edit player votes, who didn't vote ... he will pass
            itr = RollId.begin();
        }
        else
            ++itr;
    }
}

void Group::CountTheRoll(Rolls::iterator rollI)
{
    Roll* roll = *rollI;
    if (!roll->isValid())                                   // is loot already deleted ?
    {
        RollId.erase(rollI);
        delete roll;
        return;
    }

    //end of the roll
    if (roll->totalNeed > 0)
    {
        if (!roll->playerVote.empty())
        {
            uint8 maxresul = 0;
            ObjectGuid maxguid  = (*roll->playerVote.begin()).first;
            Player* player;

            for (Roll::PlayerVote::const_iterator itr=roll->playerVote.begin(); itr != roll->playerVote.end(); ++itr)
            {
                if (itr->second != NEED)
                    continue;

                uint8 randomN = urand(1, 100);
                SendLootRoll(itr->first, randomN, ROLL_NEED, *roll);
                if (maxresul < randomN)
                {
                    maxguid  = itr->first;
                    maxresul = randomN;
                }

            }
            SendLootRollWon(maxguid, maxresul, ROLL_NEED, *roll);
            player = ObjectAccessor::FindPlayer(maxguid);

            if (player && player->GetSession())
            {
                player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED_ON_LOOT, roll->itemid, maxresul);

                ItemPosCountVec dest;
                LootItem* item = &(roll->itemSlot >= roll->getLoot()->items.size() ? roll->getLoot()->quest_items[roll->itemSlot - roll->getLoot()->items.size()] : roll->getLoot()->items[roll->itemSlot]);
                InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, roll->itemid, item->count);
                if (msg == EQUIP_ERR_OK)
                {
                    item->is_looted = true;
                    roll->getLoot()->NotifyItemRemoved(roll->itemSlot);
                    roll->getLoot()->unlootedCount--;
                    Item* newitem = player->StoreNewItem(dest, roll->itemid, true, item->randomPropertyId, item->GetAllowedLooters());
                    if (newitem)
                        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_LOOT_ITEM, item->itemid, item->count);

                    sScriptMgr->OnItemPickup(player, newitem, roll->getLoot()->GetItemPickupSourceType(), roll->getLoot()->sourceEntry);
                }
                else
                {
                    item->is_blocked = false;
                    player->SendEquipError(msg, NULL, NULL, roll->itemid);
                }
            }
        }
    }
    else if (roll->totalGreed > 0)
    {
        if (!roll->playerVote.empty())
        {
            uint8 maxresul = 0;
            ObjectGuid maxguid = (*roll->playerVote.begin()).first;
            Player* player;
            RollVote rollvote = NOT_VALID;

            Roll::PlayerVote::iterator itr;
            for (itr = roll->playerVote.begin(); itr != roll->playerVote.end(); ++itr)
            {
                if (itr->second != GREED && itr->second != DISENCHANT)
                    continue;

                uint8 randomN = urand(1, 100);
                SendLootRoll(itr->first, randomN, itr->second, *roll);
                if (maxresul < randomN)
                {
                    maxguid  = itr->first;
                    maxresul = randomN;
                    rollvote = itr->second;
                }

            }
            SendLootRollWon(maxguid, maxresul, rollvote, *roll);
            player = ObjectAccessor::FindPlayer(maxguid);

            if (player && player->GetSession())
            {
                player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED_ON_LOOT, roll->itemid, maxresul);

                LootItem* item = &(roll->itemSlot >= roll->getLoot()->items.size() ? roll->getLoot()->quest_items[roll->itemSlot - roll->getLoot()->items.size()] : roll->getLoot()->items[roll->itemSlot]);

                if (rollvote == GREED)
                {
                    ItemPosCountVec dest;
                    InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, roll->itemid, item->count);
                    if (msg == EQUIP_ERR_OK)
                    {
                        item->is_looted = true;
                        roll->getLoot()->NotifyItemRemoved(roll->itemSlot);
                        roll->getLoot()->unlootedCount--;
                        Item* newitem = player->StoreNewItem(dest, roll->itemid, true, item->randomPropertyId, item->GetAllowedLooters());

                        sScriptMgr->OnItemPickup(player, newitem, roll->getLoot()->GetItemPickupSourceType(), roll->getLoot()->sourceEntry);
                    }
                    else
                    {
                        item->is_blocked = false;
                        player->SendEquipError(msg, NULL, NULL, roll->itemid);
                    }
                }
                else if (rollvote == DISENCHANT)
                {
                    item->is_looted = true;
                    roll->getLoot()->NotifyItemRemoved(roll->itemSlot);
                    roll->getLoot()->unlootedCount--;
                    ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(roll->itemid);
                    uint32 guildPerkChance = player->GetTotalAuraModifier(SPELL_AURA_MOD_GATHERING_ITEMS_GAINED_PERCENT);
                    player->AutoStoreLoot(pProto->DisenchantID, LootTemplates_Disenchant, ItemPickupSourceType::ItemLoot, roll->itemid, true, guildPerkChance);
                    player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_CAST_SPELL, 13262); // Disenchant
                }
            }
        }
    }
    else
    {
        SendLootAllPassed(*roll);

        // remove is_blocked so that the item is lootable by all players
        LootItem* item = &(roll->itemSlot >= roll->getLoot()->items.size() ? roll->getLoot()->quest_items[roll->itemSlot - roll->getLoot()->items.size()] : roll->getLoot()->items[roll->itemSlot]);
        if (item)
            item->is_blocked = false;
    }

    RollId.erase(rollI);
    delete roll;
}

void Group::SetTargetIcon(uint8 symbol, ObjectGuid whoGuid, ObjectGuid targetGuid, uint8 partyIndex)
{
    if (symbol >= TARGETICONCOUNT)
        return;

    // clean other icons
    if (targetGuid != 0)
        for (int i=0; i<TARGETICONCOUNT; ++i)
            if (m_targetIcons[i] == targetGuid)
                SetTargetIcon(i, ObjectGuid::Empty, ObjectGuid::Empty, partyIndex);

    m_targetIcons[symbol] = targetGuid;

    WorldPacket data(SMSG_RAID_TARGET_UPDATE_SINGLE, 8 + 1 + 8 + 1);

    data.WriteBit(whoGuid[6]);
    data.WriteBit(targetGuid[4]);
    data.WriteBit(whoGuid[0]);
    data.WriteBit(whoGuid[7]);
    data.WriteBit(targetGuid[6]);
    data.WriteBit(whoGuid[5]);
    data.WriteBit(whoGuid[3]);
    data.WriteBit(whoGuid[4]);
    data.WriteBit(targetGuid[7]);
    data.WriteBit(targetGuid[2]);
    data.WriteBit(targetGuid[5]);
    data.WriteBit(targetGuid[1]);
    data.WriteBit(whoGuid[2]);
    data.WriteBit(whoGuid[1]);
    data.WriteBit(targetGuid[0]);
    data.WriteBit(targetGuid[3]);

    data.WriteByteSeq(targetGuid[1]);

    data << uint8(partyIndex);

    data.WriteByteSeq(whoGuid[0]);
    data.WriteByteSeq(whoGuid[5]);
    data.WriteByteSeq(whoGuid[3]);
    data.WriteByteSeq(targetGuid[7]);
    data.WriteByteSeq(targetGuid[6]);
    data.WriteByteSeq(whoGuid[1]);
    data.WriteByteSeq(targetGuid[2]);
    data.WriteByteSeq(targetGuid[4]);
    data.WriteByteSeq(targetGuid[0]);
    data.WriteByteSeq(targetGuid[3]);
    data.WriteByteSeq(targetGuid[5]);
    data.WriteByteSeq(whoGuid[6]);

    data << uint8(symbol);

    data.WriteByteSeq(whoGuid[4]);
    data.WriteByteSeq(whoGuid[2]);
    data.WriteByteSeq(whoGuid[7]);

    BroadcastPacket(&data, true);
}

void Group::SendTargetIconList(WorldSession* session, int8 partyIndex)
{
    if (!session)
        return;

    WorldPackets::Party::SendRaidTargetUpdateAll updateAll;
    updateAll.PartyIndex = partyIndex;
    for (uint8 i = 0; i < TARGETICONCOUNT; i++)
        if (m_targetIcons[i] != ObjectGuid::Empty)
            updateAll.TargetIcons.insert(std::pair<uint8, ObjectGuid>(i, m_targetIcons[i]));

    session->SendPacket(updateAll.Write());
}

void Group::SendUpdate()
{
    for (member_witerator witr = m_memberSlots.begin(); witr != m_memberSlots.end(); ++witr)
        SendUpdateToPlayer(witr->guid, &(*witr));
}

void Group::SendUpdateToPlayer(ObjectGuid playerGUID, MemberSlot const* slot)
{
    ASSERT(slot);

    Player* player = ObjectAccessor::FindConnectedPlayer(playerGUID);

    if (!player)
        return;

    if (player->GetGroup() != this && player->GetOriginalGroup() != this)
        return;

    uint8 groupPosition = 0;
    uint8 i = 0;

    for (member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        if (citr->group != slot->group)
            continue;

        if (citr->guid == slot->guid)
        {
            groupPosition = i;
            break;
        }

        i++;
    }

    ByteBuffer memberData;
    ObjectGuid groupGuid = m_guid;
    ObjectGuid leaderGuid = m_leaderGuid;
    ObjectGuid looterGuid = m_looterGuid;

    WorldPacket data(SMSG_GROUP_LIST, 4 + 3 + 4 + 1 + 8 + 1 + 8 + 3 + (GetMembersCount() * (4 + 2 + 8)) + 1 + 8 + 2 + 4 + 4);
    data.WriteBit(groupGuid[0]);
    data.WriteBit(leaderGuid[7]);
    data.WriteBit(leaderGuid[1]);
    data.WriteBit(1);                                   // has dungeon and raid difficulty
    data.WriteBit(groupGuid[7]);
    data.WriteBit(leaderGuid[6]);
    data.WriteBit(leaderGuid[5]);
    data.WriteBits(GetMembersCount(), 21);

    for (member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {

        ObjectGuid memberGuid = citr->guid;
        std::string memberName = citr->name;

        Player* member = ObjectAccessor::FindPlayer(memberGuid);

        uint8 onlineState = member ? MEMBER_STATUS_ONLINE : MEMBER_STATUS_OFFLINE;
        onlineState = onlineState | ((isBGGroup() || isBFGroup()) ? MEMBER_STATUS_PVP : 0);

        data.WriteBit(memberGuid[1]);
        data.WriteBit(memberGuid[2]);
        data.WriteBit(memberGuid[5]);
        data.WriteBit(memberGuid[6]);
        data.WriteBits(memberName.size(), 6);
        data.WriteBit(memberGuid[7]);
        data.WriteBit(memberGuid[3]);
        data.WriteBit(memberGuid[0]);
        data.WriteBit(memberGuid[4]);

        memberData.WriteByteSeq(memberGuid[6]);
        memberData.WriteByteSeq(memberGuid[3]);
        memberData << uint8(citr->roles);
        memberData << uint8(onlineState);
        memberData.WriteByteSeq(memberGuid[7]);
        memberData.WriteByteSeq(memberGuid[4]);
        memberData.WriteByteSeq(memberGuid[1]);
        memberData.WriteString(memberName);
        memberData.WriteByteSeq(memberGuid[5]);
        memberData.WriteByteSeq(memberGuid[2]);
        memberData << uint8(citr->group);
        memberData.WriteByteSeq(memberGuid[0]);
        memberData << uint8(citr->flags);
    }

    data.WriteBit(leaderGuid[3]);
    data.WriteBit(leaderGuid[0]);
    data.WriteBit(1);                                   // has loot mode
    data.WriteBit(groupGuid[5]);

    //if (hasLootMode)
    {
        data.WriteBit(looterGuid[6]);
        data.WriteBit(looterGuid[4]);
        data.WriteBit(looterGuid[5]);
        data.WriteBit(looterGuid[2]);
        data.WriteBit(looterGuid[1]);
        data.WriteBit(looterGuid[0]);
        data.WriteBit(looterGuid[7]);
        data.WriteBit(looterGuid[3]);
    }

    data.WriteBit(groupGuid[2]);
    data.WriteBit(groupGuid[4]);
    data.WriteBit(groupGuid[1]);
    data.WriteBit(isLFGGroup());
    data.WriteBit(leaderGuid[2]);
    data.WriteBit(groupGuid[6]);

    if (isLFGGroup())
    {
        data.WriteBit(0);
        data.WriteBit(0);
    }

    data.WriteBit(leaderGuid[4]);
    data.WriteBit(groupGuid[3]);
    data.FlushBits();

    data.WriteByteSeq(leaderGuid[0]);

    //if (hasInstanceDifficulty)
    {
        data << uint32(m_dungeonDifficulty);            // dungeon Difficulty
        data << uint32(m_raidDifficulty);               // raid Difficulty
    }

    data.append(memberData);
    data.WriteByteSeq(groupGuid[1]);

    if (isLFGGroup())
    {
        data << float(1.0f);
        data << uint8(0);
        data << uint8(0);
        uint32 dungeonEntry = 0;
        if (LFGDungeonEntry const* dungeon = sLFGDungeonStore.LookupEntry(sLFGMgr->GetDungeon(GetGUID())))
            dungeonEntry = dungeon->Entry();
        data << uint32(dungeonEntry);
        data << uint8(0);
        data << uint8(0);
        data << uint8(0);
        data << uint32(0);
    }

    data.WriteByteSeq(leaderGuid[4]);
    data.WriteByteSeq(leaderGuid[2]);

    //if (hasLootMode)
    {
        data << uint8(m_lootMethod);
        data.WriteByteSeq(looterGuid[0]);
        data.WriteByteSeq(looterGuid[5]);
        data.WriteByteSeq(looterGuid[4]);
        data.WriteByteSeq(looterGuid[3]);
        data.WriteByteSeq(looterGuid[2]);
        data << uint8(m_lootThreshold);
        data.WriteByteSeq(looterGuid[7]);
        data.WriteByteSeq(looterGuid[1]);
        data.WriteByteSeq(looterGuid[6]);
    }

    data.WriteByteSeq(groupGuid[6]);
    data.WriteByteSeq(groupGuid[4]);
    data << uint8(m_groupType);
    data << uint8(m_slot);
    data << uint32(groupPosition);
    data.WriteByteSeq(groupGuid[7]);
    data.WriteByteSeq(leaderGuid[3]);
    data.WriteByteSeq(leaderGuid[1]);
    data << uint32(m_counter++);
    data.WriteByteSeq(groupGuid[0]);
    data.WriteByteSeq(groupGuid[2]);
    data.WriteByteSeq(groupGuid[5]);
    data.WriteByteSeq(groupGuid[3]);
    data.WriteByteSeq(leaderGuid[7]);
    data << uint8(0);
    data.WriteByteSeq(leaderGuid[5]);
    data.WriteByteSeq(leaderGuid[6]);

    player->GetSession()->SendPacket(&data);

    /*data << uint8(m_groupType);                         // group type (flags in 3.3)
    data << uint8(slot->group);
    data << uint8(slot->flags);
    data << uint8(slot->roles);
    if (isLFGGroup())
    {
        data << uint8(sLFGMgr->GetState(m_guid) == lfg::LFG_STATE_FINISHED_DUNGEON ? 2 : 0); // FIXME - Dungeon save status? 2 = done
        data << uint32(sLFGMgr->GetDungeon(m_guid));
        data << uint8(0); // 4.x new
    }

    data << uint64(m_guid);
    data << uint32(m_counter++);                        // 3.3, value increases every time this packet gets sent
    data << uint32(GetMembersCount()-1);
    for (member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        if (slot->guid == citr->guid)
            continue;

        Player* member = ObjectAccessor::FindPlayer(citr->guid);

        uint8 onlineState = member ? MEMBER_STATUS_ONLINE : MEMBER_STATUS_OFFLINE;
        onlineState = onlineState | ((isBGGroup() || isBFGroup()) ? MEMBER_STATUS_PVP : 0);

        data << citr->name;
        data << uint64(citr->guid);                     // guid
        data << uint8(onlineState);                     // online-state
        data << uint8(citr->group);                     // groupid
        data << uint8(citr->flags);                     // See enum GroupMemberFlags
        data << uint8(citr->roles);                     // Lfg Roles
    }

    data << uint64(m_leaderGuid);                       // leader guid

    if (GetMembersCount() - 1)
    {
        data << uint8(m_lootMethod);                    // loot method
        data << uint64(m_looterGuid);                   // looter guid
        data << uint8(m_lootThreshold);                 // loot threshold
        data << uint8(m_dungeonDifficulty);             // Dungeon Difficulty
        data << uint8(m_raidDifficulty);                // Raid Difficulty
    }*/
}

void Group::SendUpdateToPlayer(ObjectGuid playerGUID)
{
    MemberSlot* slot = nullptr;
    auto it = std::find_if(m_memberSlots.begin(), m_memberSlots.end(), [=](MemberSlot const& slot)
    {
        return slot.guid == playerGUID;
    });
    if (it != m_memberSlots.end())
        SendUpdateToPlayer(playerGUID, &(*it));
    else
        SendGroupRemoved(playerGUID);
}

void Group::SendGroupRemoved(ObjectGuid playerGUID)
{
    Player* player = ObjectAccessor::FindConnectedPlayer(playerGUID);

    if (!player)
        return;

    ObjectGuid groupGuid = m_guid;
    ObjectGuid leaderGuid = ObjectGuid::Empty;

    WorldPacket data(SMSG_GROUP_LIST, 4 + 1 + 1 + 1 + 4 + 1 + 8 + 1 + 8 + 3);
    data.WriteBit(groupGuid[0]);
    data.WriteBit(leaderGuid[7]);
    data.WriteBit(leaderGuid[1]);
    data.WriteBit(0);
    data.WriteBit(groupGuid[7]);
    data.WriteBit(leaderGuid[6]);
    data.WriteBit(leaderGuid[5]);
    data.WriteBits(0, 21);
    data.WriteBit(leaderGuid[3]);
    data.WriteBit(leaderGuid[0]);
    data.WriteBit(0);
    data.WriteBit(groupGuid[5]);
    data.WriteBit(groupGuid[2]);
    data.WriteBit(groupGuid[4]);
    data.WriteBit(groupGuid[1]);
    data.WriteBit(0);
    data.WriteBit(leaderGuid[2]);
    data.WriteBit(groupGuid[6]);
    data.WriteBit(leaderGuid[4]);
    data.WriteBit(groupGuid[3]);
    data.FlushBits();

    data.WriteByteSeq(leaderGuid[0]);
    data.WriteByteSeq(groupGuid[1]);
    data.WriteByteSeq(leaderGuid[4]);
    data.WriteByteSeq(leaderGuid[2]);
    data.WriteByteSeq(groupGuid[6]);
    data.WriteByteSeq(groupGuid[4]);

    data << uint8(0x10);
    data << uint8(m_slot);
    data << int32(-1);
    data.WriteByteSeq(groupGuid[7]);
    data.WriteByteSeq(leaderGuid[3]);
    data.WriteByteSeq(leaderGuid[1]);
    data << uint32(m_counter++);
    data.WriteByteSeq(groupGuid[0]);
    data.WriteByteSeq(groupGuid[2]);
    data.WriteByteSeq(groupGuid[5]);
    data.WriteByteSeq(groupGuid[3]);
    data.WriteByteSeq(leaderGuid[7]);
    data << uint8(0);
    data.WriteByteSeq(leaderGuid[5]);
    data.WriteByteSeq(leaderGuid[6]);

    player->GetSession()->SendPacket(&data);
}

void Group::SendReadyCheckCompleted()
{
    ObjectGuid guid = m_guid;

    WorldPacket data(SMSG_RAID_READY_CHECK_COMPLETED, 1 + 8 + 1);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[6]);

    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[5]);
    data << uint8(0);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[4]);

    BroadcastPacket(&data, false);
}

void Group::UpdatePlayerOutOfRange(Player* player)
{
    if (!player || !player->IsInWorld())
        return;

    WorldPacket data;
    player->GetSession()->BuildPartyMemberStatsChangedPacket(player, &data, player->GetGroupUpdateFlag(), player->GetGUID());

    Player* member;
    for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
    {
        member = itr->GetSource();
        if (member && !member->IsWithinDist(player, member->GetSightRange(), false))
            member->GetSession()->SendPacket(&data);
    }
}

void Group::BroadcastAddonMessagePacket(WorldPacket* packet, const std::string& prefix, bool ignorePlayersInBGRaid, int group, uint64 ignore)
{
    for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* player = itr->GetSource();
        if (!player || (ignore != 0 && player->GetGUID() == ignore) || (ignorePlayersInBGRaid && player->GetGroup() != this))
            continue;

        if (WorldSession* session = player->GetSession())
            if (session && (group == -1 || itr->getSubGroup() == group))
                if (session->IsAddonRegistered(prefix))
                    session->SendPacket(packet);
    }
}

void Group::BroadcastPacket(WorldPacket* packet, bool ignorePlayersInBGRaid, int group, ObjectGuid ignore)
{
    for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* player = itr->GetSource();
        if (!player || (ignore != 0 && player->GetGUID() == ignore) || (ignorePlayersInBGRaid && player->GetGroup() != this))
            continue;

        if (player->GetSession() && (group == -1 || itr->getSubGroup() == group))
            player->GetSession()->SendPacket(packet);
    }
}

void Group::BroadcastChat(WorldPacket const* packet, ObjectGuid sender, int32 group) const
{
    for (auto&& player : *this)
        if ((group == -1 || player->GetSubGroup() == uint8(group)) && !player->GetSocial()->HasIgnore(sender))
            player->GetSession()->SendPacket(packet);
}

void Group::BroadcastReadyCheck(WorldPacket* packet)
{
    for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* player = itr->GetSource();
        if (player && player->GetSession())
            if (IsLeader(player->GetGUID()) || IsAssistant(player->GetGUID()))
                player->GetSession()->SendPacket(packet);
    }
}

void Group::OfflineReadyCheck()
{
    ObjectGuid groupGuid = GetGUID();

    for (member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        Player* player = ObjectAccessor::FindPlayer(citr->guid);
        if (!player || !player->GetSession())
        {
            ObjectGuid playerGuid = citr->guid;

            WorldPacket data(SMSG_RAID_READY_CHECK_CONFIRM, 1 + 1 + 8 + 1 + 8);
            data.WriteBit(groupGuid[4]);
            data.WriteBit(playerGuid[5]);
            data.WriteBit(playerGuid[3]);
            data.WriteBit(0);
            data.WriteBit(groupGuid[2]);
            data.WriteBit(playerGuid[6]);
            data.WriteBit(groupGuid[3]);
            data.WriteBit(playerGuid[0]);
            data.WriteBit(playerGuid[1]);
            data.WriteBit(groupGuid[1]);
            data.WriteBit(groupGuid[5]);
            data.WriteBit(playerGuid[7]);
            data.WriteBit(playerGuid[4]);
            data.WriteBit(groupGuid[6]);
            data.WriteBit(playerGuid[2]);
            data.WriteBit(groupGuid[0]);
            data.WriteBit(groupGuid[7]);
            data.FlushBits();

            data.WriteByteSeq(playerGuid[4]);
            data.WriteByteSeq(playerGuid[2]);
            data.WriteByteSeq(playerGuid[1]);
            data.WriteByteSeq(groupGuid[4]);
            data.WriteByteSeq(groupGuid[2]);
            data.WriteByteSeq(playerGuid[0]);
            data.WriteByteSeq(groupGuid[5]);
            data.WriteByteSeq(groupGuid[3]);
            data.WriteByteSeq(playerGuid[7]);
            data.WriteByteSeq(groupGuid[6]);
            data.WriteByteSeq(groupGuid[1]);
            data.WriteByteSeq(playerGuid[6]);
            data.WriteByteSeq(playerGuid[3]);
            data.WriteByteSeq(playerGuid[5]);
            data.WriteByteSeq(groupGuid[0]);
            data.WriteByteSeq(groupGuid[7]);

            BroadcastPacket(&data, false);

            ReadyCheckMemberHasResponded(playerGuid);
        }
    }
}

bool Group::_setMembersGroup(ObjectGuid guid, uint8 group)
{
    member_witerator slot = _getMemberWSlot(guid);
    if (slot == m_memberSlots.end())
        return false;

    slot->group = group;

    SubGroupCounterIncrease(group);

    if (!isBGGroup() && !isBFGroup())
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GROUP_MEMBER_SUBGROUP);

        stmt->setUInt8(0, group);
        stmt->setUInt32(1, guid.GetCounter());

        CharacterDatabase.Execute(stmt);
    }

    return true;
}

bool Group::SameSubGroup(Player const* member1, Player const* member2) const
{
    if (!member1 || !member2)
        return false;

    if (member1->GetGroup() != this || member2->GetGroup() != this)
        return false;
    else
        return member1->GetSubGroup() == member2->GetSubGroup();
}

// Allows setting sub groups both for online or offline members
void Group::ChangeMembersGroup(ObjectGuid guid, uint8 group)
{
    // Only raid groups have sub groups
    if (!isRaidGroup())
        return;

    // Check if player is really in the raid
    member_witerator slot = _getMemberWSlot(guid);
    if (slot == m_memberSlots.end())
        return;

    // Abort if the player is already in the target sub group
    uint8 prevSubGroup = GetMemberGroup(guid);
    if (prevSubGroup == group)
        return;

    // Update the player slot with the new sub group setting
    slot->group = group;

    // Increase the counter of the new sub group..
    SubGroupCounterIncrease(group);

    // ..and decrease the counter of the previous one
    SubGroupCounterDecrease(prevSubGroup);

    // Preserve new sub group in database for non-raid groups
    if (!isBGGroup() && !isBFGroup())
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GROUP_MEMBER_SUBGROUP);

        stmt->setUInt8(0, group);
        stmt->setUInt32(1, guid.GetCounter());

        CharacterDatabase.Execute(stmt);
    }

    // In case the moved player is online, update the player object with the new sub group references
    if (Player* player = ObjectAccessor::FindPlayer(guid))
        player->SetSubGroup(m_slot, group);

    // Broadcast the changes to the group
    SendUpdate();
}

// Retrieve the next Round-Roubin player for the group
//
// No update done if loot method is Master or FFA.
//
// If the RR player is not yet set for the group, the first group member becomes the round-robin player.
// If the RR player is set, the next player in group becomes the round-robin player.
//
// If ifneed is true,
//      the current RR player is checked to be near the looted object.
//      if yes, no update done.
//      if not, he loses his turn.
void Group::UpdateLooterGuid(WorldObject* pLootedObject, bool ifneed)
{
    switch (GetLootMethod())
    {
        case MASTER_LOOT:
        case FREE_FOR_ALL:
            return;
        default:
            // round robin style looting applies for all low
            // quality items in each loot method except free for all and master loot
            break;
    }

    uint64 oldLooterGUID = GetLooterGuid();
    member_citerator guid_itr = _getMemberCSlot(oldLooterGUID);
    if (guid_itr != m_memberSlots.end())
    {
        if (ifneed)
        {
            // not update if only update if need and ok
            Player* looter = ObjectAccessor::FindPlayer(guid_itr->guid);
            if (looter && looter->CanRollForLootIn(pLootedObject))
                return;
        }
        ++guid_itr;
    }

    // search next after current
    Player* pNewLooter = NULL;
    for (member_citerator itr = guid_itr; itr != m_memberSlots.end(); ++itr)
    {
        if (Player* player = ObjectAccessor::FindPlayer(itr->guid))
            if (player->CanRollForLootIn(pLootedObject))
            {
                pNewLooter = player;
                break;
            }
    }

    if (!pNewLooter)
    {
        // search from start
        for (member_citerator itr = m_memberSlots.begin(); itr != guid_itr; ++itr)
        {
            if (Player* player = ObjectAccessor::FindPlayer(itr->guid))
                if (player->CanRollForLootIn(pLootedObject))
                {
                    pNewLooter = player;
                    break;
                }
        }
    }

    if (pNewLooter)
    {
        if (oldLooterGUID != pNewLooter->GetGUID())
        {
            SetLooterGuid(pNewLooter->GetGUID());
            SendUpdate();
        }
    }
    else
    {
        SetLooterGuid(ObjectGuid::Empty);
        SendUpdate();
    }
}

GroupJoinBattlegroundResult Group::CanJoinBattlegroundQueue(Battleground const* bgOrTemplate, BattlegroundQueueTypeId bgQueueTypeId, uint32 MinPlayerCount, uint32 /*MaxPlayerCount*/, bool isRated, uint32 arenaSlot)
{
    // check if this group is LFG group
    if (isLFGGroup())
        return ERR_LFG_CANT_USE_BATTLEGROUND;

    BattlemasterListEntry const* bgEntry = sBattlemasterListStore.LookupEntry(bgOrTemplate->GetTypeID());
    if (!bgEntry)
        return ERR_BATTLEGROUND_JOIN_FAILED;            // shouldn't happen

    // check for min / max count
    uint32 memberscount = GetMembersCount();

    if (memberscount > bgEntry->maxGroupSize)                // no MinPlayerCount for battlegrounds
        return ERR_BATTLEGROUND_NONE;                        // ERR_GROUP_JOIN_BATTLEGROUND_TOO_MANY handled on client side

    // get a player as reference, to compare other players' stats to (arena team id, queue id based on level, etc.)
    Player* reference = GetFirstMember()->GetSource();
    // no reference found, can't join this way
    if (!reference)
        return ERR_BATTLEGROUND_JOIN_FAILED;

    PvPDifficultyEntry const* bracketEntry = sDBCManager.GetBattlegroundBracketByLevel(bgOrTemplate->GetMapId(), reference->GetLevel());
    if (!bracketEntry)
        return ERR_BATTLEGROUND_JOIN_FAILED;

    uint32 team = reference->GetTeam();

    BattlegroundQueueTypeId bgQueueTypeIdRandom = BattlegroundMgr::BGQueueTypeId(BATTLEGROUND_RB, 0);

    // check every member of the group to be able to join
    memberscount = 0;
    for (GroupReference* itr = GetFirstMember(); itr != nullptr; itr = itr->next(), ++memberscount)
    {
        Player* member = itr->GetSource();
        // offline member? don't let join
        if (!member)
            return ERR_BATTLEGROUND_JOIN_FAILED;
        // don't allow cross-faction join as group
        if (member->GetTeam() != team)
            return ERR_BATTLEGROUND_JOIN_TIMED_OUT;
        // not in the same battleground level braket, don't let join
        PvPDifficultyEntry const* memberBracketEntry = sDBCManager.GetBattlegroundBracketByLevel(bracketEntry->MapID, member->GetLevel());
        if (memberBracketEntry != bracketEntry)
            return ERR_BATTLEGROUND_JOIN_RANGE_INDEX;
        // don't let join if someone from the group is already in that bg queue
        if (member->InBattlegroundQueueForBattlegroundQueueType(bgQueueTypeId))
            return ERR_BATTLEGROUND_JOIN_FAILED;            // not blizz-like
        // don't let join if someone from the group is in bg queue random
        if (member->InBattlegroundQueueForBattlegroundQueueType(bgQueueTypeIdRandom))
            return ERR_IN_RANDOM_BG;
        // don't let join to bg queue random if someone from the group is already in bg queue
        if (bgOrTemplate->GetTypeID() == BATTLEGROUND_RB && member->InBattlegroundQueue())
            return ERR_IN_NON_RANDOM_BG;
        // check for deserter debuff
        if (!member->CanJoinToBattleground(bgOrTemplate))
            return ERR_GROUP_JOIN_BATTLEGROUND_DESERTERS;
        // check if member can join any more battleground queues
        if (!member->HasFreeBattlegroundQueueId())
            return ERR_BATTLEGROUND_TOO_MANY_QUEUES;        // not blizz-like
        // check if someone in party is using dungeon system
        if (member->IsUsingLfg(!bgOrTemplate->IsArena()))
            return ERR_LFG_CANT_USE_BATTLEGROUND;
        for (auto itr : { BATTLEGROUND_QUEUE_2v2, BATTLEGROUND_QUEUE_3v3, BATTLEGROUND_QUEUE_5v5, BATTLEGROUND_QUEUE_SOLO })
            if (member->InBattlegroundQueueForBattlegroundQueueType(itr))
                return ERR_BATTLEGROUND_QUEUED_FOR_RATED;
    }

    // only check for MinPlayerCount since MinPlayerCount == MaxPlayerCount for arenas...
    if (bgOrTemplate->IsArena() && memberscount != MinPlayerCount)
        return ERR_ARENA_TEAM_PARTY_SIZE;

    return ERR_BATTLEGROUND_NONE;
}

//===================================================
//============== Roll ===============================
//===================================================

void Roll::targetObjectBuildLink()
{
    // called from link()
    getTarget()->addLootValidatorRef(this);
}

void Group::SetDungeonDifficulty(Difficulty difficulty)
{
    m_dungeonDifficulty = difficulty;
    if (!isBGGroup() && !isBFGroup())
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GROUP_DIFFICULTY);

        stmt->setUInt8(0, uint8(m_dungeonDifficulty));
        stmt->setUInt32(1, m_dbStoreId);

        CharacterDatabase.Execute(stmt);
    }

    for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* player = itr->GetSource();
        if (!player->GetSession())
            continue;

        player->SetDungeonDifficulty(difficulty);
        player->SendDungeonDifficulty(GetDungeonDifficulty());
    }

    SendUpdate();
}

void Group::SetRaidDifficulty(Difficulty difficulty)
{
    m_raidDifficulty = difficulty;
    if (!isBGGroup() && !isBFGroup())
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GROUP_RAID_DIFFICULTY);

        stmt->setUInt8(0, uint8(m_raidDifficulty));
        stmt->setUInt32(1, m_dbStoreId);

        CharacterDatabase.Execute(stmt);
    }

    for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* player = itr->GetSource();
        if (!player->GetSession())
            continue;

        player->SetRaidDifficulty(difficulty);
        player->SendRaidDifficulty(GetRaidDifficulty());
    }

    SendUpdate();
}

bool Group::InCombatToInstance(uint32 instanceId)
{
    for (GroupReference* itr = GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* player = itr->GetSource();
        if (player && !player->getAttackers().empty() && player->GetInstanceId() == instanceId && (player->GetMap()->IsRaidOrHeroicDungeon()))
            for (std::set<Unit*>::const_iterator i = player->getAttackers().begin(); i != player->getAttackers().end(); ++i)
                if ((*i) && (*i)->GetTypeId() == TYPEID_UNIT && (*i)->ToCreature()->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_INSTANCE_BIND)
                    return true;
    }
    return false;
}

void Group::ResetInstances(uint8 method, bool isRaid, Player* SendMsgTo)
{
    if (isBGGroup() || isBFGroup())
        return;

    // method can be INSTANCE_RESET_ALL, INSTANCE_RESET_CHANGE_DIFFICULTY, INSTANCE_RESET_GROUP_DISBAND

    // we assume that when the difficulty changes, all instances that can be reset will be
    Difficulty diff = GetDifficulty(isRaid);

    for (BoundInstancesMap::iterator itr = m_boundInstances[diff].begin(); itr != m_boundInstances[diff].end();)
    {
        InstanceSave* instanceSave = itr->second.save;
        const MapEntry* entry = sMapStore.LookupEntry(itr->first);
        if (!entry || entry->IsRaid() != isRaid || (!instanceSave->CanReset() && method != INSTANCE_RESET_GROUP_DISBAND))
        {
            ++itr;
            continue;
        }

        if (method == INSTANCE_RESET_ALL)
        {
            // the "reset all instances" method can only reset normal maps
            if (entry->map_type == MAP_RAID || diff > DUNGEON_DIFFICULTY_NORMAL && diff != DUNGEON_DIFFICULTY_CHALLENGE && diff != SCENARIO_DIFFICULTY_NORMAL)
            {
                ++itr;
                continue;
            }
        }

        bool isEmpty = true;
        // if the map is loaded, reset it
        Map* map = sMapMgr->FindMap(instanceSave->GetMapId(), instanceSave->GetInstanceId());
        if (map && map->IsDungeon() && !(method == INSTANCE_RESET_GROUP_DISBAND && !instanceSave->CanReset()))
        {
            if (instanceSave->CanReset())
                isEmpty = ((InstanceMap*)map)->Reset(method);
            else
                isEmpty = !map->HavePlayers();
        }

        if (SendMsgTo)
        {
            if (!isEmpty)
                SendMsgTo->SendResetInstanceFailed(0, instanceSave->GetMapId());
            else if (sWorld->getBoolConfig(CONFIG_INSTANCES_RESET_ANNOUNCE))
            {
                if (Group* group = SendMsgTo->GetGroup())
                {
                    for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
                        if (Player* player = itr->GetSource())
                            player->SendResetInstanceSuccess(instanceSave->GetMapId());
                }

                else
                    SendMsgTo->SendResetInstanceSuccess(instanceSave->GetMapId());
            }
            else
                SendMsgTo->SendResetInstanceSuccess(instanceSave->GetMapId());
        }

        if (isEmpty || method == INSTANCE_RESET_GROUP_DISBAND || method == INSTANCE_RESET_CHANGE_DIFFICULTY)
        {
            // do not reset the instance, just unbind if others are permanently bound to it
            if (instanceSave->CanReset())
                instanceSave->DeleteFromDB();
            else
            {
                CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GROUP_INSTANCE_BY_INSTANCE);

                stmt->setUInt32(0, instanceSave->GetInstanceId());

                CharacterDatabase.Execute(stmt);
            }


            // i don't know for sure if hash_map iterators
            m_boundInstances[diff].erase(itr);
            itr = m_boundInstances[diff].begin();
            // this unloads the instance save unless online players are bound to it
            // (eg. permanent binds or GM solo binds)
            instanceSave->RemoveGroup(this);
        }
        else
            ++itr;
    }
}

InstanceGroupBind* Group::GetBoundInstance(Player* player)
{
    uint32 mapid = player->GetMapId();
    MapEntry const* mapEntry = sMapStore.LookupEntry(mapid);
    return GetBoundInstance(mapEntry);
}

InstanceGroupBind* Group::GetBoundInstance(Map* aMap)
{
    // Currently spawn numbering not different from map difficulty
    Difficulty difficulty = GetDifficulty(aMap->IsRaid());
    return GetBoundInstance(difficulty, aMap->GetId());
}

InstanceGroupBind* Group::GetBoundInstance(MapEntry const* mapEntry)
{
    if (!mapEntry || !mapEntry->IsDungeon())
        return NULL;

    Difficulty difficulty = GetDifficulty(mapEntry->IsRaid());
    return GetBoundInstance(difficulty, mapEntry->MapID);
}

InstanceGroupBind* Group::GetBoundInstance(Difficulty difficulty, uint32 mapId)
{
    // some instances only have one difficulty
    sDBCManager.GetDownscaledMapDifficultyData(mapId, difficulty);

    BoundInstancesMap::iterator itr = m_boundInstances[difficulty].find(mapId);
    if (itr != m_boundInstances[difficulty].end())
        return &itr->second;

    Difficulty altDifficulty;
    switch (difficulty)
    {
        case RAID_DIFFICULTY_10MAN_NORMAL: altDifficulty = RAID_DIFFICULTY_25MAN_NORMAL; break;
        case RAID_DIFFICULTY_25MAN_NORMAL: altDifficulty = RAID_DIFFICULTY_10MAN_NORMAL; break;
        case RAID_DIFFICULTY_10MAN_HEROIC: altDifficulty = RAID_DIFFICULTY_25MAN_HEROIC; break;
        case RAID_DIFFICULTY_25MAN_HEROIC: altDifficulty = RAID_DIFFICULTY_10MAN_HEROIC; break;
        default:
            return nullptr;
    }

    itr = m_boundInstances[altDifficulty].find(mapId);
    if (itr != m_boundInstances[altDifficulty].end())
        return &itr->second;

    return nullptr;
}

InstanceGroupBind* Group::BindToInstance(InstanceSave* save, bool permanent, bool load)
{
    if (!save || isBGGroup() || isBFGroup())
        return NULL;

    InstanceGroupBind& bind = m_boundInstances[save->GetDifficulty()][save->GetMapId()];
    bool newBind = false;
    if (!load && (!bind.save || permanent != bind.perm || save != bind.save))
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_GROUP_INSTANCE);

        stmt->setUInt32(0, m_dbStoreId);
        stmt->setUInt32(1, save->GetInstanceId());
        stmt->setBool(2, permanent);

        CharacterDatabase.Execute(stmt);

        newBind = true;
    }

    if (bind.save != save)
    {
        if (bind.save)
            bind.save->RemoveGroup(this);
        save->AddGroup(this);
    }

    if (!newBind && bind.perm != permanent)
        newBind = true;

    bind.save = save;
    bind.perm = permanent;
    if (!load)
        TC_LOG_DEBUG("maps", "Group::BindToInstance: Group (guid: %u, storage id: %u) is now bound to map %d, instance %d, difficulty %d",
        GetGUID().GetCounter(), m_dbStoreId, save->GetMapId(), save->GetInstanceId(), save->GetDifficulty());

    return &bind;
}

void Group::UnbindInstance(uint32 mapid, uint8 difficulty, bool unload)
{
    BoundInstancesMap::iterator itr = m_boundInstances[difficulty].find(mapid);
    if (itr != m_boundInstances[difficulty].end())
    {
        if (!unload)
        {
            CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GROUP_INSTANCE_BY_GUID);

            stmt->setUInt32(0, m_dbStoreId);
            stmt->setUInt32(1, itr->second.save->GetInstanceId());

            CharacterDatabase.Execute(stmt);
        }

        itr->second.save->RemoveGroup(this);                // save can become invalid
        m_boundInstances[difficulty].erase(itr);
    }
}

void Group::HomebindIfInstance(Player* player)
{
    if (player && !player->IsGameMaster() && sMapStore.LookupEntry(player->GetMapId())->IsDungeon())
        player->m_InstanceValid = false;
}

void Group::BroadcastGroupUpdate(void)
{
    // FG: HACK: force flags update on group leave - for values update hack
    // -- not very efficient but safe
    for (member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        Player* pp = ObjectAccessor::FindPlayer(citr->guid);
        if (pp && pp->IsInWorld())
        {
            pp->ForceValuesUpdateAtIndex(UNIT_FIELD_BYTES_2);
            pp->ForceValuesUpdateAtIndex(UNIT_FIELD_FACTION_TEMPLATE);
            TC_LOG_DEBUG("misc", "-- Forced group value update for '%s'", pp->GetName().c_str());
        }
    }
}

void Group::ResetMaxEnchantingLevel()
{
    m_maxEnchantingLevel = 0;
    m_enchantGuildPerkChance = 0;
    Player* member = NULL;
    for (member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        member = ObjectAccessor::FindPlayer(citr->guid);
        if (member && m_maxEnchantingLevel < member->GetSkillValue(SKILL_ENCHANTING))
        {
            m_maxEnchantingLevel = member->GetSkillValue(SKILL_ENCHANTING);
            uint32 guildPerkChance = member->GetTotalAuraModifier(SPELL_AURA_MOD_GATHERING_ITEMS_GAINED_PERCENT);
            if (m_enchantGuildPerkChance < guildPerkChance)
                m_enchantGuildPerkChance = guildPerkChance;
        }
    }
}

void Group::SetLootMethod(LootMethod method)
{
    m_lootMethod = method;
}

void Group::SetLooterGuid(ObjectGuid guid)
{
    m_looterGuid = guid;
}

void Group::SetMasterLooterGuid(ObjectGuid guid)
{
    m_looterGuid = guid;
}

void Group::SetLootThreshold(ItemQualities threshold)
{
    m_lootThreshold = threshold;
}

uint8 Group::GetLfgRoles(ObjectGuid guid)
{
    member_witerator slot = _getMemberWSlot(guid);
    if (slot == m_memberSlots.end())
        return 0;

    return slot->roles;
}

void Group::SetLfgRoles(ObjectGuid guid, const uint8 roles)
{
    member_witerator slot = _getMemberWSlot(guid);
    if (slot == m_memberSlots.end())
        return;

    slot->roles = roles;
    SendUpdate();
}

bool Group::IsFull() const
{
    return isRaidGroup() ? (m_memberSlots.size() >= MAXRAIDSIZE) : (m_memberSlots.size() >= MAXGROUPSIZE);
}

bool Group::isLFGGroup() const
{
    return m_groupType & GROUPTYPE_LFG;
}

bool Group::isRaidGroup() const
{
    return m_groupType & GROUPTYPE_RAID;
}

bool Group::isBGGroup() const
{
    return m_bgGroup != NULL;
}

bool Group::isBFGroup() const
{
    return m_bfGroup != NULL;
}

bool Group::IsCreated() const
{
    return GetMembersCount() > 0;
}

ObjectGuid Group::GetLeaderGUID() const
{
    return m_leaderGuid;
}

ObjectGuid Group::GetGUID() const
{
    return m_guid;
}

uint32 Group::GetLowGUID() const
{
    return m_guid.GetCounter();
}

char const* Group::GetLeaderName() const
{
    return m_leaderName.c_str();
}

LootMethod Group::GetLootMethod() const
{
    return m_lootMethod;
}

ObjectGuid Group::GetLooterGuid() const
{
    return m_looterGuid;
}

ItemQualities Group::GetLootThreshold() const
{
    return m_lootThreshold;
}

bool Group::IsMember(ObjectGuid guid) const
{
    return _getMemberCSlot(guid) != m_memberSlots.end();
}

bool Group::IsLeader(ObjectGuid guid) const
{
    return (GetLeaderGUID() == guid);
}

ObjectGuid Group::GetMemberGUID(const std::string& name) const
{
    for (member_citerator itr = m_memberSlots.begin(); itr != m_memberSlots.end(); ++itr)
        if (itr->name == name)
            return itr->guid;
    return ObjectGuid::Empty;
}

bool Group::IsAssistant(ObjectGuid guid) const
{
    member_citerator mslot = _getMemberCSlot(guid);
    if (mslot == m_memberSlots.end())
        return false;
    return mslot->flags & MEMBER_FLAG_ASSISTANT;
}

bool Group::SameSubGroup(uint64 guid1, uint64 guid2) const
{
    member_citerator mslot2 = _getMemberCSlot(guid2);
    if (mslot2 == m_memberSlots.end())
       return false;
    return SameSubGroup(guid1, &*mslot2);
}

bool Group::SameSubGroup(uint64 guid1, MemberSlot const* slot2) const
{
    member_citerator mslot1 = _getMemberCSlot(guid1);
    if (mslot1 == m_memberSlots.end() || !slot2)
        return false;
    return (mslot1->group == slot2->group);
}

bool Group::HasFreeSlotSubGroup(uint8 subgroup) const
{
    return (m_subGroupsCounts && m_subGroupsCounts[subgroup] < MAXGROUPSIZE);
}

bool Group::HasRaidMarker(ObjectGuid objectGuid)
{
    for (RaidMarkerList::iterator itr = m_raidMarkers.begin(); itr != m_raidMarkers.end(); itr++)
        if (objectGuid == (*itr).guid)
            return true;

    return false;
}

bool Group::HasRaidMarker(uint8 markerId)
{
    uint32 mask = RAID_MARKER_NONE;

    switch (markerId)
    {
        case 0: mask = RAID_MARKER_BLUE;   break; // Blue
        case 1: mask = RAID_MARKER_GREEN;  break; // Green
        case 2: mask = RAID_MARKER_PURPLE; break; // Purple
        case 3: mask = RAID_MARKER_RED;    break; // Red
        case 4: mask = RAID_MARKER_YELLOW; break; // Yellow
        default: break;
    }

    for (RaidMarkerList::iterator itr = m_raidMarkers.begin(); itr != m_raidMarkers.end(); itr++)
        if (mask == (*itr).mask)
            return true;

    return false;
}

uint8 Group::GetRaidMarkerByGuid(ObjectGuid objectGuid)
{
    uint8 markerId = 0;
    uint32 mask = RAID_MARKER_NONE;

    for (RaidMarkerList::iterator itr = m_raidMarkers.begin(); itr != m_raidMarkers.end(); itr++)
    {
        if (objectGuid == (*itr).guid)
        {
            mask = (*itr).mask;
            break;
        }
    }

    switch (mask)
    {
        case RAID_MARKER_BLUE:   markerId = 0; break; // Blue
        case RAID_MARKER_GREEN:  markerId = 1; break; // Green
        case RAID_MARKER_PURPLE: markerId = 2; break; // Purple
        case RAID_MARKER_RED:    markerId = 3; break; // Red
        case RAID_MARKER_YELLOW: markerId = 4; break; // Yellow
        default: break;
    }

    return markerId;
}

Group::RaidMarker Group::GetRaidMarkerById(uint8 markerId)
{
    uint32 mask = RAID_MARKER_NONE;

    switch (markerId)
    {
        case 0: mask = RAID_MARKER_BLUE;   break; // Blue
        case 1: mask = RAID_MARKER_GREEN;  break; // Green
        case 2: mask = RAID_MARKER_PURPLE; break; // Purple
        case 3: mask = RAID_MARKER_RED;    break; // Red
        case 4: mask = RAID_MARKER_YELLOW; break; // Yellow
        default: break;
    }

    for (RaidMarkerList::iterator itr = m_raidMarkers.begin(); itr != m_raidMarkers.end(); itr++)
    {
        if (mask == (*itr).mask)
        {
            return (*itr);
            break;
        }
    }

    // Build a "default" one to return. Should never happen.
    RaidMarker marker;

    marker.mapId   = 0;
    marker.posX    = 0.0f;
    marker.posY    = 0.0f;
    marker.posZ    = 0.0f;
    marker.mask    = 0;
    marker.guid    = ObjectGuid::Empty;
    marker.spellId = 0;

    return marker;
}

void Group::SendRaidMarkersUpdate()
{
    uint32 mask = RAID_MARKER_NONE;

    if (!GetRaidMarkers().empty())
        for (auto itr : GetRaidMarkers())
            mask |= itr.mask;

    ByteBuffer dataBuffer;
    WorldPacket data(SMSG_RAID_MARKERS_CHANGED);

    data << uint8(0);
    data << uint32(mask);

    data.WriteBits(!GetRaidMarkers().empty() ? GetRaidMarkers().size() : 0, 3);

    if (!GetRaidMarkers().empty())
    {
        for (uint8 i = 0; i < MAX_RAID_MARKERS; i++)
        {
            if (HasRaidMarker(i))
            {
                RaidMarker marker = GetRaidMarkerById(i);

                ObjectGuid guid = marker.guid;

                data.WriteBit(guid[6]);
                data.WriteBit(guid[2]);
                data.WriteBit(guid[7]);
                data.WriteBit(guid[1]);
                data.WriteBit(guid[4]);
                data.WriteBit(guid[3]);
                data.WriteBit(guid[5]);
                data.WriteBit(guid[0]);

                dataBuffer.WriteByteSeq(guid[6]);
                dataBuffer << float(marker.posY);
                dataBuffer.WriteByteSeq(guid[2]);
                dataBuffer << float(marker.posX);
                dataBuffer.WriteByteSeq(guid[7]);
                dataBuffer.WriteByteSeq(guid[5]);
                dataBuffer.WriteByteSeq(guid[0]);
                dataBuffer.WriteByteSeq(guid[4]);
                dataBuffer << float(marker.posZ);
                dataBuffer.WriteByteSeq(guid[3]);
                dataBuffer.WriteByteSeq(guid[1]);
                dataBuffer << uint32(marker.mapId);
            }
        }
    }

    data.FlushBits();

    if (!dataBuffer.empty())
        data.append(dataBuffer);

    BroadcastPacket(&data, true);
}

void Group::AddRaidMarker(ObjectGuid objectGuid, uint32 spellId, uint32 mapId, float x, float y, float z)
{
    uint32 mask = RAID_MARKER_NONE;

    switch (spellId)
    {
        case 84996: mask = RAID_MARKER_BLUE;   break; // Raid Marker 1
        case 84997: mask = RAID_MARKER_GREEN;  break; // Raid Marker 2
        case 84998: mask = RAID_MARKER_PURPLE; break; // Raid Marker 3
        case 84999: mask = RAID_MARKER_RED;    break; // Raid Marker 4
        case 85000: mask = RAID_MARKER_YELLOW; break; // Raid Marker 5
        default: break;
    }

    RaidMarker marker;

    marker.mapId = mapId;
    marker.posX = x;
    marker.posY = y;
    marker.posZ = z;
    marker.mask = mask;
    marker.guid = objectGuid;
    marker.spellId = spellId;

    m_raidMarkers.push_back(marker);
    SendRaidMarkersUpdate();
}

void Group::RemoveRaidMarker(uint8 markerId)
{
    uint32 mask = RAID_MARKER_NONE;

    switch (markerId)
    {
        case 0: mask = RAID_MARKER_BLUE;   break; // Blue
        case 1: mask = RAID_MARKER_GREEN;  break; // Green
        case 2: mask = RAID_MARKER_PURPLE; break; // Purple
        case 3: mask = RAID_MARKER_RED;    break; // Red
        case 4: mask = RAID_MARKER_YELLOW; break; // Yellow
        default: break;
    }

    for (RaidMarkerList::iterator itr = m_raidMarkers.begin(); itr != m_raidMarkers.end();)
    {
        if (mask == (*itr).mask)
        {
            for (GroupReference* gr = GetFirstMember(); gr != NULL; gr = gr->next())
            {
                Player *player = gr->GetSource();
                if (!player->GetSession())
                    continue;

                if (DynamicObject* dynObject = ObjectAccessor::GetDynamicObject(*player, (*itr).guid))
                    if (dynObject->GetEntry() == (*itr).spellId)
                    {
                        dynObject->Remove();
                        break;
                    }
            }

            m_raidMarkers.erase(itr++);
        }
        else ++itr;
    }

    SendRaidMarkersUpdate();
}

void Group::RemoveAllRaidMarkers()
{
    for (RaidMarkerList::iterator itr = m_raidMarkers.begin(); itr != m_raidMarkers.end(); ++itr)
        for (GroupReference* gr = GetFirstMember(); gr != NULL; gr = gr->next())
        {
            Player *player = gr->GetSource();
            if (!player->GetSession())
                continue;

            if (DynamicObject* dynObject = ObjectAccessor::GetDynamicObject(*player, (*itr).guid))
                if (dynObject->GetEntry() == (*itr).spellId)
                {
                    dynObject->Remove();
                    break;
                }
        }

    m_raidMarkers.clear();
    SendRaidMarkersUpdate();
}

uint8 Group::GetMemberGroup(ObjectGuid guid) const
{
    member_citerator mslot = _getMemberCSlot(guid);
    if (mslot == m_memberSlots.end())
       return (MAX_RAID_SUBGROUPS+1);
    return mslot->group;
}

void Group::SetBattlegroundGroup(Battleground* bg)
{
    m_bgGroup = bg;
}

void Group::SetBattlefieldGroup(Battlefield *bg)
{
    m_bfGroup = bg;
}

void Group::SetGroupMemberFlag(ObjectGuid guid, bool apply, GroupMemberFlags flag)
{
    // Assistants, main assistants and main tanks are only available in raid groups
    if (!isRaidGroup())
       return;

    // Check if player is really in the raid
    member_witerator slot = _getMemberWSlot(guid);
    if (slot == m_memberSlots.end())
        return;

    // Do flag specific actions, e.g ensure uniqueness
    switch (flag)
    {
        case MEMBER_FLAG_MAINASSIST:
            RemoveUniqueGroupMemberFlag(MEMBER_FLAG_MAINASSIST);         // Remove main assist flag from current if any.
            break;
        case MEMBER_FLAG_MAINTANK:
            RemoveUniqueGroupMemberFlag(MEMBER_FLAG_MAINTANK);           // Remove main tank flag from current if any.
            break;
        case MEMBER_FLAG_ASSISTANT:
            break;
        default:
            return;                                                      // This should never happen
    }

    // Switch the actual flag
    ToggleGroupMemberFlag(slot, flag, apply);

    // Preserve the new setting in the db
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GROUP_MEMBER_FLAG);

    stmt->setUInt8(0, slot->flags);
    stmt->setUInt32(1, guid.GetCounter());

    CharacterDatabase.Execute(stmt);

    // Broadcast the changes to the group
    SendUpdate();
}

Difficulty Group::GetDifficulty(bool isRaid) const
{
    return isRaid ? m_raidDifficulty : m_dungeonDifficulty;
}

Difficulty Group::GetDungeonDifficulty() const
{
    return m_dungeonDifficulty;
}

Difficulty Group::GetRaidDifficulty() const
{
    return m_raidDifficulty;
}

bool Group::isRollLootActive() const
{
    return !RollId.empty();
}

Group::Rolls::iterator Group::GetRoll(ObjectGuid lootGuid, uint8 slot)
{
    Rolls::iterator iter;
    for (iter=RollId.begin(); iter != RollId.end(); ++iter)
        if ((*iter)->lootGUID == lootGuid && (*iter)->itemSlot == slot && (*iter)->isValid())
            return iter;
    return RollId.end();
}

void Group::LinkMember(GroupReference* pRef)
{
    m_memberMgr.insertFirst(pRef);
}

void Group::DelinkMember(uint64 guid)
{
    GroupReference* ref = m_memberMgr.getFirst();
    while (ref)
    {
        GroupReference* nextRef = ref->next();
        if (ref->GetSource()->GetGUID() == guid)
        {
            ref->unlink();
            break;
        }
        ref = nextRef;
    }
}

Group::BoundInstancesMap& Group::GetBoundInstances(Difficulty difficulty)
{
    return m_boundInstances[difficulty];
}

void Group::_initRaidSubGroupsCounter()
{
    // Sub group counters initialization
    if (!m_subGroupsCounts)
        m_subGroupsCounts = new uint8[MAX_RAID_SUBGROUPS];

    memset((void*)m_subGroupsCounts, 0, (MAX_RAID_SUBGROUPS)*sizeof(uint8));

    for (member_citerator itr = m_memberSlots.begin(); itr != m_memberSlots.end(); ++itr)
        ++m_subGroupsCounts[itr->group];
}

Group::member_citerator Group::_getMemberCSlot(uint64 Guid) const
{
    for (member_citerator itr = m_memberSlots.begin(); itr != m_memberSlots.end(); ++itr)
        if (itr->guid == Guid)
            return itr;
    return m_memberSlots.end();
}

Group::member_witerator Group::_getMemberWSlot(uint64 Guid)
{
    for (member_witerator itr = m_memberSlots.begin(); itr != m_memberSlots.end(); ++itr)
        if (itr->guid == Guid)
            return itr;
    return m_memberSlots.end();
}

void Group::SubGroupCounterIncrease(uint8 subgroup)
{
    if (m_subGroupsCounts)
        ++m_subGroupsCounts[subgroup];
}

void Group::SubGroupCounterDecrease(uint8 subgroup)
{
    if (m_subGroupsCounts)
        --m_subGroupsCounts[subgroup];
}

void Group::RemoveUniqueGroupMemberFlag(GroupMemberFlags flag)
{
    for (member_witerator itr = m_memberSlots.begin(); itr != m_memberSlots.end(); ++itr)
        if (itr->flags & flag)
            itr->flags &= ~flag;
}

void Group::ToggleGroupMemberFlag(member_witerator slot, uint8 flag, bool apply)
{
    if (apply)
        slot->flags |= flag;
    else
        slot->flags &= ~flag;
}

bool Group::IsGuildGroup(uint32 guildId, Player* refPlayer, float* outXpMultiplier, uint32* outCurrentCount, uint32* outRequiredCount) const
{
    if (!guildId)
        return false;
    if (!refPlayer || !refPlayer->FindMap() || !refPlayer->GetGuildId())
        return false;

    uint32 mapId = refPlayer->GetMap()->GetId();
    uint32 InstanceId = refPlayer->GetMap()->GetInstanceId();
    uint32 count = 0;
    uint32 reqCount = 0;
    float xpMul = 1.0f;
    // First we populate the array
    for (auto&& player : *this)
    {
        WorldLocation corpseLocation = player->GetCorpseLocation();
        if (player->GetGuildId() == guildId && // If player is in the same guild
            ((player->FindMap() && player->GetMap()->GetId() == mapId && player->GetMap()->GetInstanceId() == InstanceId) || // If player is in the same map
            (player->HasCorpse() && corpseLocation.GetMapId() == mapId && player->GetCorpse()->GetMap()->GetInstanceId() == InstanceId))) // Or their corpse is in the same map
            ++count;
    }

    bool ret = false;

    if (refPlayer->GetMap()->IsNonRaidDungeon())
    {
        reqCount = 3;
        if (count >= reqCount)
        {
            if (count == 3)
                xpMul = 0.5f;
            else if (count == 4)
                xpMul = 1.0f;
            else if (count == 5)
                xpMul = 1.25f;
            if (refPlayer->GetMap()->IsHeroic())
                xpMul *= 1.5f;
        }
    }
    else if (refPlayer->GetMap()->IsRaid())
    {
        if (refPlayer->GetMap()->GetEntry()->maxPlayers == 40)
            reqCount = 10;
        else
        {
            switch (refPlayer->GetMap()->GetDifficulty())
            {
                case RAID_DIFFICULTY_10MAN_NORMAL:
                case RAID_DIFFICULTY_10MAN_HEROIC:
                    reqCount = 8;
                    break;
                case RAID_DIFFICULTY_25MAN_NORMAL:
                case RAID_DIFFICULTY_25MAN_HEROIC:
                    reqCount = 20;
                    break;
            }
        }
    }
    else if (refPlayer->GetMap()->IsBattleArena())
        reqCount = GetMembersCount();
    else if (refPlayer->GetMap()->IsBattleground())
    {
        if (Battleground* bg = refPlayer->GetBattleground())
            reqCount = uint32(bg->GetMaxPlayers() * 0.8f);
    }
    else if (refPlayer->GetMap()->IsScenario())
        reqCount = 3;

    if (outXpMultiplier)
        *outXpMultiplier = xpMul;
    if (outCurrentCount)
        *outCurrentCount = count;
    if (outRequiredCount)
        *outRequiredCount = reqCount;
    return reqCount && count >= reqCount;
}

void Group::UpdateGuildAchievementCriteria(AchievementCriteriaTypes type, uint32 miscValue1, uint32 miscValue2, uint32 miscValue3, Unit* unit, WorldObject* rewardSource)
{
    Player* player;
    if (Guild* guild = GetGroupGuild(rewardSource, &player))
        guild->UpdateAchievementCriteria(type, miscValue1, miscValue2, miscValue3, unit, player);
}

Guild* Group::GetGroupGuild(WorldObject* rewardSource, Player** participant) const
{
    std::set<uint32> guilds;
    for (auto&& member : *this)
    {
        if (!member->CanRollForLootIn(rewardSource))
            continue;

        if (member->GetGuildId())
        {
            if (guilds.find(member->GetGuildId()) == guilds.end())
            {
                guilds.insert(member->GetGuildId());
                if (IsGuildGroup(member->GetGuildId(), member))
                {
                    Guild* guild = member->GetGuild();
                    ASSERT(guild);
                    if (participant)
                        *participant = member;
                    return guild;
                }
            }
        }
    }
    return nullptr;
}

void Group::SetMemberRole(ObjectGuid guid, uint32 role)
{
    member_witerator itr = _getMemberWSlot(guid);
    if (itr == m_memberSlots.end())
        return;

    itr->roles = role;

    CharacterDatabasePreparedStatement* prepStatement = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GROUP_ROLE);
    prepStatement->setUInt8(0, role);
    prepStatement->setUInt32(1, guid.GetCounter());

    CharacterDatabase.Execute(prepStatement);
}

uint32 Group::GetMemberRole(ObjectGuid guid) const
{
    member_citerator itr = _getMemberCSlot(guid);
    if (itr == m_memberSlots.end())
        return 0;

    return itr->roles;
}

void Group::ReadyCheckMemberHasResponded(uint64 guid)
{
    member_witerator itr = _getMemberWSlot(guid);
    if (itr == m_memberSlots.end())
        return;

    itr->readyCheckHasResponded = true;
}

void Group::ReadyCheckResetResponded()
{
    for (member_witerator itr = m_memberSlots.begin(); itr != m_memberSlots.end(); itr++)
        itr->readyCheckHasResponded = false;
}

bool Group::ReadyCheckAllResponded() const
{
    for (member_citerator itr = m_memberSlots.begin(); itr != m_memberSlots.end(); itr++)
        if (!itr->readyCheckHasResponded)
            return false;

    return true;
}

void Group::AddLootLockout(WorldObject* lootedObject, bool pending /*= true*/)
{
    if (!lootedObject)
        return;

    for (GroupReference *itr = GetFirstMember(); itr != NULL; itr = itr->next())
        if (Player* player = itr->GetSource())
            if (player->IsInWorld() && player->CanRollForLootIn(lootedObject))
                player->AddLootLockout(LootLockoutType::PersonalLoot, lootedObject->GetEntry(), player->GetMap()->GetDifficulty(), pending);
}

void Group::FindNewLeader(ObjectGuid exceptGuid)
{
    for (member_witerator itr = m_memberSlots.begin(); itr != m_memberSlots.end(); ++itr)
    {
        if (exceptGuid && itr->guid == exceptGuid)
            continue;
        if (ObjectAccessor::FindPlayer(itr->guid))
        {
            ChangeLeader(itr->guid);
            return;
        }
    }
}

void Group::ResumeLoggingIfNeeded()
{
    if (m_logResumeOnLogin)
    {
        m_logResumeOnLogin = false;
    }
}

char const* Group::GetPlayerName(ObjectGuid guid) const
{
    if (!guid)
        return "<nobody>";

    for (auto&& member : m_memberSlots)
        if (member.guid == guid)
            return member.name.c_str();

    if (Player* player = ObjectAccessor::FindConnectedPlayer(guid))
        return player->GetName().c_str();

    return "<offline>";
}

std::string Group::FormatPlayer(ObjectGuid guid)
{
    return ::Format("%s(%u)", GetPlayerName(guid), guid.GetCounter());
}

std::string Group::FormatLeader()
{
    return ::Format("%s(%u)", GetLeaderName(), GetLeaderGUID().GetCounter());
}

char const* Group::GetMapName(uint32 id)
{
    if (MapEntry const* map = sMapStore.LookupEntry(id))
        return map->name;

    return "<unknown>";
}

char const* Group::GetItemName(uint32 id)
{
    if (ItemLocale const* itemLocale = sObjectMgr->GetItemLocale(id))
    {
        StringVector const& data = itemLocale->Name;
        std::string name;
        for (uint8 locale = 0; locale < TOTAL_LOCALES && name.empty(); ++locale)
            if (data.size() > size_t(locale) && !data[locale].empty())
                return data[locale].c_str();
    }

    if (ItemTemplate const* item = sObjectMgr->GetItemTemplate(id))
        return item->Name1.c_str();

    return "<unknown>";
}

char const* Group::Format(Difficulty difficulty)
{
    switch (difficulty)
    {
        case REGULAR_DIFFICULTY:            return "Regular";
        case DUNGEON_DIFFICULTY_NORMAL:     return "Normal";
        case DUNGEON_DIFFICULTY_HEROIC:     return "Heroic";
        case RAID_DIFFICULTY_10MAN_NORMAL:  return "10 Normal";
        case RAID_DIFFICULTY_25MAN_NORMAL:  return "25 Normal";
        case RAID_DIFFICULTY_10MAN_HEROIC:  return "10 Heroic";
        case RAID_DIFFICULTY_25MAN_HEROIC:  return "25 Heroic";
        case RAID_DIFFICULTY_25MAN_LFR:     return "LFR";
        case RAID_DIFFICULTY_1025MAN_FLEX:  return "Flex";
        default: return "???";
    }
}

char const* Group::Format(LootMethod method)
{
    switch (method)
    {
        case FREE_FOR_ALL:      return "FFA";
        case ROUND_ROBIN:       return "Round Robin";
        case MASTER_LOOT:       return "Master Loot";
        case GROUP_LOOT:        return "Group Loot";
        case NEED_BEFORE_GREED: return "Need Before Greed";
        default: return "???";
    }
}

char const* Group::Format(ItemQualities quality)
{
    switch (quality)
    {
        case ITEM_QUALITY_POOR:         return "Poor";
        case ITEM_QUALITY_NORMAL:       return "Normal";
        case ITEM_QUALITY_UNCOMMON:     return "Uncommon";
        case ITEM_QUALITY_RARE:         return "Rare";
        case ITEM_QUALITY_EPIC:         return "Epic";
        case ITEM_QUALITY_LEGENDARY:    return "Legendary";
        case ITEM_QUALITY_ARTIFACT:     return "Artifact";
        case ITEM_QUALITY_HEIRLOOM:     return "Heirloom";
        default: return "???";
    }
}

char const* Group::Format(RollVote vote)
{
    switch (vote)
    {
        case PASS:              return "PASS";
        case NEED:              return "NEED";
        case GREED:             return "GREED";
        case DISENCHANT:        return "DISENCHANT";
        case NOT_EMITED_YET:    return "MISSED";
        case NOT_VALID:         return "INVALID";
        default: return "???";
    }
}

std::string Group::Format(Roll const* roll)
{
    return ::Format("%s[%u]x%u", GetItemName(roll->itemid), roll->itemid, roll->itemCount);
}

std::string Group::Format(Item const* item)
{
    return ::Format("%s[%u](%u)x%u", GetItemName(item->GetEntry()), item->GetEntry(), item->GetGUID().GetCounter(), item->GetCount());
}

std::string Group::Format(Player const* player)
{
    return ::Format("%s(%u)", player->GetName().c_str(), player->GetGUID().GetCounter());
}

std::string Group::FormatMoney(uint32 money)
{
    std::stringstream ss;
    uint32 g = (money / GOLD);
    uint32 s = (money % GOLD) / SILVER;
    uint32 c = (money % GOLD) % SILVER;
    if (g)
    {
        ss << g << "g";
        ss.fill('0');
        ss.width(2);
        ss << s << "s";
        ss.fill('0');
        ss.width(2);
        ss << c << "c";
    }
    else if (s)
    {
        ss << s << "s";
        ss.fill('0');
        ss.width(2);
        ss << c << "c";
    }
    else
        ss << c << "c";

    return ss.str();
}

std::string Group::GetPlayerTalentString(Player const* player)
{
    uint32 active = player->GetActiveSpec();
    uint32 secondary = active ? 0 : 1;
    std::string activeStr, secondaryStr;

    if (auto entry = sChrSpecializationStore.LookupEntry(player->GetTalentSpecialization(active)))
        activeStr = entry->Name[0];
    else
        activeStr = std::to_string(active);

    if (auto entry = sChrSpecializationStore.LookupEntry(player->GetTalentSpecialization(secondary)))
        secondaryStr = entry->Name[0];
    else
        secondaryStr = std::to_string(secondary);

    return ::Format("%s (secondary: %s)", activeStr.c_str(), secondaryStr.c_str());
}
