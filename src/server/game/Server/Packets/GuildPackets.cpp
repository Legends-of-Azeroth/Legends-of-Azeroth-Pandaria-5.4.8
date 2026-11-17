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

#include "GuildPackets.h"
#include "Guild.h"

void WorldPackets::Guild::QueryGuildInfo::Read()
{
    PlayerGuid[7] = _worldPacket.ReadBit();
    PlayerGuid[3] = _worldPacket.ReadBit();
    PlayerGuid[4] = _worldPacket.ReadBit();
    GuildGuid[3] = _worldPacket.ReadBit();
    GuildGuid[4] = _worldPacket.ReadBit();
    PlayerGuid[2] = _worldPacket.ReadBit();
    PlayerGuid[6] = _worldPacket.ReadBit();
    GuildGuid[2] = _worldPacket.ReadBit();
    GuildGuid[5] = _worldPacket.ReadBit();
    PlayerGuid[1] = _worldPacket.ReadBit();
    PlayerGuid[5] = _worldPacket.ReadBit();
    GuildGuid[7] = _worldPacket.ReadBit();
    PlayerGuid[0] = _worldPacket.ReadBit();
    GuildGuid[1] = _worldPacket.ReadBit();
    GuildGuid[6] = _worldPacket.ReadBit();
    GuildGuid[0] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(PlayerGuid[7]);
    _worldPacket.ReadByteSeq(GuildGuid[2]);
    _worldPacket.ReadByteSeq(GuildGuid[4]);
    _worldPacket.ReadByteSeq(GuildGuid[7]);
    _worldPacket.ReadByteSeq(PlayerGuid[6]);
    _worldPacket.ReadByteSeq(PlayerGuid[0]);
    _worldPacket.ReadByteSeq(GuildGuid[6]);
    _worldPacket.ReadByteSeq(GuildGuid[0]);
    _worldPacket.ReadByteSeq(GuildGuid[3]);
    _worldPacket.ReadByteSeq(PlayerGuid[2]);
    _worldPacket.ReadByteSeq(GuildGuid[5]);
    _worldPacket.ReadByteSeq(PlayerGuid[3]);
    _worldPacket.ReadByteSeq(GuildGuid[1]);
    _worldPacket.ReadByteSeq(PlayerGuid[4]);
    _worldPacket.ReadByteSeq(PlayerGuid[1]);
    _worldPacket.ReadByteSeq(PlayerGuid[5]);
}

void WorldPackets::Guild::GuildGetRoster::Read()
{
}

void WorldPackets::Guild::GuildGetRanks::Read()
{
    GuildGuid[0] = _worldPacket.ReadBit();
    GuildGuid[2] = _worldPacket.ReadBit();
    GuildGuid[5] = _worldPacket.ReadBit();
    GuildGuid[4] = _worldPacket.ReadBit();
    GuildGuid[3] = _worldPacket.ReadBit();
    GuildGuid[7] = _worldPacket.ReadBit();
    GuildGuid[6] = _worldPacket.ReadBit();
    GuildGuid[1] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(GuildGuid[6]);
    _worldPacket.ReadByteSeq(GuildGuid[0]);
    _worldPacket.ReadByteSeq(GuildGuid[1]);
    _worldPacket.ReadByteSeq(GuildGuid[7]);
    _worldPacket.ReadByteSeq(GuildGuid[3]);
    _worldPacket.ReadByteSeq(GuildGuid[2]);
    _worldPacket.ReadByteSeq(GuildGuid[5]);
    _worldPacket.ReadByteSeq(GuildGuid[4]);
}

void WorldPackets::Guild::GuildQueryNews::Read()
{
}

void WorldPackets::Guild::GuildEventLogQuery::Read()
{
}

void WorldPackets::Guild::GuildPermissionsQuery::Read()
{
}

void WorldPackets::Guild::GuildInviteByName::Read()
{
    uint32 nameLength = _worldPacket.ReadBits(9);
    InvitedName = _worldPacket.ReadString(nameLength);
}

