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
#include "WorldPacket.h"
#include "WorldSession.h"
#include "World.h"
#include "ObjectMgr.h"
#include "GuildMgr.h"
#include "Log.h"
#include "Opcodes.h"
#include "Guild.h"
#include "GossipDef.h"
#include "SocialMgr.h"
#include "Group.h"

 // Cleanup bad characters
void cleanStr(std::string &str) {
    str.erase(remove(str.begin(), str.end(), '|'), str.end());
}


void WorldSession::HandleGuildQueryOpcode(WorldPackets::Guild::QueryGuildInfo& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_QUERY [{}]: Guild: {} Target: {}",
        GetPlayerInfo().c_str(), packet.GuildGuid.GetCounter(), packet.PlayerGuid.GetCounter());

    if (Guild* guild = sGuildMgr->GetGuildByGuid(packet.GuildGuid))
        if (guild->IsMember(packet.PlayerGuid))
            guild->HandleQuery(this);
}

void WorldSession::HandleGuildInviteOpcode(WorldPackets::Guild::GuildInviteByName& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_INVITE [{}]: Invited: {}", GetPlayerInfo().c_str(), packet.InvitedName.c_str());
    if (normalizePlayerName(packet.InvitedName))
        if (Guild* guild = GetPlayer()->GetGuild())
            guild->HandleInviteMember(this, packet.InvitedName);
}

void WorldSession::HandleGuildRemoveOpcode(WorldPackets::Guild::GuildOfficerRemoveMember& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_REMOVE [{}]: Target: {}", GetPlayerInfo().c_str(), packet.PlayerGuid.GetCounter());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleRemoveMember(this, packet.PlayerGuid);
}

void WorldSession::HandleGuildAcceptOpcode(WorldPackets::Guild::AcceptGuildInvite& /*packet*/)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_ACCEPT [{}]", GetPlayerInfo().c_str());

    if (!GetPlayer()->GetGuildId())
        if (Guild* guild = sGuildMgr->GetGuildById(GetPlayer()->GetGuildIdInvited()))
            guild->HandleAcceptMember(this);
}

void WorldSession::HandleGuildDeclineOpcode(WorldPackets::Guild::GuildDeclineInvitation& /*packet*/)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_DECLINE [{}]", GetPlayerInfo().c_str());

    GetPlayer()->SetGuildIdInvited(0);
    GetPlayer()->SetInGuild(0);

    if (GetPlayer()->GetLastGuildInviterGUID())
    {
        if (Player* inviter = ObjectAccessor::FindPlayer(GetPlayer()->GetLastGuildInviterGUID()))
            inviter->SendDeclineGuildInvitation(GetPlayer()->GetName());

        GetPlayer()->SetLastGuildInviterGUID(ObjectGuid::Empty);
    }
}

void WorldSession::HandleGuildRosterOpcode(WorldPackets::Guild::GuildGetRoster& /*packet*/)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_ROSTER [{}]", GetPlayerInfo().c_str());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleRoster(this);
    else
        Guild::SendCommandResult(this, GUILD_COMMAND_ROSTER, ERR_GUILD_PLAYER_NOT_IN_GUILD);
}

void WorldSession::HandleGuildPromoteOpcode(WorldPackets::Guild::GuildPromoteMember& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_PROMOTE [{}]: Target: {}", GetPlayerInfo().c_str(), packet.TargetGuid.GetCounter());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleUpdateMemberRank(this, packet.TargetGuid, false);
}

void WorldSession::HandleGuildDemoteOpcode(WorldPackets::Guild::GuildDemoteMember& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_DEMOTE [{}]: Target: {}", GetPlayerInfo().c_str(), packet.TargetGuid.GetCounter());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleUpdateMemberRank(this, packet.TargetGuid, true);
}

void WorldSession::HandleGuildAssignRankOpcode(WorldPackets::Guild::GuildAssignMemberRank& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_ASSIGN_MEMBER_RANK [{}]: Target: {} Rank: {}, Issuer: {}",
        GetPlayerInfo().c_str(), packet.TargetGuid.GetCounter(), packet.RankIndex, _player->GetGUID().GetCounter());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleSetMemberRank(this, packet.TargetGuid, _player->GetGUID(), packet.RankIndex);
}

void WorldSession::HandleGuildLeaveOpcode(WorldPackets::Guild::GuildLeave& /*packet*/)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_LEAVE [{}]", GetPlayerInfo().c_str());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleLeaveMember(this);
}

