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

/// \addtogroup u2w
/// @{
/// \file

#ifndef __WORLDSESSION_H
#define __WORLDSESSION_H

#include <atomic>
#include <memory>
#include <map>
#include <utility>
#include "AllPackets.h"
#include "Common.h"
#include "SharedDefines.h"
#include "AddonMgr.h"
#include "World.h"
#include "WorldPacket.h"
#include "Cryptography/BigNumber.h"
#include "Opcodes.h"
#include "AccountMgr.h"
#include "Object.h"
#include "AsyncCallbackProcessor.h"
#include "DatabaseEnvFwd.h"
#include <boost/circular_buffer.hpp>

class BigNumber;
class AccountAchievementMgr;

class PetBattle;
class Creature;
class CharacterBooster;
class GameObject;
class InstanceSave;
class Item;
class LoginQueryHolder;
class Object;
class Player;
class GameClient;
class Quest;
class SpellCastTargets;
class Unit;
class Warden;
class WorldPacket;
class WorldSocket;
struct AreaTableEntry;
struct AuctionEntry;
struct DeclinedName;
struct ItemTemplate;
struct MovementInfo;
struct PetBattleRequest;

namespace WorldPackets
{
    namespace Misc
    {
        class CompleteCinematic;
        class CompleteMovie;
        class NextCinematicCamera;
        class OpeningCinematic;
        class TogglePvP;
        class SetPvP;
    }
}

namespace lfg
{
struct LfgJoinResultData;
struct LfgPlayerBoot;
struct LfgProposal;
struct LfgQueueStatusData;
struct LfgPlayerRewardData;
struct LfgRoleCheck;
struct PlayerQueueData;
enum LfgUpdateType : uint32;
}

namespace rbac
{
class RBACData;
}

enum BlackMarketError : int32;

enum AccountDataType
{
    GLOBAL_CONFIG_CACHE             = 0,                    // 0x01 g
    PER_CHARACTER_CONFIG_CACHE      = 1,                    // 0x02 p
    GLOBAL_BINDINGS_CACHE           = 2,                    // 0x04 g
    PER_CHARACTER_BINDINGS_CACHE    = 3,                    // 0x08 p
    GLOBAL_MACROS_CACHE             = 4,                    // 0x10 g
    PER_CHARACTER_MACROS_CACHE      = 5,                    // 0x20 p
    PER_CHARACTER_LAYOUT_CACHE      = 6,                    // 0x40 p
    PER_CHARACTER_CHAT_CACHE        = 7                     // 0x80 p
};

#define NUM_ACCOUNT_DATA_TYPES        8

#define GLOBAL_CACHE_MASK           0x15
#define PER_CHARACTER_CACHE_MASK    0xEA

#define REGISTERED_ADDON_PREFIX_SOFTCAP 64

struct AccountData
{
    AccountData() : Time(0), Data("") { }

    time_t Time;
    std::string Data;
};

enum PartyOperation
{
    PARTY_OP_INVITE = 0,
    PARTY_OP_UNINVITE = 1,
    PARTY_OP_LEAVE = 2,
    PARTY_OP_SWAP = 4
};

enum BFLeaveReason
{
    BF_LEAVE_REASON_CLOSE     = 0x00000001,
    //BF_LEAVE_REASON_UNK1      = 0x00000002, (not used)
    //BF_LEAVE_REASON_UNK2      = 0x00000004, (not used)
    BF_LEAVE_REASON_EXITED    = 0x00000008,
    BF_LEAVE_REASON_LOW_LEVEL = 0x00000010
};

enum ChatRestrictionType
{
    ERR_CHAT_RESTRICTED = 0,
    ERR_CHAT_THROTTLED  = 1,
    ERR_USER_SQUELCHED  = 2,
    ERR_YELL_RESTRICTED = 3
};

enum CharterTypes
{
    GUILD_CHARTER_TYPE                            = 4,
    ARENA_TEAM_CHARTER_2v2_TYPE                   = 2,
    ARENA_TEAM_CHARTER_3v3_TYPE                   = 3,
    ARENA_TEAM_CHARTER_5v5_TYPE                   = 5,
};

enum BarberShopResult
{
    BARBER_SHOP_SUCCESS          = 0,
    BARBER_SHOP_NOT_ENOUGH_MONEY = 1,
    BARBER_SHOP_NOT_SITTING      = 2
    /*BARBER_SHOP_NOT_ENOUGH_MONEY = 3*/
};

enum DB2Types : uint32
{
    DB2_REPLY_BROADCAST_TEXT                   = 35137211,       // hash of BroadcastText.db2
    DB2_REPLY_MAP_CHALLENGE_MODE               = 943410215,      // hash of MapChallengeMode.db2
    DB2_REPLY_ITEM                             = 1344507586,     // hash of Item.db2
    DB2_REPLY_BATTLE_PET_EFFECT_PROPERTIES     = 1672791226,     // hash of BattlePetEffectProperties.db2
    DB2_REPLY_ITEM_UPGRADE                     = 1879459387,     // hash of ItemUpgrade.db2
    DB2_REPLY_SPARSE                           = 2442913102,     // hash of Item-sparse.db2
    DB2_REPLY_ITEM_EXTENDED_COST               = 3146089301,     // hash of ItemExtendedCost.db2
    DB2_REPLY_SCENE_SCRIPT                     = 3568395212,     // hash of ScreneScript.db2
    DB2_REPLY_SPELL_VISUAL                     = 4146370265,     // hash of SpellVisual.db2
};

enum AccountFlags
{
    ACC_FLAG_TOURNAMENT_OBSERVER = 0x00000002, // not used, just for compatibility with wotlk
    ACC_FLAG_VALOR_CAP_REACHED   = 0x00000004,
};

enum TutorialsFlag : uint8
{
    TUTORIALS_FLAG_NONE                           = 0x00,
    TUTORIALS_FLAG_CHANGED                        = 0x01,
    TUTORIALS_FLAG_LOADED_FROM_DB                 = 0x02
};

//class to deal with packet processing
//allows to determine if next packet is safe to be processed
class PacketFilter
{
public:
    explicit PacketFilter(WorldSession* pSession) : m_pSession(pSession) { }
    virtual ~PacketFilter() { }

    virtual bool Process(WorldPacket* /*packet*/) { return true; }
    virtual bool ProcessUnsafe() const { return true; }
    static uint16 DropHighBytes(uint16 opcode) { return opcode & NUM_OPCODE_HANDLERS; }

protected:
    WorldSession* const m_pSession;
};
//process only thread-safe packets in Map::Update()
class MapSessionFilter : public PacketFilter
{
public:
    explicit MapSessionFilter(WorldSession* pSession) : PacketFilter(pSession) { }
    ~MapSessionFilter() { }

    virtual bool Process(WorldPacket* packet);
    //in Map::Update() we do not process player logout!
    virtual bool ProcessUnsafe() const { return false; }
};

//class used to filer only thread-unsafe packets from queue
//in order to update only be used in World::UpdateSessions()
class WorldSessionFilter : public PacketFilter
{
public:
    explicit WorldSessionFilter(WorldSession* pSession) : PacketFilter(pSession) { }
    ~WorldSessionFilter() { }

    virtual bool Process(WorldPacket* packet);
};

// Proxy structure to contain data passed to callback function,
// only to prevent bloating the parameter list
class CharacterCreateInfo
{
    friend class WorldSession;
    friend class Player;

    public:
        CharacterCreateInfo(std::string const& name, uint8 race, uint8 cclass, uint8 gender, uint8 skin, uint8 face, uint8 hairStyle, uint8 hairColor, uint8 facialHair, uint8 outfitId,
        WorldPacket& data) : Name(name), Race(race), Class(cclass), Gender(gender), Skin(skin), Face(face), HairStyle(hairStyle), HairColor(hairColor), FacialHair(facialHair),
        OutfitId(outfitId), Data(data), CharCount(0)
        { }

        /// User specified variables
        std::string Name;
        uint8 Race;
        uint8 Class;
        uint8 Gender;
        uint8 Skin;
        uint8 Face;
        uint8 HairStyle;
        uint8 HairColor;
        uint8 FacialHair;
        uint8 OutfitId;
        WorldPacket Data;

        /// Server side data
        uint8 CharCount;
};

struct PacketCounter
{
    time_t lastReceiveTime;
    uint32 amountCounter;
};

struct MuteInfo
{
    MuteInfo() : Timer(0) { }
    MuteInfo(uint64 timer, std::string const& author, std::string const& reason, bool publicChannelsOnly) :
        Timer(timer), Author(author), Reason(reason), PublicChannelsOnly(publicChannelsOnly) { }

    // Remaining online mute time in ms
    uint32 Timer;
    std::string Author;
    std::string Reason;
    bool PublicChannelsOnly;
};

/// Player session in the World
class TC_GAME_API WorldSession 
{
    public:
        WorldSession(uint32 id, std::string const& accountName, std::shared_ptr<WorldSocket> sock, AccountTypes sec, uint8 expansion, time_t mute_time, LocaleConstant locale, uint32 recruiter, uint32 flags, bool isARecruiter, bool hasBoost, bool isBot = false);
        ~WorldSession();

        bool PlayerLoading() const { return m_playerLoading; }
        bool PlayerLogout() const { return m_playerLogout; }
        bool PlayerLogoutWithSave() const { return m_playerLogout && m_playerSave; }
        bool PlayerRecentlyLoggedOut() const { return m_playerRecentlyLogout; }
        bool PlayerDisconnected() const { return !m_Socket; }

        void ReadAddonsInfo(WorldPacket& data);
        void SendAddonsInfo();
        void SendTimezoneInformation();
        bool IsAddonRegistered(const std::string& prefix) const;