void WorldPackets::Guild::GuildDeclineInvitation::Read()
{
}

void WorldPackets::Guild::GuildLeave::Read()
{
}

void WorldPackets::Guild::GuildDelete::Read()
{
}

void WorldPackets::Guild::GuildOfficerRemoveMember::Read()
{
    PlayerGuid[7] = _worldPacket.ReadBit();
    PlayerGuid[3] = _worldPacket.ReadBit();
    PlayerGuid[4] = _worldPacket.ReadBit();
    PlayerGuid[2] = _worldPacket.ReadBit();
    PlayerGuid[5] = _worldPacket.ReadBit();
    PlayerGuid[6] = _worldPacket.ReadBit();
    PlayerGuid[1] = _worldPacket.ReadBit();
    PlayerGuid[0] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(PlayerGuid[0]);
    _worldPacket.ReadByteSeq(PlayerGuid[2]);
    _worldPacket.ReadByteSeq(PlayerGuid[5]);
    _worldPacket.ReadByteSeq(PlayerGuid[6]);
    _worldPacket.ReadByteSeq(PlayerGuid[7]);
    _worldPacket.ReadByteSeq(PlayerGuid[1]);
    _worldPacket.ReadByteSeq(PlayerGuid[4]);
    _worldPacket.ReadByteSeq(PlayerGuid[3]);
}

void WorldPackets::Guild::GuildPromoteMember::Read()
{
    TargetGuid[6] = _worldPacket.ReadBit();
    TargetGuid[0] = _worldPacket.ReadBit();
    TargetGuid[4] = _worldPacket.ReadBit();
    TargetGuid[3] = _worldPacket.ReadBit();
    TargetGuid[1] = _worldPacket.ReadBit();
    TargetGuid[7] = _worldPacket.ReadBit();
    TargetGuid[2] = _worldPacket.ReadBit();
    TargetGuid[5] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(TargetGuid[1]);
    _worldPacket.ReadByteSeq(TargetGuid[7]);
    _worldPacket.ReadByteSeq(TargetGuid[2]);
    _worldPacket.ReadByteSeq(TargetGuid[5]);
    _worldPacket.ReadByteSeq(TargetGuid[3]);
    _worldPacket.ReadByteSeq(TargetGuid[4]);
    _worldPacket.ReadByteSeq(TargetGuid[0]);
    _worldPacket.ReadByteSeq(TargetGuid[6]);
}

void WorldPackets::Guild::GuildDemoteMember::Read()
{
    TargetGuid[3] = _worldPacket.ReadBit();
    TargetGuid[6] = _worldPacket.ReadBit();
    TargetGuid[0] = _worldPacket.ReadBit();
    TargetGuid[2] = _worldPacket.ReadBit();
    TargetGuid[7] = _worldPacket.ReadBit();
    TargetGuid[5] = _worldPacket.ReadBit();
    TargetGuid[4] = _worldPacket.ReadBit();
    TargetGuid[1] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(TargetGuid[7]);
    _worldPacket.ReadByteSeq(TargetGuid[4]);
    _worldPacket.ReadByteSeq(TargetGuid[2]);
    _worldPacket.ReadByteSeq(TargetGuid[5]);
    _worldPacket.ReadByteSeq(TargetGuid[1]);
    _worldPacket.ReadByteSeq(TargetGuid[3]);
    _worldPacket.ReadByteSeq(TargetGuid[0]);
    _worldPacket.ReadByteSeq(TargetGuid[6]);
}