void WorldSession::HandleGuildDisbandOpcode(WorldPackets::Guild::GuildDelete& /*packet*/)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_DISBAND [{}]", GetPlayerInfo().c_str());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleDisband(this);
}

void WorldSession::HandleGuildMOTDOpcode(WorldPackets::Guild::GuildUpdateMotdText& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_MOTD [{}]: MOTD: {}", GetPlayerInfo().c_str(), packet.Motd.c_str());

    // Check for overflow
    if (packet.Motd.length() > 128)
        return;

    // Cleanup bad characters
    cleanStr(packet.Motd);

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleSetMOTD(this, packet.Motd);
}

void WorldSession::HandleGuildSetNoteOpcode(WorldPackets::Guild::GuildSetMemberNote& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_SET_NOTE [{}]: Target: {}, Note: {}, Public: {}",
        GetPlayerInfo().c_str(), packet.PlayerGuid.GetCounter(), packet.Note.c_str(), packet.IsPublic);

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleSetMemberNote(this, packet.Note, packet.PlayerGuid, packet.IsPublic);
}

void WorldSession::HandleGuildQueryRanksOpcode(WorldPackets::Guild::GuildGetRanks& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_QUERY_RANKS [{}]: Guild: {}",
        GetPlayerInfo().c_str(), packet.GuildGuid.GetCounter());

    if (Guild* guild = sGuildMgr->GetGuildByGuid(packet.GuildGuid))
        if (guild->IsMember(_player->GetGUID()))
            guild->SendGuildRankInfo(this);
}

void WorldSession::HandleGuildAddRankOpcode(WorldPackets::Guild::GuildAddRank& packet)
{
    // Check for overflow
    if (packet.RankName.length() > 15)
        return;

    // Cleanup bad characters
    cleanStr(packet.RankName);

    TC_LOG_DEBUG("guild", "CMSG_GUILD_ADD_RANK [{}]: Rank: {}", GetPlayerInfo().c_str(), packet.RankName.c_str());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleAddNewRank(this, packet.RankName);
}

void WorldSession::HandleGuildDelRankOpcode(WorldPackets::Guild::GuildDeleteRank& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_DEL_RANK [{}]: Rank: {}", GetPlayerInfo().c_str(), packet.RankIndex);

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleRemoveRank(this, packet.RankIndex);
}

void WorldSession::HandleGuildSwitchRank(WorldPackets::Guild::GuildShiftRank& packet)
{
    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleSwitchRank(this, packet.RankIndex, packet.Up);
}

void WorldSession::HandleGuildChangeInfoTextOpcode(WorldPackets::Guild::GuildUpdateInfoText& packet)
{
    // Check for overflow
    if (packet.Info.length() > 500)
        return;

    // Cleanup bad characters
    cleanStr(packet.Info);

    TC_LOG_DEBUG("guild", "CMSG_GUILD_INFO_TEXT [{}]: {}", GetPlayerInfo().c_str(), packet.Info.c_str());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleSetInfo(this, packet.Info);
}

void WorldSession::HandleSaveGuildEmblemOpcode(WorldPacket& recvPacket)
{
    EmblemInfo emblemInfo;
    emblemInfo.ReadPacket(recvPacket);

    ObjectGuid vendorGuid;
    recvPacket.ReadBit(); // vendor guid bit 0
    recvPacket.ReadBit(); // vendor guid bit 1
    recvPacket.ReadBit(); // vendor guid bit 2
    recvPacket.ReadBit(); // vendor guid bit 3
    recvPacket.ReadBit(); // vendor guid bit 4
    recvPacket.ReadBit(); // vendor guid bit 5
    recvPacket.ReadBit(); // vendor guid bit 6
    recvPacket.ReadBit(); // vendor guid bit 7
    recvPacket.ReadByteSeq(vendorGuid[6]);
    recvPacket.ReadByteSeq(vendorGuid[2]);
    recvPacket.ReadByteSeq(vendorGuid[7]);
    recvPacket.ReadByteSeq(vendorGuid[5]);
    recvPacket.ReadByteSeq(vendorGuid[0]);
    recvPacket.ReadByteSeq(vendorGuid[4]);
    recvPacket.ReadByteSeq(vendorGuid[1]);
    recvPacket.ReadByteSeq(vendorGuid[3]);

    TC_LOG_DEBUG("guild", "CMSG_SAVE_GUILD_EMBLEM [{}]: Guid: [" "{}" "] Style: {}, Color: {}, BorderStyle: {}, BorderColor: {}, BackgroundColor: {}",
        GetPlayerInfo().c_str(), uint64(vendorGuid), emblemInfo.GetStyle(), emblemInfo.GetColor(), emblemInfo.GetBorderStyle(),
        emblemInfo.GetBorderColor(), emblemInfo.GetBackgroundColor());

    if (GetPlayer()->GetNPCIfCanInteractWith(vendorGuid, UNIT_NPC_FLAG_TABARDDESIGNER))
    {
        // Remove fake death
        if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
            GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

        if (Guild* guild = GetPlayer()->GetGuild())
            guild->HandleSetEmblem(this, emblemInfo);
        else
            Guild::SendSaveEmblemResult(this, ERR_GUILDEMBLEM_NOGUILD); // "You are not part of a guild!";
    }
    else
        Guild::SendSaveEmblemResult(this, ERR_GUILDEMBLEM_INVALIDVENDOR); // "That's not an emblem vendor!"
}

