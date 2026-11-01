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

#ifndef GuildPackets_h__
#define GuildPackets_h__

#include "Packet.h"

#include <set>
#include <string>
#include <utility>
#include <vector>

namespace WorldPackets
{
    namespace Guild
    {
        class QueryGuildInfo final : public ClientPacket
        {
        public:
            QueryGuildInfo(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_QUERY, std::move(packet)) { }

            void Read() override;

            ObjectGuid PlayerGuid;
            ObjectGuid GuildGuid;
        };

        class GuildGetRoster final : public ClientPacket
        {
        public:
            GuildGetRoster(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_ROSTER, std::move(packet)) { }

            void Read() override;
        };

        class GuildGetRanks final : public ClientPacket
        {
        public:
            GuildGetRanks(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_QUERY_RANKS, std::move(packet)) { }

            void Read() override;

            ObjectGuid GuildGuid;
        };

        class GuildQueryNews final : public ClientPacket
        {
        public:
            GuildQueryNews(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_QUERY_NEWS, std::move(packet)) { }

            void Read() override;
        };

        class GuildEventLogQuery final : public ClientPacket
        {
        public:
            GuildEventLogQuery(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_EVENT_LOG_QUERY, std::move(packet)) { }

            void Read() override;
        };

        class GuildPermissionsQuery final : public ClientPacket
        {
        public:
            GuildPermissionsQuery(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_PERMISSIONS, std::move(packet)) { }

            void Read() override;
        };

        class GuildInviteByName final : public ClientPacket
        {
        public:
            GuildInviteByName(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_INVITE, std::move(packet)) { }

            void Read() override;

            std::string InvitedName;
        };

        class GuildDeclineInvitation final : public ClientPacket
        {
        public:
            GuildDeclineInvitation(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_DECLINE, std::move(packet)) { }

            void Read() override;
        };

        class GuildLeave final : public ClientPacket
        {
        public:
            GuildLeave(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_LEAVE, std::move(packet)) { }

            void Read() override;
        };

        class GuildDelete final : public ClientPacket
        {
        public:
            GuildDelete(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_DISBAND, std::move(packet)) { }

            void Read() override;
        };

        class GuildOfficerRemoveMember final : public ClientPacket
        {
        public:
            GuildOfficerRemoveMember(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_REMOVE, std::move(packet)) { }

            void Read() override;

            ObjectGuid PlayerGuid;
        };

        class GuildPromoteMember final : public ClientPacket
        {
        public:
            GuildPromoteMember(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_PROMOTE, std::move(packet)) { }

            void Read() override;

            ObjectGuid TargetGuid;
        };

        class GuildDemoteMember final : public ClientPacket
        {
        public:
            GuildDemoteMember(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_DEMOTE, std::move(packet)) { }

            void Read() override;

            ObjectGuid TargetGuid;
        };

        class GuildSetMemberNote final : public ClientPacket
        {
        public:
            GuildSetMemberNote(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_SET_NOTE, std::move(packet)) { }

            void Read() override;

            ObjectGuid PlayerGuid;
            std::string Note;
            bool IsPublic = false;
        };

        class GuildAssignMemberRank final : public ClientPacket
        {
        public:
            GuildAssignMemberRank(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_ASSIGN_MEMBER_RANK, std::move(packet)) { }

            void Read() override;

            uint32 RankIndex = 0;
            ObjectGuid TargetGuid;
        };

        class GuildAddRank final : public ClientPacket
        {
        public:
            GuildAddRank(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_ADD_RANK, std::move(packet)) { }

            void Read() override;

            uint32 RankId = 0;
            std::string RankName;
        };

        class GuildDeleteRank final : public ClientPacket
        {
        public:
            GuildDeleteRank(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_DEL_RANK, std::move(packet)) { }

            void Read() override;

            int32 RankIndex = 0;
        };

        class GuildShiftRank final : public ClientPacket
        {
        public:
            GuildShiftRank(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_SWITCH_RANK, std::move(packet)) { }

            void Read() override;