        void SendPacket(WorldPacket const* packet, bool forced = false);
        void SendNotification(const char *format, ...) ATTR_PRINTF(2, 3);
        void SendNotification(uint32 string_id, ...);
        void SendPetNameInvalid(uint32 error, std::string const& name, DeclinedName *declinedName);
        void SendPartyResult(PartyOperation operation, std::string const& member, PartyResult res, uint32 val = 0);
        void SendAreaTriggerMessage(char const* Text, ...) ATTR_PRINTF(2, 3);
        void SendSetPhaseShift(std::set<uint32> const& phaseIds, std::set<uint32> const& terrainswaps, std::set<uint32> const& worldAreas, uint32 flags = 0);
        void SendQueryTimeResponse();
        void SendGroupInviteNotification(const std::string& inviterName, bool inGroup);
        void SendRolePollInform(uint8 Index);
        void SendServerWorldInfo();

        void SendAuthResponse(uint8 code, bool queued, uint32 queuePos = 0);
        void SendClientCacheVersion(uint32 version);

        void InitializeSession();
        void InitializeSessionCallback(CharacterDatabaseQueryHolder const& realmHolder);

        void SendFeatureSystemStatusGlueScreen();
        void SendDanceStudioCreateResult();
        void SendDispalyPromotionOpcode();

        AccountTypes GetSecurity() const { return _security; }
        uint32 GetAccountId() const { return _accountId; }
        std::string const& GetAccountName() const { return _accountName; }
        Player* GetPlayer() const { return _player; }
        GameClient* GetGameClient() const { return _gameClient; }
        std::string const& GetPlayerName() const;
        std::string GetPlayerInfo() const;

        uint32 GetCurrentVendor() const { return m_currentVendorEntry; }
        void SetCurrentVendor(uint32 vendorEntry) { m_currentVendorEntry = vendorEntry; }

        ObjectGuid GetGUID() const;
        uint32 GetGuidLow() const;

        rbac::RBACData* GetRBACData() const { return _RBACData; }
        bool HasPermission(uint32 permissionId);
        void LoadPermissions();
        QueryCallback LoadPermissionsAsync();
        void InvalidateRBACData();
        std::string const& GetRemoteAddress() { return m_Address; }
        void SetPlayer(Player* player);
        uint8 Expansion() const { return m_expansion; }

        void InitWarden(BigNumber* k, std::string const& os);
        Warden* GetWarden() const { return _warden; }

        /// Session in auth.queue currently
        void SetInQueue(bool state) { m_inQueue = state; }

        /// Is the user engaged in a log out process?
        bool isLogingOut() const { return _logoutTime || m_playerLogout; }

        bool IsBot() { return _isBot; }

        /// Engage the logout process for the user
        void SetLogoutStartTime(time_t requestTime)
        {
            _logoutTime = requestTime;
        }

        /// Is logout cooldown expired?
        bool ShouldLogOut(time_t currTime) const
        {
            return (_logoutTime > 0 && currTime >= _logoutTime + 20);
        }

        void LogoutPlayer(bool save);
        void KickPlayerOP(std::string const & reason);
        void KickPlayer();
        bool forceExit;

        void QueuePacket(WorldPacket* new_packet);
        bool Update(uint32 diff, PacketFilter& updater);

        /// Handle the authentication waiting queue (to be completed)
        void SendAuthWaitQue(uint32 position);

        //void SendTestCreatureQueryOpcode(uint32 entry, uint64 guid, uint32 testvalue);
        void SendNameQueryOpcode(ObjectGuid guid);

        void SendRealmNameQueryOpcode(uint32 realmId);

        void SendTrainerList(ObjectGuid guid);
        void SendTrainerList(ObjectGuid guid, std::string const& strTitle, bool multiple = false, uint32 mTrainer = 0);
        void SendListInventory(ObjectGuid guid, uint32 vendorEntry = 0);
        void SendShowBank(ObjectGuid guid);
        void SendShowMailBox(ObjectGuid guid);
        void SendTabardVendorActivate(ObjectGuid guid);
        void SendSpiritResurrect();
        void SendBindPoint(Creature* npc);

        void SendAttackStop(Unit const* enemy);

        void SendBattleGroundList(ObjectGuid guid, BattlegroundTypeId bgTypeId = BATTLEGROUND_RB);

        void SendTradeStatus(TradeStatus status);
        void SendUpdateTrade(bool trader_data = true);
        void SendCancelTrade();

        void SendPetitionQueryOpcode(ObjectGuid petitionguid);

        void SendPlayerChoice(uint32 choiceId);

        // Totem
        void SendTotemCreated(ObjectGuid TotemGUID, uint32 Duration, uint32 SpellID, uint8 Slot);

        // Pet
        void SendPetNameQuery(ObjectGuid guid, uint64 petNumber);
        void SendPetList(ObjectGuid guid, uint8 first, uint8 last);
        void SendStableResult(uint8 guid);
        bool CheckStableMaster(ObjectGuid guid);

        // Account Data
        AccountData* GetAccountData(AccountDataType type) { return &m_accountData[type]; }
        void SetAccountData(AccountDataType type, time_t tm, std::string const& data);
        void SendAccountDataTimes(uint32 mask);
        void LoadGlobalAccountData();
        void LoadAccountData(PreparedQueryResult result, uint32 mask);

        void LoadTutorialsData(PreparedQueryResult result);
        void SendTutorialsData();
        void SaveTutorialsData(CharacterDatabaseTransaction trans);
        uint32 GetTutorialInt(uint8 index) const { return m_Tutorials[index]; }
        void SetTutorialInt(uint8 index, uint32 value)
        {
            if (m_Tutorials[index] != value)
            {
                m_Tutorials[index] = value;
                m_TutorialsChanged |= TUTORIALS_FLAG_CHANGED;
            }
        }
        //used with item_page table
        bool SendItemInfo(uint32 itemid, WorldPacket data);
        //auction
        void SendAuctionHello(ObjectGuid guid, Creature* unit);
        void SendAuctionCommandResult(AuctionEntry* auction, uint32 Action, uint32 ErrorCode, uint32 bidError = 0);
        void SendAuctionBidderNotification(uint32 location, uint32 auctionId, ObjectGuid bidder, uint32 bidSum, uint32 diff, uint32 item_template);
        void SendAuctionOwnerNotification(AuctionEntry* auction);
        //void SendAuctionRemovedNotification(uint32 auctionId, uint32 itemEntry, int32 randomPropertyId);

        //Item Enchantment
        void SendEnchantmentLog(ObjectGuid target, ObjectGuid caster, ObjectGuid item, uint32 itemId, uint32 enchantId, uint8 slot);
        void SendItemEnchantTimeUpdate(ObjectGuid Playerguid, ObjectGuid Itemguid, uint32 slot, uint32 Duration);

        //Taxi
        void SendTaxiStatus(ObjectGuid guid);
        void SendTaxiMenu(Creature* unit);
        void SendDoFlight(uint32 mountDisplayId, uint32 path, uint32 pathNode = 0);
        bool SendLearnNewTaxiNode(Creature* unit);
        void SendDiscoverNewTaxiNode(uint32 nodeid);

        // Guild/Arena Team
        void SendArenaTeamCommandResult(uint32 team_action, std::string const& team, std::string const& player, uint32 error_id = 0);
        void SendNotInArenaTeamPacket(uint8 type);
        void SendPetitionShowList(ObjectGuid guid);

        void BuildPartyMemberStatsChangedPacket(Player* player, WorldPacket* data, uint32 mask, ObjectGuid guid, bool full = false);

        void DoLootRelease(ObjectGuid lguid);
        void DoLootReleaseAll();

        // Account mute time
        time_t m_muteTime;

        // Locales
        LocaleConstant GetSessionDbcLocale() const { return m_sessionDbcLocale; }
        LocaleConstant GetSessionDbLocaleIndex() const { return m_sessionDbLocaleIndex; }
        const char *GetTrinityString(int32 entry) const;

        uint32 GetLatency() const { return m_latency; }
        void SetLatency(uint32 latency) { m_latency = latency; }

        uint32 AdjustClientMovementTime(uint32 time) const;
        void ComputeNewClockDelta();

        std::atomic<time_t> m_timeOutTime;
        void UpdateTimeOutTime(uint32 diff)
        {
            if (time_t(diff) > m_timeOutTime)
                m_timeOutTime = 0;
            else
                m_timeOutTime -= diff;
        }
        //void ResetTimeOutTime() { m_timeOutTime = sWorld->getIntConfig(CONFIG_SOCKET_TIMEOUTTIME); }
        void ResetTimeOutTime(bool onlyActive);

        bool IsConnectionIdle() const { return (m_timeOutTime <= 0 && !m_inQueue); }

        // Recruit-A-Friend Handling
        uint32 GetRecruiterId() const { return recruiterId; }
        bool IsARecruiter() const { return isRecruiter; }

        bool HasBoost() const { return m_hasBoost; }
        void SetBoost(bool boost) { m_hasBoost = boost; }
        CharacterBooster* GetBoost() { return m_charBooster; }

        AccountAchievementMgr& GetAchievementMgr() const { return *_achievementMgr; }

        bool HasFlag(AccountFlags flag) const { return m_flags & flag; }
        void AddFlag(AccountFlags flag);
        void RemoveFlag(AccountFlags flag);

        MuteInfo& GetMute() { return _mute; }
        void SetMute(MuteInfo const& mute) { _mute = mute; }

        void EnterSoloQueue(bool asGroup);

        bool queueAnnounceSent = false;

    public:                                                 // opcodes handlers

        void Handle_NULL(WorldPacket& recvPacket);          // not used
        void Handle_EarlyProccess(WorldPacket& recvPacket); // just mark packets processed in WorldSocket::OnRead
        void Handle_Deprecated(WorldPacket& recvPacket);    // never used anymore by client
        void LogUnprocessedTail(WorldPacket const* packet);