void WorldSession::HandleGuildEventLogQueryOpcode(WorldPackets::Guild::GuildEventLogQuery& /*packet*/)
{
    TC_LOG_DEBUG("guild", "MSG_GUILD_EVENT_LOG_QUERY [{}]", GetPlayerInfo().c_str());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->SendEventLog(this);
}

void WorldSession::HandleGuildBankMoneyWithdrawn(WorldPackets::Guild::GuildBankRemainingWithdrawMoneyQuery& /*packet*/)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_BANK_MONEY_WITHDRAWN [{}]", GetPlayerInfo().c_str());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->SendMoneyInfo(this);
}

void WorldSession::HandleGuildPermissions(WorldPackets::Guild::GuildPermissionsQuery& /*packet*/)
{
    // Null Packet
    TC_LOG_DEBUG("guild", "CMSG_GUILD_PERMISSIONS [{}]", GetPlayerInfo().c_str());

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->SendPermissions(this);
}

// Called when clicking on Guild bank gameobject
void WorldSession::HandleGuildBankerActivate(WorldPackets::Guild::GuildBankActivate& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_BANKER_ACTIVATE [{}]: Go: [" "{}" "] AllSlots: {}", GetPlayerInfo().c_str(), (uint64)packet.Guid, packet.SendAllSlots);

    auto go = GetPlayer()->GetGameObjectIfCanInteractWith(packet.Guid, GAMEOBJECT_TYPE_GUILD_BANK);
    if (!go)
        return;

    Guild* ownerGuild = nullptr;
    if (go->GetOwner())
        if (Player* player = go->GetOwner()->ToPlayer())
            ownerGuild = player->GetGuild();

    Guild* const guild = GetPlayer()->GetGuild();
    if (ownerGuild && ownerGuild != guild) // player must be in same guild as summoner
    {
        Guild::SendCommandResult(this, GUILD_COMMAND_VIEW_TAB, ERR_GUILD_PERMISSIONS);
        return;
    }

    if (!guild)
    {
        Guild::SendCommandResult(this, GUILD_COMMAND_VIEW_TAB, ERR_GUILD_PLAYER_NOT_IN_GUILD);
        return;
    }

    guild->SendBankList(this, 0, true, true);
}

// Called when opening guild bank tab only (first one)
void WorldSession::HandleGuildBankQueryTab(WorldPackets::Guild::GuildBankQueryTab& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_BANK_QUERY_TAB [{}]: Go: [" "{}" "], TabId: {}, AllSlots: {}", GetPlayerInfo().c_str(), (uint64)packet.Guid, packet.TabId, packet.SendAllSlots);

    if (GetPlayer()->GetGameObjectIfCanInteractWith(packet.Guid, GAMEOBJECT_TYPE_GUILD_BANK))
        if (Guild* guild = GetPlayer()->GetGuild())
            guild->SendBankList(this, packet.TabId, true, false);
}