void WorldPackets::Guild::GuildSetMemberNote::Read()
{
    PlayerGuid[1] = _worldPacket.ReadBit();
    uint32 noteLength = _worldPacket.ReadBits(8);
    PlayerGuid[4] = _worldPacket.ReadBit();
    PlayerGuid[2] = _worldPacket.ReadBit();
    IsPublic = _worldPacket.ReadBit();
    PlayerGuid[3] = _worldPacket.ReadBit();
    PlayerGuid[5] = _worldPacket.ReadBit();
    PlayerGuid[0] = _worldPacket.ReadBit();
    PlayerGuid[6] = _worldPacket.ReadBit();
    PlayerGuid[7] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(PlayerGuid[5]);
    _worldPacket.ReadByteSeq(PlayerGuid[1]);
    _worldPacket.ReadByteSeq(PlayerGuid[6]);
    Note = _worldPacket.ReadString(noteLength);
    _worldPacket.ReadByteSeq(PlayerGuid[0]);
    _worldPacket.ReadByteSeq(PlayerGuid[7]);
    _worldPacket.ReadByteSeq(PlayerGuid[4]);
    _worldPacket.ReadByteSeq(PlayerGuid[3]);
    _worldPacket.ReadByteSeq(PlayerGuid[2]);
}

void WorldPackets::Guild::GuildAssignMemberRank::Read()
{
    _worldPacket >> RankIndex;

    TargetGuid[2] = _worldPacket.ReadBit();
    TargetGuid[3] = _worldPacket.ReadBit();
    TargetGuid[1] = _worldPacket.ReadBit();
    TargetGuid[6] = _worldPacket.ReadBit();
    TargetGuid[0] = _worldPacket.ReadBit();
    TargetGuid[4] = _worldPacket.ReadBit();
    TargetGuid[7] = _worldPacket.ReadBit();
    TargetGuid[5] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(TargetGuid[7]);
    _worldPacket.ReadByteSeq(TargetGuid[3]);
    _worldPacket.ReadByteSeq(TargetGuid[2]);
    _worldPacket.ReadByteSeq(TargetGuid[5]);
    _worldPacket.ReadByteSeq(TargetGuid[6]);
    _worldPacket.ReadByteSeq(TargetGuid[0]);
    _worldPacket.ReadByteSeq(TargetGuid[4]);
    _worldPacket.ReadByteSeq(TargetGuid[1]);
}

void WorldPackets::Guild::GuildAddRank::Read()
{
    _worldPacket >> RankId;

    uint32 length = _worldPacket.ReadBits(7);
    RankName = _worldPacket.ReadString(length);
}

void WorldPackets::Guild::GuildDeleteRank::Read()
{
    _worldPacket >> RankIndex;
}

void WorldPackets::Guild::GuildShiftRank::Read()
{
    RankIndex = _worldPacket.read<int32>();
    Up = _worldPacket.ReadBit();
}

void WorldPackets::Guild::GuildSetRankPermissions::Read()
{
    uint32 unk;
    _worldPacket >> unk;

    RightsAndSlots.resize(8);
    for (uint8 tabId = 0; tabId < 8; ++tabId)
    {
        uint32 bankRights;
        uint32 slots;

        _worldPacket >> bankRights;
        _worldPacket >> slots;

        RightsAndSlots[tabId] = {uint8(bankRights), slots};
    }

    _worldPacket >> MoneyPerDay;
    uint32 oldRights;
    _worldPacket >> oldRights;
    _worldPacket >> NewRights;
    _worldPacket >> RankIndex;

    uint32 nameLength = _worldPacket.ReadBits(7);
    RankName = _worldPacket.ReadString(nameLength);
}

void WorldPackets::Guild::GuildUpdateInfoText::Read()
{
    uint32 length = _worldPacket.ReadBits(11);
    Info = _worldPacket.ReadString(length);
}

void WorldPackets::Guild::GuildUpdateMotdText::Read()
{
    uint32 motdLength = _worldPacket.ReadBits(10);
    Motd = _worldPacket.ReadString(motdLength);
}

void WorldPackets::Guild::GuildSetAchievementTracking::Read()
{
    Count = _worldPacket.ReadBits(22);

    for (uint32 i = 0; i < Count; ++i)
    {
        uint32 achievementId;
        _worldPacket >> achievementId;
        AchievementIds.insert(achievementId);
    }
}