        void HandleCharEnumOpcode(WorldPackets::Character::EnumCharacters& /*enumCharacters*/);
        void HandleCharDeleteOpcode(WorldPackets::Character::CharDelete& packet);
        void HandleCharCreateOpcode(WorldPackets::Character::CreateCharacter& packet);
        void HandlePlayerLoginOpcode(WorldPackets::Character::PlayerLogin& packet);
        void HandleLoadScreenOpcode(WorldPackets::Character::LoadingScreenNotify& packet);
        void HandleCharEnum(PreparedQueryResult result);
        void HandlePlayerLogin(LoginQueryHolder const& holder);
        void HandleCharFactionOrRaceChange(WorldPackets::Character::CharFactionOrRaceChange& packet);
        void HandleRandomizeCharNameOpcode(WorldPackets::Character::RandomizeCharName& packet);
        void HandleReorderCharacters(WorldPackets::Character::ReorderCharacters& packet);
        void HandleOpeningCinematic(WorldPackets::Misc::OpeningCinematic& packet);

        // played time
        void HandlePlayedTime(WorldPackets::Misc::PlayedTimeClient& packet);

        // new
        void HandleMoveUnRootAck(WorldPackets::Movement::MoveUnRootAck& packet);
        void HandleMoveRootAck(WorldPackets::Movement::MoveRootAck& packet);
        void HandleLookingForGroup(WorldPacket& recvPacket);
        void HandleReturnToGraveyard(WorldPackets::Combat::PortGraveyard& packet);

        // new inspect
        void HandleInspectOpcode(WorldPackets::Inspect::Inspect& packet);

        // new party stats
        void HandleInspectHonorStatsOpcode(WorldPackets::Inspect::InspectHonorStats& packet);
        void HandleInspectRatedBGStatsOpcode(WorldPackets::Inspect::InspectRatedBGStats& packet);

        void HandleMoveWaterWalkAck(WorldPackets::Movement::MoveWaterWalkAck& packet);
        void HandleFeatherFallAck(WorldPackets::Movement::FeatherFallAck& packet);

        void HandleMoveHoverAck(WorldPackets::Movement::MoveHoverAck& packet);
        void HandleMoveGravityAck(WorldPackets::Movement::MoveGravityAck& packet);

        void HandleMountSpecialAnimOpcode(WorldPackets::Movement::MountSpecialAnim& packet);

        // character view
        void HandleShowingHelmOpcode(WorldPackets::Character::ShowingHelm& packet);
        void HandleShowingCloakOpcode(WorldPackets::Character::ShowingCloak& packet);

        // repair
        void HandleRepairItemOpcode(WorldPackets::Item::RepairItem& packet);

        // Knockback
        void HandleMoveKnockBackAck(WorldPackets::Movement::MoveKnockBackAck& packet);

        void HandleMoveTeleportAck(WorldPackets::Movement::MoveTeleportAck& packet);
        void HandleForceSpeedChangeAck(WorldPackets::Movement::MovementSpeedAck& packet);
        void HandleSetCollisionHeightAck(WorldPackets::Movement::MoveSetCollisionHeightAck& packet);
        void HandleMovementForceAck(WorldPackets::Movement::MoveApplyMovementForceAck& packet);
        void HandleMoveSetCanTurnWhileFallingAck(WorldPackets::Movement::MoveSetCanTurnWhileFallingAck& packet);

        void HandlePingOpcode(WorldPacket& recvPacket);
        void HandleAuthSessionOpcode(WorldPacket& recvPacket);
        void HandleRepopRequestOpcode(WorldPackets::Combat::RepopRequest& packet);
        void HandleAutostoreLootItemOpcode(WorldPackets::Loot::LootItem& packet);
        void HandleLootMoneyOpcode(WorldPackets::Loot::LootMoney& packet);
        void HandleLootOpcode(WorldPackets::Loot::LootUnit& packet);
        void HandleLootReleaseOpcode(WorldPackets::Loot::LootRelease& packet);
        void HandleLootMasterGiveOpcode(WorldPackets::Loot::MasterLootItem& packet);
        void HandleWhoOpcode(WorldPackets::Who::WhoRequestPkt& recvPacket);
        void HandleLogoutRequestOpcode(WorldPackets::Misc::LogoutRequest& packet);
        void HandlePlayerLogoutOpcode(WorldPacket& recvPacket);
        void HandleLogoutCancelOpcode(WorldPackets::Misc::LogoutCancel& packet);
        void HandleChangeCurrencyFlags(WorldPackets::Loot::SetCurrencyFlags& packet);

        // GM Ticket opcodes
        void HandleGMTicketCreateOpcode(WorldPackets::Ticket::GMTicketCreate& recvPacket);
        void HandleGMTicketUpdateOpcode(WorldPackets::Ticket::GMTicketUpdateText& recvPacket);
        void HandleGMTicketDeleteOpcode(WorldPackets::Ticket::GMTicketDeleteTicket& recvPacket);
        void HandleGMTicketGetTicketOpcode(WorldPackets::Ticket::GMTicketGetTicket& recvPacket);
        void HandleGMTicketSystemStatusOpcode(WorldPackets::Ticket::GMTicketGetSystemStatus& recvPacket);
        void HandleGMTicketCaseStatusOpcode(WorldPackets::Ticket::GMTicketGetCaseStatus& recvPacket);
        void HandleGMSurveySubmit(WorldPacket& recvPacket);
        void HandleReportLag(WorldPacket& recvPacket);
        void HandleGMResponseResolve(WorldPacket& recvPacket);

        // Gm Bug Ticket
        void HandleSubmitBugOpcode(WorldPacket& recvPacket);

        void HandleTogglePvP(WorldPackets::Misc::TogglePvP& togglePvP);
        void HandleSetPvP(WorldPackets::Misc::SetPvP& setPvP);

        void HandleZoneUpdateOpcode(WorldPacket& recvPacket);
        void HandleSetSelectionOpcode(WorldPackets::Misc::SetSelection& packet);
        void HandleStandStateChangeOpcode(WorldPackets::Misc::StandStateChange& packet);
        void HandleEmoteOpcode(WorldPackets::Chat::EmoteClient& packet);
        void HandleContactListOpcode(WorldPackets::Social::SendContactList& recvPacket);
        void HandleAddFriendOpcode(WorldPackets::Social::AddFriend& recvPacket);
        void HandleAddFriendOpcodeCallBack(PreparedQueryResult result, std::string const& friendNote);
        void HandleDelFriendOpcode(WorldPackets::Social::DelFriend& recvPacket);
        void HandleAddIgnoreOpcode(WorldPackets::Social::AddIgnore& recvPacket);
        void HandleAddIgnoreOpcodeCallBack(PreparedQueryResult result);
        void HandleDelIgnoreOpcode(WorldPackets::Social::DelIgnore& recvPacket);
        void HandleSetContactNotesOpcode(WorldPackets::Social::SetContactNotes& recvPacket);
        void HandleBugOpcode(WorldPacket& recvPacket);

        void HandleAreaTriggerOpcode(WorldPackets::AreaTrigger::AreaTrigger& packet);

        void HandleSetFactionAtWar(WorldPackets::Character::SetFactionAtWar& packet);
        void HandleSetFactionNotAtWar(WorldPackets::Character::SetFactionNotAtWar& packet);
        void HandleSetFactionCheat(WorldPacket& recvData);
        void HandleSetWatchedFactionOpcode(WorldPackets::Character::SetWatchedFaction& packet);
        void HandleSetFactionInactiveOpcode(WorldPackets::Character::SetFactionInactive& packet);
        void HandleRequestForcedReactionsOpcode(WorldPackets::Character::RequestForcedReactions& packet);

        void HandleUpdateAccountData(WorldPackets::ClientConfig::UserClientUpdateAccountData& packet);
        void HandleRequestAccountData(WorldPackets::ClientConfig::RequestAccountData& packet);
        void HandleSetActionButtonOpcode(WorldPackets::Misc::SetActionButton& packet);

        void HandleGameObjectUseOpcode(WorldPackets::GameObject::GameObjUse& packet);
        void HandleMeetingStoneInfo(WorldPacket& recPacket);
        void HandleGameobjectReportUse(WorldPackets::GameObject::GameObjReportUse& packet);

        void HandleNameQueryOpcode(WorldPackets::Query::QueryPlayerName& packet);
        void HandleRealmNameQueryOpcode(WorldPackets::Query::QueryRealmName& packet);

        void HandleQueryTimeOpcode(WorldPacket& recvPacket);
        void HandleQueryCountdownTimer(WorldPackets::Misc::QueryCountdownTimer& packet);

        void HandleCreatureQueryOpcode(WorldPackets::Query::QueryCreature& packet);

        void HandleGameObjectQueryOpcode(WorldPackets::Query::QueryGameObject& packet);

        void HandleMoveWorldportAckOpcode(WorldPackets::Movement::WorldPortResponse& packet);
        void HandleMoveWorldportAck();

        void HandleMovementOpcodes(WorldPackets::Movement::ClientPlayerMovement& packet);
        void HandleMovementOpcode(OpcodeClient opcode, MovementInfo& movementInfo);
        void HandleSetActiveMoverOpcode(WorldPackets::Movement::SetActiveMover& packet);
        void HandleMoveNotActiveMover(WorldPacket& recvData);
        void HandleDismissControlledVehicle(WorldPackets::Vehicle::MoveDismissVehicle& recvData);
        void HandleRequestVehicleExit(WorldPackets::Vehicle::RequestVehicleExit& recvData);
        void HandleMoveSetVehicleRecAck(WorldPackets::Vehicle::MoveSetVehicleRecIdAck& recvData);
        void HandleChangeSeatsOnControlledVehicle(WorldPacket& recvData);
        void HandleMoveTimeSkippedOpcode(WorldPackets::Movement::MoveTimeSkipped& packet);

        void HandleRequestRaidInfoOpcode(WorldPackets::Party::RequestRaidInfo& packet);

        void HandleBattlefieldStatusOpcode(WorldPackets::Battleground::RequestBattlefieldStatus& packet);
        void HandleBattleMasterHelloOpcode(WorldPacket& recvData);