void WorldSession::HandleGuildBankDepositMoney(WorldPackets::Guild::GuildBankDepositMoney& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_BANK_DEPOSIT_MONEY [{}]: Go: [" "{}" "], money: " UI64FMTD,
        GetPlayerInfo().c_str(), (uint64)packet.Guid, packet.Money);

    if (GetPlayer()->GetGameObjectIfCanInteractWith(packet.Guid, GAMEOBJECT_TYPE_GUILD_BANK))
        if (packet.Money && GetPlayer()->HasEnoughMoney(packet.Money))
            if (Guild* guild = GetPlayer()->GetGuild())
                guild->HandleMemberDepositMoney(this, packet.Money);
}

void WorldSession::HandleGuildBankWithdrawMoney(WorldPackets::Guild::GuildBankWithdrawMoney& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_BANK_WITHDRAW_MONEY [{}]: Go: [" "{}" "], money: " UI64FMTD,
        GetPlayerInfo().c_str(), (uint64)packet.Guid, packet.Money);

    if (packet.Money && GetPlayer()->GetGameObjectIfCanInteractWith(packet.Guid, GAMEOBJECT_TYPE_GUILD_BANK))
        if (Guild* guild = GetPlayer()->GetGuild())
            guild->HandleMemberWithdrawMoney(this, packet.Money);
}

void WorldSession::HandleGuildBankSwapItems(WorldPackets::Guild::GuildBankSwapItems& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_BANK_SWAP_ITEMS [{}]", GetPlayerInfo().c_str());

    Guild* guild = GetPlayer()->GetGuild();
    if (!guild)
        return;

    ObjectGuid const banker = packet.Banker;

    if (!GetPlayer()->GetGameObjectIfCanInteractWith(banker, GAMEOBJECT_TYPE_GUILD_BANK))
        return;

    if (packet.BankOnly)
        guild->SwapItems(GetPlayer(), packet.BankTab2, packet.BankSlot2, packet.BankTab, packet.BankSlot, packet.StackCount);
    else
    {
        if (!Player::IsInventoryPos(packet.ContainerSlot, packet.ContainerItemSlot) && !(packet.ContainerSlot == NULL_BAG && packet.ContainerItemSlot == NULL_SLOT))
            GetPlayer()->SendEquipError(EQUIP_ERR_INTERNAL_BAG_ERROR, NULL);
        else
        {
            // What the fuck is this? Why client sends 255 for bag slot 0? Arrays start at -1? /s
            if (packet.ToSlot == 0 && packet.ContainerItemSlot == 255)
                packet.ContainerItemSlot = 0;
            guild->SwapItemsWithInventory(GetPlayer(), packet.ToSlot != 0, packet.BankTab, packet.BankSlot, packet.ContainerSlot, packet.ContainerItemSlot, packet.StackCount);
        }
    }
}

void WorldSession::HandleGuildBankBuyTab(WorldPackets::Guild::GuildBankBuyTab& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_BANK_BUY_TAB [{}]: Go: [" "{}" "], TabId: {}", GetPlayerInfo().c_str(), (uint64)packet.Guid, packet.TabId);

    // Since Cata you can buy tabs from the guild constrol tab - no need to check for guid
    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleBuyBankTab(this, packet.TabId);
}

void WorldSession::HandleGuildBankUpdateTab(WorldPackets::Guild::GuildBankUpdateTab& packet)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_BANK_UPDATE_TAB [{}]: Go: [" "{}" "], TabId: {}, Name: {}, Icon: {}",
        GetPlayerInfo().c_str(), (uint64)packet.Guid, packet.TabId, packet.Name.c_str(), packet.Icon.c_str());

    // Check for overflow
    if (packet.Name.length() > 16 || packet.Icon.length() > 128)
        return;

    // Cleanup bad characters
    cleanStr(packet.Name);

    if (!packet.Name.empty() && !packet.Icon.empty())
        if (GetPlayer()->GetGameObjectIfCanInteractWith(packet.Guid, GAMEOBJECT_TYPE_GUILD_BANK))
            if (Guild* guild = GetPlayer()->GetGuild())
                guild->HandleSetBankTabInfo(this, packet.TabId, packet.Name, packet.Icon);
}

void WorldSession::HandleGuildBankLogQuery(WorldPackets::Guild::GuildBankLogQuery& packet)
{
    TC_LOG_DEBUG("guild", "MSG_GUILD_BANK_LOG_QUERY [{}]: TabId: {}", GetPlayerInfo().c_str(), packet.TabId);

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->SendBankLog(this, packet.TabId);
}