void WorldPackets::Guild::GuildSetGuildMaster::Read()
{
    uint8 nameLength = _worldPacket.ReadBits(9);
    PlayerName = _worldPacket.ReadString(nameLength);
}

void WorldPackets::Guild::GuildReplaceGuildMaster::Read()
{
}

void WorldPackets::Guild::RequestGuildPartyState::Read()
{
    GuildGuid[1] = _worldPacket.ReadBit();
    GuildGuid[5] = _worldPacket.ReadBit();
    GuildGuid[7] = _worldPacket.ReadBit();
    GuildGuid[2] = _worldPacket.ReadBit();
    GuildGuid[6] = _worldPacket.ReadBit();
    GuildGuid[3] = _worldPacket.ReadBit();
    GuildGuid[0] = _worldPacket.ReadBit();
    GuildGuid[4] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(GuildGuid[2]);
    _worldPacket.ReadByteSeq(GuildGuid[5]);
    _worldPacket.ReadByteSeq(GuildGuid[4]);
    _worldPacket.ReadByteSeq(GuildGuid[6]);
    _worldPacket.ReadByteSeq(GuildGuid[1]);
    _worldPacket.ReadByteSeq(GuildGuid[0]);
    _worldPacket.ReadByteSeq(GuildGuid[7]);
    _worldPacket.ReadByteSeq(GuildGuid[3]);
}

void WorldPackets::Guild::RequestGuildRewardsList::Read()
{
    _worldPacket.read_skip<uint32>();
}

void WorldPackets::Guild::AcceptGuildInvite::Read()
{
}

void WorldPackets::Guild::DeclineGuildInvites::Read()
{
    Enable = _worldPacket.ReadBit();
}

void WorldPackets::Guild::GuildChallengeUpdateRequest::Read()
{
}

void WorldPackets::Guild::GuildBankActivate::Read()
{
    Guid[3] = _worldPacket.ReadBit();
    SendAllSlots = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[3]);
}

void WorldPackets::Guild::GuildBankQueryTab::Read()
{
    _worldPacket >> TabId;

    Guid[7] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    SendAllSlots = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[1]);
}

void WorldPackets::Guild::GuildBankLogQuery::Read()
{
    _worldPacket >> TabId;
}

void WorldPackets::Guild::GuildBankTextQuery::Read()
{
    _worldPacket >> TabId;
}

void WorldPackets::Guild::GuildBankBuyTab::Read()
{
    _worldPacket >> TabId;

    Guid[0] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[0]);
}

void WorldPackets::Guild::GuildBankUpdateTab::Read()
{
    _worldPacket >> TabId;

    Guid[5] = _worldPacket.ReadBit();
    uint32 iconLen = _worldPacket.ReadBits(9);
    Guid[1] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    uint32 nameLen = _worldPacket.ReadBits(7);

    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[4]);
    Icon = _worldPacket.ReadString(iconLen);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[0]);
    Name = _worldPacket.ReadString(nameLen);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[6]);
}

void WorldPackets::Guild::GuildBankDepositMoney::Read()
{
    _worldPacket >> Money;

    Guid[2] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[3]);
}

void WorldPackets::Guild::GuildBankWithdrawMoney::Read()
{
    _worldPacket >> Money;

    Guid[1] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[5]);
}

void WorldPackets::Guild::GuildBankRemainingWithdrawMoneyQuery::Read()
{
}

void WorldPackets::Guild::GuildBankSetTabText::Read()
{
    _worldPacket >> TabId;

    uint32 noteLength = _worldPacket.ReadBits(14);
    _worldPacket.FlushBits();

    Note = _worldPacket.ReadString(noteLength);
}