        void HandleGroupInviteOpcode(WorldPackets::Party::PartyInviteClient& packet);
        //void HandleGroupCancelOpcode(WorldPacket& recvPacket);
        void HandleGroupInviteResponseOpcode(WorldPackets::Party::GroupInviteResponse& packet);
        //void HandleGroupUninviteOpcode(WorldPacket& recvPacket);
        void HandleGroupUninviteGuidOpcode(WorldPackets::Party::GroupUninviteGuid& packet);
        void HandleGroupSetLeaderOpcode(WorldPackets::Party::GroupSetLeader& packet);
        void HandleGroupSetRolesOpcode(WorldPackets::Party::GroupSetRoles& packet);
        void HandleGroupDisbandOpcode(WorldPackets::Party::GroupDisband& packet);
        void HandleOptOutOfLootOpcode(WorldPackets::Party::OptOutOfLoot& packet);
        void HandleLootMethodOpcode(WorldPackets::Party::LootMethod& packet);
        void HandleLootRoll(WorldPackets::Loot::LootRoll& packet);
        void HandleRequestPartyMemberStatsOpcode(WorldPackets::Party::RequestPartyMemberStats& packet);
        void HandleRaidTargetUpdateOpcode(WorldPacket& recvData);
        void HandleRaidReadyCheckOpcode(WorldPackets::Party::RaidReadyCheck& packet);
        void HandleRaidReadyCheckConfirmOpcode(WorldPackets::Party::RaidReadyCheckConfirm& packet);
        void HandleRaidReadyCheckFinishedOpcode(WorldPacket& recvData);
        void HandleGroupRaidConvertOpcode(WorldPackets::Party::GroupRaidConvert& packet);
        void HandleGroupRequestJoinUpdates(WorldPackets::Party::GroupRequestJoinUpdates& packet);
        void HandleGroupChangeSubGroupOpcode(WorldPackets::Party::GroupChangeSubGroup& packet);
        void HandleGroupSwapSubGroupOpcode(WorldPacket& recvData);
        void HandleGroupAssistantLeaderOpcode(WorldPackets::Party::GroupAssistantLeader& packet);
        void HandleGroupEveryoneIsAssistantOpcode(WorldPackets::Party::SetEveryoneIsAssistant& packet);
        void HandlePartyAssignmentOpcode(WorldPacket& recvData);
        void HandleGroupInitiatePollRole(WorldPackets::Party::RolePollBegin& packet);
        void HandleClearRaidMarkerOpcode(WorldPackets::Party::ClearRaidMarker& packet);

        void HandlePetitionBuyOpcode(WorldPackets::Petitions::PetitionBuy& packet);
        void HandlePetitionShowSignOpcode(WorldPackets::Petitions::PetitionShowSignatures& packet);
        void HandlePetitionQueryOpcode(WorldPackets::Petitions::QueryPetition& packet);
        void HandlePetitionRenameOpcode(WorldPackets::Petitions::PetitionRename& packet);
        void HandlePetitionSignOpcode(WorldPackets::Petitions::SignPetition& packet);
        void HandlePetitionDeclineOpcode(WorldPackets::Petitions::PetitionDecline& packet);
        void HandleOfferPetitionOpcode(WorldPackets::Petitions::OfferPetition& packet);
        void HandleTurnInPetitionOpcode(WorldPackets::Petitions::TurnInPetition& packet);
        void SendPetitionSignResults(ObjectGuid petitionGuid, ObjectGuid playerGuid, uint8 result);

        void HandleGuildQueryOpcode(WorldPackets::Guild::QueryGuildInfo& packet);
        void HandleGuildInviteOpcode(WorldPackets::Guild::GuildInviteByName& packet);
        void HandleGuildRemoveOpcode(WorldPackets::Guild::GuildOfficerRemoveMember& packet);
        void HandleGuildAcceptOpcode(WorldPackets::Guild::AcceptGuildInvite& packet);
        void HandleGuildDeclineOpcode(WorldPackets::Guild::GuildDeclineInvitation& packet);
        void HandleGuildEventLogQueryOpcode(WorldPackets::Guild::GuildEventLogQuery& packet);
        void HandleGuildRosterOpcode(WorldPackets::Guild::GuildGetRoster& packet);
        void HandleGuildRewardsQueryOpcode(WorldPackets::Guild::RequestGuildRewardsList& packet);
        void HandleGuildPromoteOpcode(WorldPackets::Guild::GuildPromoteMember& packet);
        void HandleGuildDemoteOpcode(WorldPackets::Guild::GuildDemoteMember& packet);
        void HandleGuildAssignRankOpcode(WorldPackets::Guild::GuildAssignMemberRank& packet);
        void HandleGuildLeaveOpcode(WorldPackets::Guild::GuildLeave& packet);
        void HandleGuildDisbandOpcode(WorldPackets::Guild::GuildDelete& packet);
        void HandleGuildSetAchievementTracking(WorldPackets::Guild::GuildSetAchievementTracking& packet);
        void HandleGuildSetGuildMaster(WorldPackets::Guild::GuildSetGuildMaster& packet);
        void HandleGuildReplaceGuildMaster(WorldPackets::Guild::GuildReplaceGuildMaster& packet);
        void HandleGuildMOTDOpcode(WorldPackets::Guild::GuildUpdateMotdText& packet);
        void HandleGuildNewsUpdateStickyOpcode(WorldPackets::Guild::GuildNewsUpdateSticky& packet);
        void HandleGuildSetNoteOpcode(WorldPackets::Guild::GuildSetMemberNote& packet);
        void HandleGuildQueryRanksOpcode(WorldPackets::Guild::GuildGetRanks& packet);
        void HandleGuildQueryNewsOpcode(WorldPackets::Guild::GuildQueryNews& packet);
        void HandleGuildSetRankPermissionsOpcode(WorldPackets::Guild::GuildSetRankPermissions& packet);
        void HandleGuildAddRankOpcode(WorldPackets::Guild::GuildAddRank& packet);
        void HandleGuildDelRankOpcode(WorldPackets::Guild::GuildDeleteRank& packet);
        void HandleGuildSwitchRank(WorldPackets::Guild::GuildShiftRank& packet);
        void HandleGuildChangeInfoTextOpcode(WorldPackets::Guild::GuildUpdateInfoText& packet);
        void HandleSaveGuildEmblemOpcode(WorldPacket& recvPacket);
        void HandleGuildRequestPartyState(WorldPackets::Guild::RequestGuildPartyState& packet);
        void HandleGuildRequestMaxDailyXP(WorldPacket& recvPacket);
        void HandleGuildRequestChallengeUpdate(WorldPackets::Guild::GuildChallengeUpdateRequest& packet);
        void HandleAutoDeclineGuildInvites(WorldPackets::Guild::DeclineGuildInvites& packet);
        void HandleQueryGuildRecipes(WorldPackets::Guild::QueryGuildRecipes& packet);
        void HandleQueryGuildMembersForRecipe(WorldPackets::Guild::QueryGuildMembersForRecipe& packet);
        void HandleQueryGuildMemberRecipes(WorldPackets::Guild::QueryGuildMemberRecipes& packet);

        void HandleGuildFinderAddApplication(WorldPackets::GuildFinder::LFGuildAddApplication& packet);
        void HandleGuildFinderBrowse(WorldPackets::GuildFinder::LFGuildBrowse& packet);
        void HandleGuildFinderDeclineRecruit(WorldPackets::GuildFinder::LFGuildDeclineRecruit& packet);
        void HandleGuildFinderGetApplications(WorldPackets::GuildFinder::LFGuildGetApplications& packet);
        void HandleGuildFinderGetRecruits(WorldPackets::GuildFinder::LFGuildGetRecruits& packet);
        void HandleGuildFinderPostRequest(WorldPackets::GuildFinder::LFGuildPostRequest& packet);
        void HandleGuildFinderRemoveApplication(WorldPackets::GuildFinder::LFGuildRemoveApplication& packet);
        void HandleGuildFinderSetGuildPost(WorldPackets::GuildFinder::LFGuildSetGuildPost& packet);

        void HandleTaxiNodeStatusQueryOpcode(WorldPackets::Taxi::TaxiNodeStatusQuery& recvPacket);
        void HandleTaxiQueryAvailableNodes(WorldPackets::Taxi::TaxiQueryAvailableNodes& recvPacket);
        void HandleEnableTaxi(WorldPackets::Taxi::EnableTaxiNode& recvPacket);
        void HandleActivateTaxiOpcode(WorldPackets::Taxi::ActivateTaxi& recvPacket);
        void HandleActivateTaxiExpressOpcode(WorldPackets::Taxi::ActivateTaxiExpress& recvPacket);
        void HandleMoveSplineDoneOpcode(WorldPacket& recvPacket);
        void SendActivateTaxiReply(ActivateTaxiReply reply);

        void HandleTabardVendorActivateOpcode(WorldPackets::NPC::TabardVendorActivate& packet);
        void HandleBankerActivateOpcode(WorldPackets::Bank::BankerActivate& packet);
        void HandleBuyBankSlotOpcode(WorldPackets::Bank::BuyBankSlot& packet);
        void HandleTrainerListOpcode(WorldPackets::NPC::TrainerList& packet);
        void HandleTrainerBuySpellOpcode(WorldPackets::NPC::TrainerBuySpell& packet);
        void HandlePetitionShowListOpcode(WorldPackets::Petitions::PetitionShowList& packet);
        void HandleGossipHelloOpcode(WorldPackets::NPC::GossipHello& packet);
        void HandleGossipSelectOptionOpcode(WorldPacket& recvPacket);
        void HandleSpiritHealerActivateOpcode(WorldPackets::NPC::SpiritHealerActivate& packet);
        void HandleNpcTextQueryOpcode(WorldPackets::Query::QueryNPCText& packet);
        void HandleBinderActivateOpcode(WorldPackets::NPC::BinderActivate& packet);
        void HandleListPets(WorldPackets::NPC::ListPets& packet);
        void HandleSetPetSlot(WorldPackets::NPC::SetPetSlot& packet);
        void HandleStablePetCallback(PreparedQueryResult result);
        void HandleUnstablePet(WorldPacket& recvPacket);
        void HandleUnstablePetCallback(PreparedQueryResult result, uint32 petId);
        void HandleBuyStableSlot(WorldPacket& recvPacket);
        void HandleStableRevivePet(WorldPacket& recvPacket);
        void HandleStableSwapPet(WorldPacket& recvPacket);
        void HandleStableSwapPetCallback(PreparedQueryResult result, uint32 petId);
        void HandleLearnPetSpecialization(WorldPackets::Pet::LearnPetSpecializationGroup& packet);
        void SendTrainerBuyFailed(ObjectGuid guid, uint32 spellId, uint32 reason);