void WorldSession::HandleQueryGuildBankTabText(WorldPackets::Guild::GuildBankTextQuery& packet)
{
    TC_LOG_DEBUG("guild", "MSG_QUERY_GUILD_BANK_TEXT [{}]: TabId: {}", GetPlayerInfo().c_str(), packet.TabId);

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->SendBankTabText(this, packet.TabId);
}

void WorldSession::HandleGuildQueryXPOpcode(WorldPackets::Guild::QueryGuildXP& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_QUERY_GUILD_XP");

    TC_LOG_DEBUG("guild", "CMSG_QUERY_GUILD_XP [{}]: Guild: {}", GetPlayerInfo().c_str(), packet.GuildGuid.GetCounter());

    if (Guild* guild = sGuildMgr->GetGuildByGuid(packet.GuildGuid))
        if (guild->IsMember(_player->GetGUID()))
            guild->SendGuildXP(this);
}

void WorldSession::HandleGuildSetRankPermissionsOpcode(WorldPackets::Guild::GuildSetRankPermissions& packet)
{
    Guild* guild = GetPlayer()->GetGuild();
    if (!guild)
        return;

    GuildBankRightsAndSlotsVec rightsAndSlots(GUILD_BANK_MAX_TABS);
    for (uint8 tabId = 0; tabId < GUILD_BANK_MAX_TABS; ++tabId)
        rightsAndSlots[tabId] = GuildBankRightsAndSlots(tabId, packet.RightsAndSlots[tabId].first, packet.RightsAndSlots[tabId].second);

    TC_LOG_DEBUG("guild", "CMSG_GUILD_SET_RANK_PERMISSIONS [{}]: Rank: {} ({})", GetPlayerInfo().c_str(), packet.RankName.c_str(), packet.RankIndex);

    guild->HandleSetRankInfo(this, packet.RankIndex, packet.RankName, packet.NewRights, packet.MoneyPerDay, rightsAndSlots);
}

void WorldSession::HandleGuildRequestPartyState(WorldPackets::Guild::RequestGuildPartyState& /*packet*/)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_REQUEST_GUILD_PARTY_STATE");

    uint32 requiredMemberCount = 0;
    float xpMult = 0.0f;
    uint32 currentMemberCount = 0;
    bool isGuildGroup = false;
    if (Group* group = GetPlayer()->GetGroup())
        isGuildGroup = group->IsGuildGroup(GetPlayer()->GetGuildId(), GetPlayer(), &xpMult, &currentMemberCount, &requiredMemberCount);

    WorldPacket data(SMSG_GUILD_PARTY_STATE_RESPONSE, 13);

    data << uint32(requiredMemberCount);
    data << float(xpMult);
    data << uint32(currentMemberCount);
    data.WriteBit(isGuildGroup);
    data.FlushBits();

    SendPacket(&data);
}

void WorldSession::HandleGuildRequestMaxDailyXP(WorldPacket& recvPacket)
{
    ObjectGuid guid;
    guid[0] = recvPacket.ReadBit();
    guid[3] = recvPacket.ReadBit();
    guid[5] = recvPacket.ReadBit();
    guid[1] = recvPacket.ReadBit();
    guid[4] = recvPacket.ReadBit();
    guid[6] = recvPacket.ReadBit();
    guid[7] = recvPacket.ReadBit();
    guid[2] = recvPacket.ReadBit();

    recvPacket.ReadByteSeq(guid[7]);
    recvPacket.ReadByteSeq(guid[4]);
    recvPacket.ReadByteSeq(guid[3]);
    recvPacket.ReadByteSeq(guid[5]);
    recvPacket.ReadByteSeq(guid[1]);
    recvPacket.ReadByteSeq(guid[2]);
    recvPacket.ReadByteSeq(guid[6]);
    recvPacket.ReadByteSeq(guid[0]);

    if (Guild* guild = sGuildMgr->GetGuildByGuid(guid))
    {
        if (guild->IsMember(_player->GetGUID()))
        {
            WorldPacket data(SMSG_GUILD_MAX_DAILY_XP, 8);
            data << uint64(sWorld->getIntConfig(CONFIG_GUILD_DAILY_XP_CAP));
            SendPacket(&data);
        }
    }
}

void WorldSession::HandleAutoDeclineGuildInvites(WorldPackets::Guild::DeclineGuildInvites& packet)
{
    GetPlayer()->ApplyModFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_AUTO_DECLINE_GUILD, packet.Enable);
}