void WorldPackets::Guild::GuildBankSwapItems::Read()
{
    _worldPacket >> StackCount;
    _worldPacket >> BankSlot;
    _worldPacket >> ToSlot;
    _worldPacket >> ItemId;
    _worldPacket >> BankTab;

    Banker[5] = _worldPacket.ReadBit();
    HasBankTab2 = !_worldPacket.ReadBit();
    Banker[1] = _worldPacket.ReadBit();
    HasContainerSlot = !_worldPacket.ReadBit();
    AutoStore = _worldPacket.ReadBit();
    Banker[0] = _worldPacket.ReadBit();
    HasItemId2 = !_worldPacket.ReadBit();
    HasBankSlot2 = !_worldPacket.ReadBit();
    Banker[2] = _worldPacket.ReadBit();
    BankOnly = _worldPacket.ReadBit();
    Banker[4] = _worldPacket.ReadBit();
    Banker[7] = _worldPacket.ReadBit();
    Banker[3] = _worldPacket.ReadBit();
    HasContainerItemSlot = !_worldPacket.ReadBit();
    Banker[6] = _worldPacket.ReadBit();
    HasBankItemCount = !_worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Banker[2]);
    _worldPacket.ReadByteSeq(Banker[6]);
    _worldPacket.ReadByteSeq(Banker[5]);
    _worldPacket.ReadByteSeq(Banker[4]);
    _worldPacket.ReadByteSeq(Banker[0]);
    _worldPacket.ReadByteSeq(Banker[3]);
    _worldPacket.ReadByteSeq(Banker[1]);
    _worldPacket.ReadByteSeq(Banker[7]);

    ContainerSlot = NULL_BAG;
    ContainerItemSlot = NULL_SLOT;
    if (HasItemId2)
        _worldPacket >> ItemId2;
    if (HasContainerSlot)
        _worldPacket >> ContainerSlot;
    if (HasContainerItemSlot)
        _worldPacket >> ContainerItemSlot;
    if (HasBankSlot2)
        _worldPacket >> BankSlot2;
    if (HasBankItemCount)
        _worldPacket >> BankItemCount;
    if (HasBankTab2)
        _worldPacket >> BankTab2;
}

void WorldPackets::Guild::QueryGuildXP::Read()
{
    GuildGuid[5] = _worldPacket.ReadBit();
    GuildGuid[6] = _worldPacket.ReadBit();
    GuildGuid[0] = _worldPacket.ReadBit();
    GuildGuid[1] = _worldPacket.ReadBit();
    GuildGuid[3] = _worldPacket.ReadBit();
    GuildGuid[7] = _worldPacket.ReadBit();
    GuildGuid[4] = _worldPacket.ReadBit();
    GuildGuid[2] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(GuildGuid[4]);
    _worldPacket.ReadByteSeq(GuildGuid[6]);
    _worldPacket.ReadByteSeq(GuildGuid[3]);
    _worldPacket.ReadByteSeq(GuildGuid[0]);
    _worldPacket.ReadByteSeq(GuildGuid[7]);
    _worldPacket.ReadByteSeq(GuildGuid[5]);
    _worldPacket.ReadByteSeq(GuildGuid[2]);
    _worldPacket.ReadByteSeq(GuildGuid[1]);
}

void WorldPackets::Guild::GuildNewsUpdateSticky::Read()
{
    NewsId = _worldPacket.read<uint32>();

    Guid[6] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Sticky = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[7]);
}

void WorldPackets::Guild::QueryGuildRecipes::Read()
{
    GuildGuid[7] = _worldPacket.ReadBit();
    GuildGuid[4] = _worldPacket.ReadBit();
    GuildGuid[0] = _worldPacket.ReadBit();
    GuildGuid[3] = _worldPacket.ReadBit();
    GuildGuid[2] = _worldPacket.ReadBit();
    GuildGuid[5] = _worldPacket.ReadBit();
    GuildGuid[6] = _worldPacket.ReadBit();
    GuildGuid[1] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(GuildGuid[1]);
    _worldPacket.ReadByteSeq(GuildGuid[0]);
    _worldPacket.ReadByteSeq(GuildGuid[6]);
    _worldPacket.ReadByteSeq(GuildGuid[5]);
    _worldPacket.ReadByteSeq(GuildGuid[2]);
    _worldPacket.ReadByteSeq(GuildGuid[4]);
    _worldPacket.ReadByteSeq(GuildGuid[7]);
    _worldPacket.ReadByteSeq(GuildGuid[3]);
}