            int32 RankIndex = 0;
            bool Up = false;
        };

        class GuildSetRankPermissions final : public ClientPacket
        {
        public:
            GuildSetRankPermissions(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_SET_RANK_PERMISSIONS, std::move(packet)) { }

            void Read() override;

            std::vector<std::pair<uint8, uint32>> RightsAndSlots;
            uint32 MoneyPerDay = 0;
            uint32 NewRights = 0;
            uint32 RankIndex = 0;
            std::string RankName;
        };

        class GuildUpdateInfoText final : public ClientPacket
        {
        public:
            GuildUpdateInfoText(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_INFO_TEXT, std::move(packet)) { }

            void Read() override;

            std::string Info;
        };

        class GuildUpdateMotdText final : public ClientPacket
        {
        public:
            GuildUpdateMotdText(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_MOTD, std::move(packet)) { }

            void Read() override;

            std::string Motd;
        };

        class GuildSetAchievementTracking final : public ClientPacket
        {
        public:
            GuildSetAchievementTracking(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_SET_ACHIEVEMENT_TRACKING, std::move(packet)) { }

            void Read() override;

            uint32 Count = 0;
            std::set<uint32> AchievementIds;
        };

        class GuildSetGuildMaster final : public ClientPacket
        {
        public:
            GuildSetGuildMaster(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_SET_GUILD_MASTER, std::move(packet)) { }

            void Read() override;

            std::string PlayerName;
        };

        class GuildReplaceGuildMaster final : public ClientPacket
        {
        public:
            GuildReplaceGuildMaster(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_REPLACE_GUILD_MASTER, std::move(packet)) { }

            void Read() override;
        };

        class RequestGuildPartyState final : public ClientPacket
        {
        public:
            RequestGuildPartyState(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_GUILD_PARTY_STATE, std::move(packet)) { }

            void Read() override;

            ObjectGuid GuildGuid;
        };

        class RequestGuildRewardsList final : public ClientPacket
        {
        public:
            RequestGuildRewardsList(WorldPacket&& packet) : ClientPacket(CMSG_QUERY_GUILD_REWARDS, std::move(packet)) { }

            void Read() override;
        };

        class AcceptGuildInvite final : public ClientPacket
        {
        public:
            AcceptGuildInvite(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_ACCEPT, std::move(packet)) { }

            void Read() override;
        };

        class DeclineGuildInvites final : public ClientPacket
        {
        public:
            DeclineGuildInvites(WorldPacket&& packet) : ClientPacket(CMSG_AUTO_DECLINE_GUILD_INVITES, std::move(packet)) { }

            void Read() override;

            bool Enable = false;
        };

        class GuildChallengeUpdateRequest final : public ClientPacket
        {
        public:
            GuildChallengeUpdateRequest(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_CHALLENGE_UPDATE_REQUEST, std::move(packet)) { }

            void Read() override;
        };

        class GuildBankActivate final : public ClientPacket
        {
        public:
            GuildBankActivate(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_BANKER_ACTIVATE, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
            bool SendAllSlots = false;
        };

        class GuildBankQueryTab final : public ClientPacket
        {
        public:
            GuildBankQueryTab(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_BANK_QUERY_TAB, std::move(packet)) { }

            void Read() override;

            uint8 TabId = 0;
            ObjectGuid Guid;
            bool SendAllSlots = false;
        };

        class GuildBankLogQuery final : public ClientPacket
        {
        public:
            GuildBankLogQuery(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_BANK_LOG_QUERY, std::move(packet)) { }

            void Read() override;

            uint32 TabId = 0;
        };

        class GuildBankTextQuery final : public ClientPacket
        {
        public:
            GuildBankTextQuery(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_BANK_TEXT_QUERY, std::move(packet)) { }

            void Read() override;

            uint32 TabId = 0;
        };

        class GuildBankBuyTab final : public ClientPacket
        {
        public:
            GuildBankBuyTab(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_BANK_BUY_TAB, std::move(packet)) { }

            void Read() override;

            uint8 TabId = 0;
            ObjectGuid Guid;
        };