void WorldSession::HandleGuildRewardsQueryOpcode(WorldPackets::Guild::RequestGuildRewardsList& /*packet*/)
{
    if (sGuildMgr->GetGuildById(_player->GetGuildId()))
    {
        std::vector<GuildReward> const& rewards = sGuildMgr->GetGuildRewards();

        WorldPacket data(SMSG_GUILD_REWARDS_LIST, 3 + rewards.size() * (4 + 4 + 4 + 8 + 4 + 4));
        data.WriteBits(rewards.size(), 19);

        for (uint32 i = 0; i < rewards.size(); i++)
            data.WriteBits(rewards[i].Achievements.size(), 22);

        data.FlushBits();

        for (uint32 i = 0; i < rewards.size(); i++)
        {
            for (uint32 j = 0; j < rewards[i].Achievements.size(); j++)
                data << uint32(rewards[i].Achievements[j]);

            data << int32(rewards[i].Racemask);
            data << uint32(rewards[i].Entry);
            data << uint32(0); // minGuildLevel
            data << uint32(rewards[i].Standing);
            data << uint64(rewards[i].Price);
        }

        data << uint32(time(NULL));
        SendPacket(&data);
    }
}

void WorldSession::HandleGuildQueryNewsOpcode(WorldPackets::Guild::GuildQueryNews& /*packet*/)
{
    TC_LOG_DEBUG("guild", "CMSG_GUILD_QUERY_NEWS [{}]", GetPlayerInfo().c_str());
    if (Guild* guild = GetPlayer()->GetGuild())
        guild->SendNewsUpdate(this);
}

void WorldSession::HandleGuildNewsUpdateStickyOpcode(WorldPackets::Guild::GuildNewsUpdateSticky& packet)
{
    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleNewsSetSticky(this, packet.NewsId, packet.Sticky);
}

void WorldSession::HandleGuildSetGuildMaster(WorldPackets::Guild::GuildSetGuildMaster& packet)
{
    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleSetNewGuildMaster(this, packet.PlayerName);
}

void WorldSession::HandleGuildReplaceGuildMaster(WorldPackets::Guild::GuildReplaceGuildMaster& /*packet*/)
{
    std::string playerName = GetPlayer()->GetName();

    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleSetNewGuildMaster(this, playerName, true);
}

void WorldSession::HandleGuildRequestChallengeUpdate(WorldPackets::Guild::GuildChallengeUpdateRequest& /*packet*/)
{
    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleRequestChallengeUpdate(this);
}

void WorldSession::HandleGuildBankTabNote(WorldPackets::Guild::GuildBankSetTabText& packet)
{
    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleSetBankTabNote(this, packet.TabId, packet.Note);
}

void WorldSession::HandleGuildSetAchievementTracking(WorldPackets::Guild::GuildSetAchievementTracking& packet)
{
    if (Guild* guild = GetPlayer()->GetGuild())
        guild->HandleSetAchievementTracking(this, packet.AchievementIds);
}

void WorldSession::HandleQueryGuildRecipes(WorldPackets::Guild::QueryGuildRecipes& packet)
{
    Guild* guild = sGuildMgr->GetGuildByGuid(packet.GuildGuid);
    if (!guild)
        return;

    if (GetPlayer()->GetGuildId() != guild->GetId())
        return;

    guild->HandleQueryGuildRecipes(this);
}

void WorldSession::HandleQueryGuildMembersForRecipe(WorldPackets::Guild::QueryGuildMembersForRecipe& packet)
{
    Guild* guild = sGuildMgr->GetGuildByGuid(packet.GuildGuid);
    if (!guild)
        return;

    if (GetPlayer()->GetGuildId() != guild->GetId())
        return;

    guild->HandleQueryGuildMembersForRecipe(this, packet.SpellId, packet.SkillId, packet.SkillValue);
}

void WorldSession::HandleQueryGuildMemberRecipes(WorldPackets::Guild::QueryGuildMemberRecipes& packet)
{
    Guild* guild = sGuildMgr->GetGuildByGuid(packet.GuildGuid);
    if (!guild)
        return;

    if (GetPlayer()->GetGuildId() != guild->GetId())
        return;

    if (!guild->IsMember(packet.MemberGuid))
        return;

    guild->HandleQueryGuildMemberRecipes(this, packet.MemberGuid, packet.SkillId);
}