void WorldPackets::Guild::QueryGuildMembersForRecipe::Read()
{
    SpellId = _worldPacket.read<uint32>();
    SkillValue = _worldPacket.read<uint32>();
    SkillId = _worldPacket.read<uint32>();

    GuildGuid[6] = _worldPacket.ReadBit();
    GuildGuid[7] = _worldPacket.ReadBit();
    GuildGuid[3] = _worldPacket.ReadBit();
    GuildGuid[2] = _worldPacket.ReadBit();
    GuildGuid[5] = _worldPacket.ReadBit();
    GuildGuid[1] = _worldPacket.ReadBit();
    GuildGuid[0] = _worldPacket.ReadBit();
    GuildGuid[4] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(GuildGuid[0]);
    _worldPacket.ReadByteSeq(GuildGuid[3]);
    _worldPacket.ReadByteSeq(GuildGuid[2]);
    _worldPacket.ReadByteSeq(GuildGuid[5]);
    _worldPacket.ReadByteSeq(GuildGuid[6]);
    _worldPacket.ReadByteSeq(GuildGuid[7]);
    _worldPacket.ReadByteSeq(GuildGuid[4]);
    _worldPacket.ReadByteSeq(GuildGuid[1]);
}

void WorldPackets::Guild::QueryGuildMemberRecipes::Read()
{
    SkillId = _worldPacket.read<uint32>();

    GuildGuid[6] = _worldPacket.ReadBit();
    GuildGuid[7] = _worldPacket.ReadBit();
    MemberGuid[3] = _worldPacket.ReadBit();
    GuildGuid[1] = _worldPacket.ReadBit();
    GuildGuid[2] = _worldPacket.ReadBit();
    GuildGuid[4] = _worldPacket.ReadBit();
    MemberGuid[4] = _worldPacket.ReadBit();
    MemberGuid[1] = _worldPacket.ReadBit();
    GuildGuid[0] = _worldPacket.ReadBit();
    GuildGuid[3] = _worldPacket.ReadBit();
    MemberGuid[5] = _worldPacket.ReadBit();
    GuildGuid[5] = _worldPacket.ReadBit();
    MemberGuid[0] = _worldPacket.ReadBit();
    MemberGuid[7] = _worldPacket.ReadBit();
    MemberGuid[6] = _worldPacket.ReadBit();
    MemberGuid[2] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(GuildGuid[6]);
    _worldPacket.ReadByteSeq(MemberGuid[5]);
    _worldPacket.ReadByteSeq(MemberGuid[4]);
    _worldPacket.ReadByteSeq(MemberGuid[3]);
    _worldPacket.ReadByteSeq(MemberGuid[2]);
    _worldPacket.ReadByteSeq(GuildGuid[7]);
    _worldPacket.ReadByteSeq(MemberGuid[6]);
    _worldPacket.ReadByteSeq(GuildGuid[2]);
    _worldPacket.ReadByteSeq(MemberGuid[0]);
    _worldPacket.ReadByteSeq(MemberGuid[7]);
    _worldPacket.ReadByteSeq(GuildGuid[1]);
    _worldPacket.ReadByteSeq(GuildGuid[0]);
    _worldPacket.ReadByteSeq(GuildGuid[5]);
    _worldPacket.ReadByteSeq(MemberGuid[1]);
    _worldPacket.ReadByteSeq(GuildGuid[4]);
    _worldPacket.ReadByteSeq(GuildGuid[3]);
}