        void HandleDuelProposedOpcode(WorldPackets::Duel::CanDuel& packet);
        void HandleDuelResponseOpcode(WorldPackets::Duel::DuelResponse& packet);

        void HandleAcceptTradeOpcode(WorldPackets::Trade::AcceptTrade& recvPacket);
        void HandleBeginTradeOpcode(WorldPackets::Trade::BeginTrade& recvPacket);
        void HandleBusyTradeOpcode(WorldPacket& recvPacket);
        void HandleCancelTradeOpcode(WorldPackets::Trade::CancelTrade& recvPacket);
        void HandleClearTradeItemOpcode(WorldPackets::Trade::ClearTradeItem& recvPacket);
        void HandleIgnoreTradeOpcode(WorldPackets::Trade::IgnoreTrade& recvPacket);
        void HandleInitiateTradeOpcode(WorldPackets::Trade::InitiateTrade& recvPacket);
        void HandleSetTradeGoldOpcode(WorldPackets::Trade::SetTradeGold& recvPacket);
        void HandleSetTradeItemOpcode(WorldPackets::Trade::SetTradeItem& recvPacket);
        void HandleUnacceptTradeOpcode(WorldPackets::Trade::UnacceptTrade& recvPacket);

        void HandleAuctionHelloOpcode(WorldPackets::AuctionHouse::AuctionHello& packet);
        void HandleAuctionListItems(WorldPackets::AuctionHouse::AuctionListItems& packet);
        void HandleAuctionListBidderItems(WorldPackets::AuctionHouse::AuctionListBidderItems& packet);
        void HandleAuctionSellItem(WorldPackets::AuctionHouse::AuctionSellItem& packet);
        void HandleAuctionRemoveItem(WorldPackets::AuctionHouse::AuctionRemoveItem& packet);
        void HandleAuctionListOwnerItems(WorldPackets::AuctionHouse::AuctionListOwnerItems& packet);
        void HandleAuctionPlaceBid(WorldPackets::AuctionHouse::AuctionPlaceBid& packet);
        void HandleAuctionListPendingSales(WorldPackets::AuctionHouse::AuctionListPendingSales& packet);
        void HandleReplicateItems(WorldPackets::AuctionHouse::AuctionReplicateItems& packet);

        bool CanOpenMailBox(ObjectGuid guid);

        void HandleGetMailList(WorldPackets::Mail::MailGetList& recvData);
        void HandleSendMail(WorldPackets::Mail::SendMail& recvData);
        void HandleMailTakeMoney(WorldPackets::Mail::MailTakeMoney& recvData);
        void HandleMailTakeItem(WorldPackets::Mail::MailTakeItem& recvData);
        void HandleMailMarkAsRead(WorldPackets::Mail::MailMarkAsRead& recvData);
        void HandleMailReturnToSender(WorldPackets::Mail::MailReturnToSender& recvData);
        void HandleMailDelete(WorldPackets::Mail::MailDelete& recvData);
        void HandleItemTextQuery(WorldPackets::Item::ItemTextQuery& packet);
        void HandleMailCreateTextItem(WorldPackets::Mail::MailCreateTextItem& recvData);
        void HandleQueryNextMailTime(WorldPackets::Mail::MailQueryNextMailTime& recvData);
        void HandleCancelChanneling(WorldPackets::Combat::CancelChannelling& packet);

        void SendItemPageInfo(ItemTemplate* itemProto);
        void HandleSplitItemOpcode(WorldPackets::Item::SplitItem& packet);
        void HandleSwapInvItemOpcode(WorldPackets::Item::SwapInvItem& packet);
        void HandleDestroyItemOpcode(WorldPackets::Item::DestroyItem& packet);
        void HandleAutoEquipItemOpcode(WorldPackets::Item::AutoEquipItem& packet);
        void HandleSellItemOpcode(WorldPackets::Item::SellItem& packet);
        void HandleBuyItemInSlotOpcode(WorldPacket& recvPacket);
        void HandleBuyItemOpcode(WorldPackets::Item::BuyItem& packet);
        void HandleListInventoryOpcode(WorldPackets::NPC::ListInventory& packet);
        void HandleAutoStoreBagItemOpcode(WorldPackets::Item::AutoStoreBagItem& packet);
        void HandleReadItem(WorldPackets::Item::ReadItem& packet);
        void HandleAutoEquipItemSlotOpcode(WorldPackets::Item::AutoEquipItemSlot& packet);
        void HandleSwapItem(WorldPackets::Item::SwapItem& packet);
        void HandleBuybackItem(WorldPackets::Item::BuyBackItem& packet);
        void HandleAutoBankItemOpcode(WorldPackets::Bank::AutoBankItem& packet);
        void HandleAutoStoreBankItemOpcode(WorldPackets::Bank::AutoStoreBankItem& packet);
        void HandleWrapItemOpcode(WorldPackets::Item::WrapItem& packet);
        void HandleUpgradeItemOpcode(WorldPackets::Item::UpgradeItem& packet);
        void SendItemUpgradeResult(bool success);
        void HandleSetLootSpecialization(WorldPackets::Item::SetLootSpecialization& packet);

        void HandleAttackSwingOpcode(WorldPackets::Combat::AttackSwing& packet);
        void HandleAttackStopOpcode(WorldPackets::Combat::AttackStop& packet);
        void HandleSetSheathedOpcode(WorldPackets::Combat::SetSheathed& packet);

        void HandleUseItemOpcode(WorldPackets::Spells::UseItem& packet);
        void HandleOpenItemOpcode(WorldPackets::Item::OpenItem& packet);
        void HandleCastSpellOpcode(WorldPackets::Spells::CastSpell& packet);
        void HandleCancelCastOpcode(WorldPackets::Combat::CancelCast& packet);
        void HandleCancelAuraOpcode(WorldPackets::Combat::CancelAura& packet);
        void HandleCancelModSpeedNoControlAuras(WorldPackets::Combat::CancelModSpeedNoControlAuras& packet);
        void HandleCancelGrowthAuraOpcode(WorldPacket& recvPacket);
        void HandleCancelAutoRepeatSpellOpcode(WorldPackets::Combat::CancelAutoRepeatSpell& packet);

        void HandleLearnTalentOpcode(WorldPackets::Talent::LearnTalent& packet);
        void HandleLearnPreviewTalents(WorldPacket& recvPacket);
        void HandleRespecWipeConfirmOpcode(WorldPackets::Talent::ConfirmRespecWipe& packet);
        void HandleUnlearnSkillOpcode(WorldPackets::Talent::UnlearnSkill& packet);
        void HandleUnlearnSpecializationOpcode(WorldPackets::Talent::UnlearnSpecialization& packet);

        void HandleArcheologyRequestHistory(WorldPackets::Talent::RequestResearchHistory& packet);

        void HandleQuestgiverStatusQueryOpcode(WorldPackets::Quest::QuestGiverStatusQuery& packet);
        void HandleQuestgiverStatusMultipleQuery(WorldPackets::Quest::QuestGiverStatusMultipleQuery& packet);
        void HandleQuestgiverHelloOpcode(WorldPackets::Quest::QuestGiverHello& packet);
        void HandleQuestgiverAcceptQuestOpcode(WorldPackets::Quest::QuestGiverAcceptQuest& packet);
        void HandleQuestgiverQueryQuestOpcode(WorldPackets::Quest::QuestGiverQueryQuest& packet);
        void HandleQuestgiverChooseRewardOpcode(WorldPackets::Quest::QuestGiverChooseReward& packet);
        void HandleQuestgiverRequestRewardOpcode(WorldPackets::Quest::QuestGiverRequestReward& packet);
        void HandleQuestQueryOpcode(WorldPackets::Quest::QueryQuestInfo& packet);
        void HandleQuestgiverCancel(WorldPacket& recvData);
        void HandleQuestLogSwapQuest(WorldPacket& recvData);
        void HandleQuestLogRemoveQuest(WorldPackets::Quest::QuestLogRemoveQuest& packet);
        void HandleQuestConfirmAccept(WorldPackets::Quest::QuestConfirmAccept& packet);
        void HandleQuestgiverCompleteQuest(WorldPackets::Quest::QuestGiverCompleteQuest& packet);
        void HandleQuestgiverQuestAutoLaunch(WorldPacket& recvPacket);
        void HandlePushQuestToParty(WorldPackets::Quest::PushQuestToParty& packet);
        void HandleQuestPushResult(WorldPackets::Quest::QuestPushResult& packet);

        void HandleMessagechatOpcode(WorldPacket& recvPacket);
        void HandleAddonMessagechatOpcode(WorldPacket& recvPacket);
        void SendPlayerNotFoundNotice(std::string const& name);
        void SendPlayerAmbiguousNotice(std::string const& name);
        void SendChatRestrictedNotice(ChatRestrictionType restriction);
        void HandleTextEmoteOpcode(WorldPackets::Chat::CTextEmote& packet);
        void HandleChatIgnoredOpcode(WorldPackets::Chat::ChatReportIgnored& packet);

        void HandleUnregisterAddonPrefixesOpcode(WorldPacket& recvPacket);
        void HandleAddonRegisteredPrefixesOpcode(WorldPacket& recvPacket);