        class GuildBankUpdateTab final : public ClientPacket
        {
        public:
            GuildBankUpdateTab(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_BANK_UPDATE_TAB, std::move(packet)) { }

            void Read() override;

            uint8 TabId = 0;
            ObjectGuid Guid;
            std::string Icon;
            std::string Name;
        };

        class GuildBankDepositMoney final : public ClientPacket
        {
        public:
            GuildBankDepositMoney(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_BANK_DEPOSIT_MONEY, std::move(packet)) { }

            void Read() override;

            uint64 Money = 0;
            ObjectGuid Guid;
        };

        class GuildBankWithdrawMoney final : public ClientPacket
        {
        public:
            GuildBankWithdrawMoney(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_BANK_WITHDRAW_MONEY, std::move(packet)) { }

            void Read() override;

            uint64 Money = 0;
            ObjectGuid Guid;
        };

        class GuildBankRemainingWithdrawMoneyQuery final : public ClientPacket
        {
        public:
            GuildBankRemainingWithdrawMoneyQuery(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_BANK_MONEY_WITHDRAWN_QUERY, std::move(packet)) { }

            void Read() override;
        };

        class GuildBankSetTabText final : public ClientPacket
        {
        public:
            GuildBankSetTabText(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_BANK_NOTE, std::move(packet)) { }

            void Read() override;

            uint32 TabId = 0;
            std::string Note;
        };

        class GuildBankSwapItems final : public ClientPacket
        {
        public:
            GuildBankSwapItems(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_BANK_SWAP_ITEMS, std::move(packet)) { }

            void Read() override;

            uint32 StackCount = 0;
            uint8 BankSlot = 0;
            uint8 ToSlot = 0;
            uint32 ItemId = 0;
            uint8 BankTab = 0;
            ObjectGuid Banker;
            bool HasBankTab2 = false;
            bool HasContainerSlot = false;
            bool AutoStore = false;
            bool HasItemId2 = false;
            bool HasBankSlot2 = false;
            bool BankOnly = false;
            bool HasContainerItemSlot = false;
            bool HasBankItemCount = false;
            uint32 ItemId2 = 0;
            uint8 ContainerSlot = 0;
            uint8 ContainerItemSlot = 0;
            uint8 BankSlot2 = 0;
            uint32 BankItemCount = 0;
            uint8 BankTab2 = 0;
        };

        class QueryGuildXP final : public ClientPacket
        {
        public:
            QueryGuildXP(WorldPacket&& packet) : ClientPacket(CMSG_QUERY_GUILD_XP, std::move(packet)) { }

            void Read() override;

            ObjectGuid GuildGuid;
        };

        class GuildNewsUpdateSticky final : public ClientPacket
        {
        public:
            GuildNewsUpdateSticky(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_NEWS_UPDATE_STICKY, std::move(packet)) { }

            void Read() override;

            uint32 NewsId = 0;
            bool Sticky = false;
            ObjectGuid Guid;
        };

        class QueryGuildRecipes final : public ClientPacket
        {
        public:
            QueryGuildRecipes(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_QUERY_RECIPES, std::move(packet)) { }

            void Read() override;

            ObjectGuid GuildGuid;
        };

        class QueryGuildMembersForRecipe final : public ClientPacket
        {
        public:
            QueryGuildMembersForRecipe(WorldPacket&& packet) : ClientPacket(CMSG_QUERY_GUILD_MEMBERS_FOR_RECIPE, std::move(packet)) { }

            void Read() override;

            uint32 SpellId = 0;
            uint32 SkillValue = 0;
            uint32 SkillId = 0;
            ObjectGuid GuildGuid;
        };

        class QueryGuildMemberRecipes final : public ClientPacket
        {
        public:
            QueryGuildMemberRecipes(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_QUERY_MEMBER_RECIPES, std::move(packet)) { }

            void Read() override;

            uint32 SkillId = 0;
            ObjectGuid GuildGuid;
            ObjectGuid MemberGuid;
        };
    }
}

#endif // GuildPackets_h__