        void HandleReclaimCorpseOpcode(WorldPackets::Combat::ReclaimCorpse& packet);
        void HandleCorpseQueryOpcode(WorldPackets::Query::QueryCorpseLocationFromClient& packet);
        void HandleCorpseMapPositionQuery(WorldPackets::Query::QueryCorpseTransport& packet);
        void HandleResurrectResponseOpcode(WorldPackets::Combat::ResurrectResponse& packet);
        void HandleSummonResponseOpcode(WorldPackets::Movement::SummonResponse& packet);

        void HandleJoinChannel(WorldPackets::Channel::JoinChannel& recvPacket);
        void HandleLeaveChannel(WorldPackets::Channel::LeaveChannel& recvPacket);
        void HandleChannelList(WorldPackets::Channel::ChannelList& recvPacket);
        void HandleChannelPassword(WorldPackets::Channel::ChannelPassword& recvPacket);
        void HandleChannelSetOwner(WorldPackets::Channel::ChannelSetOwner& recvPacket);
        void HandleChannelOwner(WorldPackets::Channel::ChannelOwner& recvPacket);
        void HandleChannelModerator(WorldPackets::Channel::ChannelModerator& recvPacket);
        void HandleChannelUnmoderator(WorldPackets::Channel::ChannelUnmoderator& recvPacket);
        void HandleChannelMute(WorldPackets::Channel::ChannelMute& recvPacket);
        void HandleChannelUnmute(WorldPackets::Channel::ChannelUnmute& recvPacket);
        void HandleChannelInvite(WorldPackets::Channel::ChannelInvite& recvPacket);
        void HandleChannelKick(WorldPackets::Channel::ChannelKick& recvPacket);
        void HandleChannelBan(WorldPackets::Channel::ChannelBan& recvPacket);
        void HandleChannelUnban(WorldPackets::Channel::ChannelUnban& recvPacket);
        void HandleChannelAnnouncements(WorldPackets::Channel::ChannelAnnouncements& recvPacket);
        void HandleChannelModerate(WorldPacket& recvPacket);
        void HandleChannelDeclineInvite(WorldPacket& recvPacket);
        void HandleChannelDisplayListQuery(WorldPackets::Channel::ChannelDisplayList& recvPacket);
        //void HandleGetChannelMemberCount(WorldPacket& recvPacket);
        void HandleSetChannelWatch(WorldPacket& recvPacket);

        void HandleCompleteCinematic(WorldPackets::Misc::CompleteCinematic& packet);
        void HandleCompleteMovie(WorldPackets::Misc::CompleteMovie& packet);    
        void HandleNextCinematicCamera(WorldPackets::Misc::NextCinematicCamera& packet);

        void HandlePageTextQueryOpcode(WorldPackets::Query::QueryPageText& packet);

        void HandleTutorialFlag (WorldPackets::Misc::TutorialSetFlag& packet);
        void HandleTutorialClear(WorldPackets::Misc::TutorialClear& packet);
        void HandleTutorialReset(WorldPackets::Misc::TutorialReset& packet);

        void HandlePlayerChoiceResponse(WorldPackets::Spells::ChoiceResponse& packet);

        //Pet
        void HandlePetAction(WorldPackets::Pet::PetAction& packet);
        void HandlePetStopAttack(WorldPackets::Pet::PetStopAttack& packet);
        void HandlePetActionHelper(Unit* pet, ObjectGuid guid1, uint32 spellid, uint16 flag, ObjectGuid guid2, float x, float y, float z);
        void HandlePetNameQuery(WorldPackets::Pet::PetNameQuery& packet);
        void HandlePetSetAction(WorldPackets::Pet::PetSetAction& packet);
        void HandlePetAbandon(WorldPackets::Pet::PetAbandon& packet);
        void HandlePetRename(WorldPackets::Pet::PetRename& packet);
        void HandlePetCancelAuraOpcode(WorldPackets::Spells::PetCancelAura& packet);
        void HandlePetSpellAutocastOpcode(WorldPackets::Pet::PetSpellAutocast& packet);
        void HandlePetCastSpellOpcode(WorldPackets::Pet::PetCastSpell& packet);
        void HandlePetLearnTalent(WorldPacket& recvPacket);
        void HandleLearnPreviewTalentsPet(WorldPacket& recvPacket);

        void HandleSetActionBarToggles(WorldPackets::Misc::SetActionBarToggles& packet);

        void HandleCharRenameOpcode(WorldPackets::Character::CharacterRenameRequest& packet);
        void HandleChangePlayerNameOpcodeCallBack(PreparedQueryResult result, std::string const& newName);
        void SendRenameResult(uint8 result, ObjectGuid guid = ObjectGuid::Empty, std::string name = "");
        void HandleSetPlayerDeclinedNames(WorldPackets::Character::SetPlayerDeclinedNames& packet);
        void HandeSetTalentSpecialization(WorldPackets::Talent::SetPrimaryTalentTree& packet);

        void HandleTotemDestroyed(WorldPackets::Totem::TotemDestroyed& packet);
        void HandleDismissCritter(WorldPackets::Pet::DismissCritter& packet);

        //Battleground
        void HandleBattlemasterHelloOpcode(WorldPacket& recvData);
        void HandleBattlemasterJoinOpcode(WorldPackets::Battleground::BattlemasterJoin& packet);
        void HandlePVPLogDataOpcode(WorldPackets::Battleground::PVPLogDataRequest& packet);
        void HandleBattleFieldPortOpcode(WorldPackets::Battleground::BattlefieldPort& packet);
        void HandleBattlefieldListOpcode(WorldPackets::Battleground::BattlefieldListRequest& packet);
        void HandleBattlefieldLeaveOpcode(WorldPackets::Battleground::BattlefieldLeave& packet);
        void HandleBattlemasterJoinArena(WorldPackets::Battleground::BattlemasterJoinArena& packet);
        void HandleBattlemasterJoinRated(WorldPackets::Battleground::BattlemasterJoinRated& packet);
        void HandleReportPvPAFK(WorldPackets::Battleground::ReportPvPPlayerAFK& packet);
        //void HandleRequestRatedBgInfo(WorldPacket& recvData); 
        void HandleRequestPvpOptions(WorldPackets::Battleground::RequestPvpOptionsEnabled& packet);
        void HandleRequestPvpReward(WorldPackets::Battleground::RequestPVPRewards& packet);
        void HandleRequestRatedBgStats(WorldPackets::Battleground::RequestRatedBgStats& packet);
        void HandleRequestConquestFormulaConstants(WorldPackets::Battleground::RequestConquestFormulaConstants& packet);

        void HandleWardenDataOpcode(WorldPacket& recvData);
        void HandleWorldTeleportOpcode(WorldPacket& recvData);
        void HandleMinimapPingOpcode(WorldPackets::Party::MinimapPingClient& packet);
        void HandleRandomRollOpcode(WorldPackets::Party::RandomRollClient& packet);
        void HandleFarSightOpcode(WorldPackets::Misc::FarSight& packet);
        void HandleSetDungeonDifficultyOpcode(WorldPackets::Instance::SetDungeonDifficulty& packet);
        void HandleSetRaidDifficultyOpcode(WorldPackets::Instance::SetRaidDifficulty& packet);
        void HandleMoveSetCanFlyAckOpcode(WorldPackets::Movement::MoveSetCanFlyAck& packet);
        void HandleRealmSplitOpcode(WorldPacket& recvData);
        void HandleTimeSyncResp(WorldPackets::Misc::TimeSyncResponse& packet);
        void HandleWhoisOpcode(WorldPackets::Who::WhoIsRequest& recvData);
        void HandleResetInstancesOpcode(WorldPackets::Instance::ResetInstances& recvData);
        void HandleHearthAndResurrect(WorldPacket& recvData);
        void HandleInstanceLockResponse(WorldPackets::Instance::InstanceLockResponse& recvPacket);

        // Battlefield
        void SendBfInvitePlayerToWar(ObjectGuid guid, uint32 zoneId, uint32 time);
        void SendBfInvitePlayerToQueue(ObjectGuid guid);
        void SendBfQueueInviteResponse(ObjectGuid guid, uint32 zoneId, bool canQueue = true, bool full = false);
        void SendBfEntered(ObjectGuid guid);
        void SendBfLeaveMessage(ObjectGuid guid, BFLeaveReason reason = BF_LEAVE_REASON_EXITED);
        void HandleBfQueueInviteResponse(WorldPackets::Battleground::BattlefieldMgrQueueInviteResponse& packet);
        void HandleBfEntryInviteResponse(WorldPackets::Battleground::BattlefieldMgrEntryInviteResponse& packet);
        void HandleBfQueueRequest(WorldPackets::Battleground::BattlefieldMgrQueueRequest& packet);
        void HandleBfExitRequest(WorldPackets::Battleground::BattlefieldMgrExitRequest& packet);

        // Looking for Dungeon/Raid
        void HandleLfgGetLockInfoOpcode(WorldPackets::LFG::DFGetSystemInfo& packet);
        void SendLfgPlayerLockInfo();
        void SendLfgPartyLockInfo();
        void HandleLfgJoinOpcode(WorldPackets::LFG::LFGJoin& packet);
        void HandleLfgLeaveOpcode(WorldPackets::LFG::LFGLeave& packet);
        void HandleLfgSetRolesOpcode(WorldPackets::LFG::DFSetRoles& packet);
        void HandleLfgProposalResultOpcode(WorldPackets::LFG::DFProposalResponse& packet);
        void HandleLfgSetBootVoteOpcode(WorldPackets::LFG::DFBootPlayerVote& packet);
        void HandleLfgTeleportOpcode(WorldPackets::LFG::DFTeleport& packet);
        void HandleLfrJoinOpcode(WorldPacket& recvData);
        void HandleLfrLeaveOpcode(WorldPacket& recvData);
        void HandleLfgGetStatus(WorldPackets::LFG::DFGetJoinStatus& packet);
        void HandleSetLfgBonusFactionId(WorldPackets::LFG::SetLfgBonusFactionId& packet);

        void SendLfgUpdateStatus(lfg::LfgUpdateType updateType, lfg::PlayerQueueData const& queueData);
        void SendLfgRoleChosen(ObjectGuid guid, uint8 roles);
        void SendLfgRoleCheckUpdate(lfg::LfgRoleCheck const& pRoleCheck);
        void SendLfgJoinResult(uint32 queueId, lfg::LfgJoinResultData const& joinData);
        void SendLfgQueueStatus(lfg::LfgQueueStatusData const& queueData);
        void SendLfgPlayerReward(lfg::LfgPlayerRewardData const& lfgPlayerRewardData);
        void SendLfgBootProposalUpdate(lfg::LfgPlayerBoot const& boot);
        void SendLfgUpdateProposal(lfg::LfgProposal const& proposal);
        void SendLfgDisabled();
        void SendLfgOfferContinue(uint32 dungeonEntry);
        void SendLfgTeleportError(uint8 err);

        void HandleAreaSpiritHealerQueryOpcode(WorldPackets::Battleground::AreaSpiritHealerQuery& packet);
        void HandleAreaSpiritHealerQueueOpcode(WorldPackets::Battleground::AreaSpiritHealerQueue& packet);
        void HandleCancelMountAuraOpcode(WorldPackets::Combat::CancelMountAura& packet);
        void HandleSelfResOpcode(WorldPackets::Combat::SelfRes& packet);
        //void HandleComplainOpcode(WorldPacket& recvData);
        void HandleRequestPetInfoOpcode(WorldPackets::Pet::RequestPetInfo& packet);

        // Socket gem
        void HandleSocketOpcode(WorldPackets::Item::SocketGems& packet);

        void HandleCancelTempEnchantmentOpcode(WorldPackets::Combat::CancelTempEnchantment& packet);

        void HandleItemRefundInfoRequest(WorldPackets::Item::GetItemPurchaseData& packet);
        void HandleItemRefund(WorldPackets::Item::ItemPurchaseRefund& packet);
        void SendItemExpirePurchaseRefund(ObjectGuid itemGuid);

        void HandleChannelVoiceOnOpcode(WorldPackets::Channel::ChannelVoiceOn& recvData);
        void HandleVoiceSessionEnableOpcode(WorldPackets::VoiceChat::VoiceSessionEnable& packet);
        void HandleSetActiveVoiceChannel(WorldPacket& recvData);
        void HandleSetTaxiBenchmarkOpcode(WorldPackets::Misc::SetTaxiBenchmarkMode& packet);
        void HandleSetAdvancedCombatLogging(WorldPackets::Combat::SetAdvancedCombatLogging& packet);

        // Guild Bank
        void HandleGuildPermissions(WorldPackets::Guild::GuildPermissionsQuery& packet);
        void HandleGuildBankMoneyWithdrawn(WorldPackets::Guild::GuildBankRemainingWithdrawMoneyQuery& packet);
        void HandleGuildBankerActivate(WorldPackets::Guild::GuildBankActivate& packet);
        void HandleGuildBankQueryTab(WorldPackets::Guild::GuildBankQueryTab& packet);
        void HandleGuildBankLogQuery(WorldPackets::Guild::GuildBankLogQuery& packet);
        void HandleGuildBankDepositMoney(WorldPackets::Guild::GuildBankDepositMoney& packet);
        void HandleGuildBankWithdrawMoney(WorldPackets::Guild::GuildBankWithdrawMoney& packet);
        void HandleGuildBankSwapItems(WorldPackets::Guild::GuildBankSwapItems& packet);
        void HandleGuildBankTabNote(WorldPackets::Guild::GuildBankSetTabText& packet);

        void HandleGuildBankUpdateTab(WorldPackets::Guild::GuildBankUpdateTab& packet);
        void HandleGuildBankBuyTab(WorldPackets::Guild::GuildBankBuyTab& packet);
        void HandleQueryGuildBankTabText(WorldPackets::Guild::GuildBankTextQuery& packet);
        void HandleGuildQueryXPOpcode(WorldPackets::Guild::QueryGuildXP& packet);

        // Refer-a-Friend
        void HandleGrantLevel(WorldPackets::ReferAFriend::GrantLevel& packet);
        void HandleAcceptGrantLevel(WorldPackets::ReferAFriend::AcceptGrantLevel& packet);

        // Calendar
        void HandleCalendarGetCalendar(WorldPackets::Calendar::CalendarGetCalendar& recvData);
        void HandleCalendarGetEvent(WorldPackets::Calendar::CalendarGetEvent& recvData);
        void HandleCalendarGuildFilter(WorldPackets::Calendar::CalendarGuildFilter& recvData);
        void HandleCalendarAddEvent(WorldPackets::Calendar::CalendarAddEvent& packet);
        void HandleCalendarUpdateEvent(WorldPackets::Calendar::CalendarUpdateEvent& packet);
        void HandleCalendarRemoveEvent(WorldPackets::Calendar::CalendarRemoveEvent& recvData);
        void HandleCalendarCopyEvent(WorldPackets::Calendar::CalendarCopyEvent& recvData);
        void HandleCalendarEventInvite(WorldPackets::Calendar::CalendarInvite& recvData);
        void HandleCalendarEventRsvp(WorldPackets::Calendar::CalendarRSVP& recvData);
        void HandleCalendarEventRemoveInvite(WorldPackets::Calendar::CalendarRemoveInvite& recvData);
        void HandleCalendarEventStatus(WorldPackets::Calendar::CalendarStatus& recvData);
        void HandleCalendarEventModeratorStatus(WorldPackets::Calendar::CalendarModeratorStatusQuery& recvData);
        void HandleCalendarComplain(WorldPackets::Calendar::CalendarComplain& recvData);
        void HandleCalendarGetNumPending(WorldPackets::Calendar::CalendarGetNumPending& recvData);
        void HandleCalendarEventSignup(WorldPackets::Calendar::CalendarEventSignUp& recvData);

        void SendCalendarRaidLockout(InstanceSave const* save, bool add);
        void SendCalendarRaidLockoutUpdated(InstanceSave const* save);
        void HandleSetSavedInstanceExtend(WorldPackets::Instance::SetSavedInstanceExtend& recvData);

        // Void Storage
        void HandleVoidStorageUnlock(WorldPackets::VoidStorage::VoidStorageUnlock& packet);
        void HandleVoidStorageQuery(WorldPackets::VoidStorage::VoidStorageQuery& packet);
        void HandleVoidStorageTransfer(WorldPackets::VoidStorage::VoidStorageTransfer& packet);
        void HandleVoidSwapItem(WorldPackets::VoidStorage::VoidSwapItem& packet);
        void SendVoidStorageTransferResult(VoidTransferError result);

        // Transmogrification
        void HandleTransmogrifyItems(WorldPackets::Transmogrification::TransmogrifyItems& recvData);

        // Reforge
        void HandleReforgeItemOpcode(WorldPackets::Item::ReforgeItem& packet);
        void SendReforgeResult(bool success);

        // BlackMarket
        void HandleBlackMarketHelloOpcode(WorldPackets::BlackMarket::BlackMarketOpen& recvData);
        void SendBlackMarketHello(ObjectGuid NpcGUID, bool Open);
        void HandleBlackMarketRequestItemOpcode(WorldPackets::BlackMarket::BlackMarketRequestItems& recvData);
        void SendBlackMarketRequestItemsResult();

        void HandleBlackMarketBidOnItem(WorldPackets::BlackMarket::BlackMarketBidOnItem& recvData);
        void SendBlackMarketBidOnItemResult(uint32 auctionId, uint32 ItemID, BlackMarketError result);

        // Miscellaneous
        void HandleSpellClick(WorldPackets::Spells::SpellClick& packet);
        void HandleMirrorImageDataRequest(WorldPackets::Spells::GetMirrorImageData& packet);
        void HandleAlterAppearance(WorldPackets::Character::AlterApperance& packet);
        void SendBarberShopResult(BarberShopResult result);
        void HandleRemoveGlyph(WorldPacket& recvData);
        void HandleCharCustomize(WorldPackets::Character::CharCustomize& packet);
        void HandleQueryInspectAchievements(WorldPackets::Inspect::QueryInspectAchievements& packet);
        void HandleGuildAchievementProgressQuery(WorldPackets::Misc::GuildAchievementProgressQuery& packet);
        void HandleEquipmentSetSave(WorldPackets::EquipmentSet::SaveEquipmentSet& packet);
        void HandleEquipmentSetDelete(WorldPackets::EquipmentSet::DeleteEquipmentSet& recvData);
        void HandleEquipmentSetUse(WorldPackets::EquipmentSet::UseEquipmentSet& packet);
        void HandleWorldStateUITimerUpdate(WorldPackets::Misc::UITimeRequest& packet);
        void HandleReadyForAccountDataTimes(WorldPackets::Misc::ReadyForAccountDataTimes& packet);
        void HandleQuestNPCQuery(WorldPackets::Query::QueryQuestCompletionNPCs& packet);
        void HandleQuestPOIQuery(WorldPackets::Query::QuestPOIQuery& packet);
        void HandleEjectPassenger(WorldPackets::Vehicle::EjectPassenger& data);
        void HandleEnterPlayerVehicle(WorldPackets::Vehicle::PlayerVehicleEnter& recvData);
        void HandleMissileTrajectoryCollision(WorldPackets::Spells::MissileTrajectoryCollision& packet);
        void HandleRequestHotfix(WorldPackets::Misc::RequestHotfix& packet);
        void HandleUpdateMissileTrajectory(WorldPacket& recvPacket);
        void HandleViolenceLevel(WorldPackets::Misc::ViolenceLevel& packet);
        void HandleObjectUpdateFailedOpcode(WorldPackets::Misc::ObjectUpdateFailed& packet);
        void HandleSelectFactionOpcode(WorldPackets::Misc::SelectFaction& packet);
        void HandleRequestCategoryCooldowns(WorldPackets::Spells::RequestCategoryCooldowns& packet);
        void HandleRequestCemeteryList(WorldPackets::Misc::RequestCemeteryList& packet);
        void HandleDiscardedTimeSyncAcks(WorldPackets::Misc::DiscardedTimeSyncAcks& packet);
        void HandleLogStreamingError(WorldPackets::Misc::LogStreamingError& packet);
        void HandleShowTradeSkill(WorldPackets::Misc::ShowTradeSkill& packet);
        void SendStreamingMovie();

        void SendBroadcastTextDb2Reply(uint32 entry, ByteBuffer& buffer);
        void SendItemDb2Reply(uint32 entry, ByteBuffer& buffer);
        void SendItemSparseDb2Reply(uint32 entry, ByteBuffer& buffer);

        int32 HandleEnableNagleAlgorithm();

        // Compact Unit Frames (4.x)
        void HandleSaveCUFProfiles(WorldPacket& recvPacket);
        void SendLoadCUFProfiles();

        // Battle Pet
        void HandlePetBattleStartPvpMatchmaking(WorldPackets::BattlePet::PetBattleStartPvpMatchmaking& packet);
        void HandlePetBattleStopPvpMatchmaking(WorldPacket& recvData);
        void HandleBattlePetModifyName(WorldPackets::BattlePet::BattlePetModifyName& packet);
        void HandleBattlePetDelete(WorldPackets::BattlePet::BattlePetDeletePet& packet);
        void HandleBattlePetSetBattleSlot(WorldPackets::BattlePet::BattlePetSetBattleSlot& packet);
        void HandleBattlePetSetFlags(WorldPackets::BattlePet::BattlePetSetFlags& packet);
        void HandleBattlePetQueryName(WorldPackets::BattlePet::QueryBattlePetName& packet);
        void HandleBattlePetRequestJournal(WorldPackets::BattlePet::BattlePetRequestJournal& packet);
        void HandleBattlePetSummonCompanion(WorldPackets::BattlePet::BattlePetSummon& packet);
        void HandleBattlePetCage(WorldPackets::BattlePet::CageBattlePet& packet);
        void HandleBattlePetLearn(WorldPackets::BattlePet::BattlePetLearn& packet);

        // Pet Battle
        void HandlePetBattleInput(WorldPackets::BattlePet::PetBattleInput& packet);
        void HandlePetBattleRequestWild(WorldPackets::BattlePet::PetBattleRequestWild& packet);
        void HandlePetBattleSetFrontPet(WorldPackets::BattlePet::PetBattleSetFrontPet& packet);

        void SendPetBattleRequestFailed(uint8 reason);

        // Titles
        void HandleSetTitleOpcode(WorldPackets::Misc::SetTitle& packet);
        void SendTitleEarned(uint32 TitleIndex);
        void SendTitleLost(uint32 TitleIndex);

        // BattlePay
        void HandleBattlePayGetProductList(WorldPackets::BattlePay::BattlePayGetProductList& packet);
        void HandleBattlePayGetPurchaseList(WorldPackets::BattlePay::BattlePayGetPurchaseList& packet);
        void HandleBattlePayStartPurchase(WorldPackets::BattlePay::BattlePayStartPurchase& packet);
        void HandleBattlePayConfirmPurchaseResponse(WorldPackets::BattlePay::BattlePayConfirmPurchaseResponse& packet);
        void HandleBattlePayCharBoost(WorldPackets::BattlePay::BattlePayCharBoost& packet);

        // Scene
        void HandleSceneTriggerEvent(WorldPackets::Scene::SceneTriggerEvent& recvData);
        void HandleScenePlaybackComplete(WorldPackets::Scene::ScenePlaybackComplete& recvData);
        void HandleScenePlaybackCanceled(WorldPackets::Scene::ScenePlaybackCanceled& recvData);

        // Scenario
        void HandleQueryScenarioPOI(WorldPackets::Scene::QueryScenarioPOI& packet);

        // Challenge Mode
        void HandleGetChallengeModeRewards(WorldPackets::ChallengeMode::GetChallengeModeRewards& packet);
        void HandleChallengeModeRequestLeaders(WorldPackets::ChallengeMode::ChallengeModeRequestLeaders& packet);
        void HandleChallengeModeRequestMapStats(WorldPackets::ChallengeMode::ChallengeModeRequestMapStats& packet);
        void HandleResetChallengeMode(WorldPackets::ChallengeMode::ResetChallengeMode& packet);
        void SendChallengeModeMapStatsUpdate(uint32 mapId);

        // chat

        bool ChannelCheck(std::string channel);

        // bot
        bool HandleSocketClosed();

        union ConnectToKey
        {
            struct
            {
                uint64 AccountId : 32;
                uint64 ConnectionType : 1;
                uint64 Key : 31;
            } Fields;

            uint64 Raw;
        };

    public:
        QueryCallbackProcessor& GetQueryProcessor() { return _queryProcessor; }
        TransactionCallback& AddTransactionCallback(TransactionCallback&& callback);
        SQLQueryHolderCallback& AddQueryHolderCallback(SQLQueryHolderCallback&& callback);

    private:
        void ProcessQueryCallbacks();

        QueryCallbackProcessor _queryProcessor;
        AsyncCallbackProcessor<TransactionCallback> _transactionCallbacks;
        AsyncCallbackProcessor<SQLQueryHolderCallback> _queryHolderProcessor;

    friend class World;
    protected:
        class DosProtection
        {
            friend class World;
            public:
                DosProtection(WorldSession* s) : Session(s), _policy((Policy)sWorld->getIntConfig(CONFIG_PACKET_SPOOF_POLICY)) { }
                bool EvaluateOpcode(WorldPacket& p, time_t time) const;
                void AllowOpcode(uint16 opcode, bool allow) { _isOpcodeAllowed[opcode] = allow; }
            protected:
                enum Policy
                {
                    POLICY_LOG,
                    POLICY_KICK,
                    POLICY_BAN,
                };

                bool IsOpcodeAllowed(uint16 opcode) const
                {
                    OpcodeStatusMap::const_iterator itr = _isOpcodeAllowed.find(opcode);
                    if (itr == _isOpcodeAllowed.end())
                        return true;    // No presence in the map indicates this is the first time the opcode was sent this session, so allow

                    return itr->second;
                }

                uint32 GetMaxPacketCounterAllowed(uint16 opcode) const;

                WorldSession* Session;

            private:
                typedef std::unordered_map<uint16, bool> OpcodeStatusMap;
                OpcodeStatusMap _isOpcodeAllowed; // could be bool array, but wouldn't be practical for game versions with non-linear opcodes
                Policy _policy;
                typedef std::unordered_map<uint16, PacketCounter> PacketThrottlingMap;
                // mark this member as "mutable" so it can be modified even in const functions
                mutable PacketThrottlingMap _PacketThrottlingMap;
                DosProtection(DosProtection const& right) = delete;
                DosProtection& operator=(DosProtection const& right) = delete;
        } AntiDOS;

    private:
        // private trade methods
        void moveItems(Item* myItems[], Item* hisItems[]);

        // logging helper
        void LogUnexpectedOpcode(WorldPacket* packet, const char* status, const char *reason);

        // EnumData helpers
        bool IsLegitCharacterForAccount(ObjectGuid guid)
        {
            return _legitCharacters.find(guid) != _legitCharacters.end();
        }

        // this stores the GUIDs of the characters who can login
        // characters who failed on Player::BuildEnumData shouldn't login
        GuidSet _legitCharacters;

        uint32 m_GUIDLow;                                   // set loggined or recently logout player (while m_playerRecentlyLogout set)
        Player* _player;
        GameClient* _gameClient;
        std::shared_ptr<WorldSocket> m_Socket;
        std::string m_Address;

        AccountTypes _security;
        uint32 _accountId;
        std::string _accountName;
        rbac::RBACData* _RBACData;
        uint8 m_expansion;

        CharacterBooster* m_charBooster;

        typedef std::list<AddonInfo> AddonsList;

        // Warden
        Warden* _warden;                                    // Remains NULL if Warden system is not enabled by config

        time_t _logoutTime;
        bool m_inQueue;                                     // session wait in auth.queue
        bool m_playerLoading;                               // code processed in LoginPlayer
        bool m_playerLogout;                                // code processed in LogoutPlayer
        bool m_playerRecentlyLogout;
        bool m_playerSave;
        LocaleConstant m_sessionDbcLocale;
        LocaleConstant m_sessionDbLocaleIndex;
        uint32 m_latency;
        uint32 m_flags;
        std::unique_ptr<boost::circular_buffer<std::pair<int64, uint32>>> _timeSyncClockDeltaQueue; // first member: clockDelta. Second member: latency of the packet exchange that was used to compute that clockDelta.
        int64 _timeSyncClockDelta;
        std::map<uint32, uint32> _pendingTimeSyncRequests; // key: counter. value: server time when packet with that counter was sent.
        TimeValue m_firstCancelModSpeedNoContorlAurasPacket;
        TimeValue m_lastCancelModSpeedNoContorlAurasPacket;
        AccountData m_accountData[NUM_ACCOUNT_DATA_TYPES];
        uint32 m_Tutorials[MAX_ACCOUNT_TUTORIAL_VALUES];
        bool   m_TutorialsChanged;
        AddonsList m_addonsList;
        std::vector<std::string> _registeredAddonPrefixes;
        bool _filterAddonMessages;
        uint32 recruiterId;
        bool isRecruiter;
        bool m_hasBoost;
        bool _isBot;
        LockedQueue<WorldPacket*> _recvQueue;
        uint32 expireTime;
        time_t timeLastWhoCommand;

        std::unique_ptr<AccountAchievementMgr> _achievementMgr;

        MuteInfo _mute;
        uint32 m_currentVendorEntry;
};
#endif
/// @}
