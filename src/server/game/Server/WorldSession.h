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
        WorldSession(uint32 id, std::shared_ptr<WorldSocket> sock, AccountTypes sec, uint8 expansion, time_t mute_time, LocaleConstant locale, uint32 recruiter, uint32 flags, bool isARecruiter, bool hasBoost, bool isBot = false);
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
        Player* GetPlayer() const { return _player; }
        std::string const& GetPlayerName() const;
        std::string GetPlayerInfo() const;

        uint32 GetCurrentVendor() const { return m_currentVendorEntry; }
        void SetCurrentVendor(uint32 vendorEntry) { m_currentVendorEntry = vendorEntry; }

        ObjectGuid GetGUID() const;
        uint32 GetGuidLow() const;
        void SetSecurity(AccountTypes security) { _security = security; }
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
        void HandleCharDeleteOpcode(WorldPacket& recvPacket);
        void HandleCharCreateOpcode(WorldPacket& recvPacket);
        void HandlePlayerLoginOpcode(WorldPackets::Character::PlayerLogin& packet);
        void HandleLoadScreenOpcode(WorldPacket& recvPacket);
        void HandleCharEnum(PreparedQueryResult result);
        void HandlePlayerLogin(LoginQueryHolder const& holder);
        void HandleCharFactionOrRaceChange(WorldPacket& recvData);
        void HandleRandomizeCharNameOpcode(WorldPacket& recvData);
        void HandleReorderCharacters(WorldPacket& recvData);
        void HandleOpeningCinematic(WorldPackets::Misc::OpeningCinematic& packet);

        // played time
        void HandlePlayedTime(WorldPacket& recvPacket);

        // new
        void HandleMoveUnRootAck(WorldPacket& recvPacket);
        void HandleMoveRootAck(WorldPacket& recvPacket);
        void HandleLookingForGroup(WorldPacket& recvPacket);
        void HandleReturnToGraveyard(WorldPacket& recvPacket);

        // new inspect
        void HandleInspectOpcode(WorldPacket& recvPacket);

        // new party stats
        void HandleInspectHonorStatsOpcode(WorldPacket& recvPacket);
        void HandleInspectRatedBGStatsOpcode(WorldPacket& recvPacket);

        void HandleMoveWaterWalkAck(WorldPacket& recvPacket);
        void HandleFeatherFallAck(WorldPacket& recvData);

        void HandleMoveHoverAck(WorldPacket& recvData);
        void HandleMoveGravityAck(WorldPacket& recvData);

        void HandleMountSpecialAnimOpcode(WorldPacket& recvdata);

        // character view
        void HandleShowingHelmOpcode(WorldPackets::Character::ShowingHelm& packet);
        void HandleShowingCloakOpcode(WorldPackets::Character::ShowingCloak& packet);

        // repair
        void HandleRepairItemOpcode(WorldPacket& recvPacket);

        // Knockback
        void HandleMoveKnockBackAck(WorldPacket& recvPacket);

        void HandleMoveTeleportAck(WorldPacket& recvPacket);
        void HandleForceSpeedChangeAck(WorldPacket& recvData);
        void HandleSetCollisionHeightAck(WorldPacket& recvPacket);
        void HandleMovementForceAck(WorldPacket& recvPacket);
        void HandleMoveSetCanTurnWhileFallingAck(WorldPacket& recvData);

        void HandlePingOpcode(WorldPacket& recvPacket);
        void HandleAuthSessionOpcode(WorldPacket& recvPacket);
        void HandleRepopRequestOpcode(WorldPacket& recvPacket);
        void HandleAutostoreLootItemOpcode(WorldPacket& recvPacket);
        void HandleLootMoneyOpcode(WorldPacket& recvPacket);
        void HandleLootOpcode(WorldPacket& recvPacket);
        void HandleLootReleaseOpcode(WorldPacket& recvPacket);
        void HandleLootMasterGiveOpcode(WorldPacket& recvPacket);
        void HandleWhoOpcode(WorldPacket& recvPacket);
        void HandleLogoutRequestOpcode(WorldPacket& recvPacket);
        void HandlePlayerLogoutOpcode(WorldPacket& recvPacket);
        void HandleLogoutCancelOpcode(WorldPacket& recvPacket);
        void HandleChangeCurrencyFlags(WorldPacket& recvPacket);

        // GM Ticket opcodes
        void HandleGMTicketCreateOpcode(WorldPacket& recvPacket);
        void HandleGMTicketUpdateOpcode(WorldPacket& recvPacket);
        void HandleGMTicketDeleteOpcode(WorldPacket& recvPacket);
        void HandleGMTicketGetTicketOpcode(WorldPacket& recvPacket);
        void HandleGMTicketSystemStatusOpcode(WorldPacket& recvPacket);
        void HandleGMTicketCaseStatusOpcode(WorldPacket& recvPacket);
        void HandleGMSurveySubmit(WorldPacket& recvPacket);
        void HandleReportLag(WorldPacket& recvPacket);
        void HandleGMResponseResolve(WorldPacket& recvPacket);

        // Gm Bug Ticket
        void HandleSubmitBugOpcode(WorldPacket& recvPacket);

        void HandleTogglePvP(WorldPackets::Misc::TogglePvP& togglePvP);
        void HandleSetPvP(WorldPackets::Misc::SetPvP& setPvP);

        void HandleZoneUpdateOpcode(WorldPacket& recvPacket);
        void HandleSetSelectionOpcode(WorldPacket& recvPacket);
        void HandleStandStateChangeOpcode(WorldPacket& recvPacket);
        void HandleEmoteOpcode(WorldPacket& recvPacket);
        void HandleContactListOpcode(WorldPacket& recvPacket);
        void HandleAddFriendOpcode(WorldPacket& recvPacket);
        void HandleAddFriendOpcodeCallBack(PreparedQueryResult result, std::string const& friendNote);
        void HandleDelFriendOpcode(WorldPacket& recvPacket);
        void HandleAddIgnoreOpcode(WorldPacket& recvPacket);
        void HandleAddIgnoreOpcodeCallBack(PreparedQueryResult result);
        void HandleDelIgnoreOpcode(WorldPacket& recvPacket);
        void HandleSetContactNotesOpcode(WorldPacket& recvPacket);
        void HandleBugOpcode(WorldPacket& recvPacket);

        void HandleAreaTriggerOpcode(WorldPacket& recvPacket);

        void HandleSetFactionAtWar(WorldPacket& recvData);
        void HandleSetFactionNotAtWar(WorldPacket& recvData);
        void HandleSetFactionCheat(WorldPacket& recvData);
        void HandleSetWatchedFactionOpcode(WorldPacket& recvData);
        void HandleSetFactionInactiveOpcode(WorldPacket& recvData);
        void HandleRequestForcedReactionsOpcode(WorldPacket& recvData);

        void HandleUpdateAccountData(WorldPacket& recvPacket);
        void HandleRequestAccountData(WorldPacket& recvPacket);
        void HandleSetActionButtonOpcode(WorldPacket& recvPacket);

        void HandleGameObjectUseOpcode(WorldPacket& recPacket);
        void HandleMeetingStoneInfo(WorldPacket& recPacket);
        void HandleGameobjectReportUse(WorldPacket& recvPacket);

        void HandleNameQueryOpcode(WorldPacket& recvPacket);
        void HandleRealmNameQueryOpcode(WorldPacket& recvPacket);

        void HandleQueryTimeOpcode(WorldPacket& recvPacket);
        void HandleQueryCountdownTimer(WorldPacket& recvPacket);

        void HandleCreatureQueryOpcode(WorldPacket& recvPacket);

        void HandleGameObjectQueryOpcode(WorldPackets::Query::QueryGameObject& packet);

        void HandleMoveWorldportAckOpcode(WorldPacket& recvPacket);
        void HandleMoveWorldportAck();

        void HandleMovementOpcodes(WorldPacket& recvPacket);
        void HandleSetActiveMoverOpcode(WorldPacket& recvData);
        void HandleMoveNotActiveMover(WorldPacket& recvData);
        void HandleDismissControlledVehicle(WorldPacket& recvData);
        void HandleRequestVehicleExit(WorldPacket& recvData);
        void HandleMoveSetVehicleRecAck(WorldPacket& recvData);
        void HandleChangeSeatsOnControlledVehicle(WorldPacket& recvData);
        void HandleMoveTimeSkippedOpcode(WorldPacket& recvData);

        void HandleRequestRaidInfoOpcode(WorldPacket& recvData);

        void HandleBattlefieldStatusOpcode(WorldPacket& recvData);
        void HandleBattleMasterHelloOpcode(WorldPacket& recvData);

        void HandleGroupInviteOpcode(WorldPacket& recvPacket);
        //void HandleGroupCancelOpcode(WorldPacket& recvPacket);
        void HandleGroupInviteResponseOpcode(WorldPacket& recvPacket);
        //void HandleGroupUninviteOpcode(WorldPacket& recvPacket);
        void HandleGroupUninviteGuidOpcode(WorldPacket& recvPacket);
        void HandleGroupSetLeaderOpcode(WorldPacket& recvPacket);
        void HandleGroupSetRolesOpcode(WorldPacket& recvData);
        void HandleGroupDisbandOpcode(WorldPacket& recvPacket);
        void HandleOptOutOfLootOpcode(WorldPacket& recvData);
        void HandleLootMethodOpcode(WorldPacket& recvPacket);
        void HandleLootRoll(WorldPacket& recvData);
        void HandleRequestPartyMemberStatsOpcode(WorldPacket& recvData);
        void HandleRaidTargetUpdateOpcode(WorldPacket& recvData);
        void HandleRaidReadyCheckOpcode(WorldPacket& recvData);
        void HandleRaidReadyCheckConfirmOpcode(WorldPacket& recvData);
        void HandleRaidReadyCheckFinishedOpcode(WorldPacket& recvData);
        void HandleGroupRaidConvertOpcode(WorldPacket& recvData);
        void HandleGroupRequestJoinUpdates(WorldPacket& recvData);
        void HandleGroupChangeSubGroupOpcode(WorldPacket& recvData);
        void HandleGroupSwapSubGroupOpcode(WorldPacket& recvData);
        void HandleGroupAssistantLeaderOpcode(WorldPacket& recvData);
        void HandleGroupEveryoneIsAssistantOpcode(WorldPacket& recvData);
        void HandlePartyAssignmentOpcode(WorldPacket& recvData);
        void HandleGroupInitiatePollRole(WorldPacket& recvData);
        void HandleClearRaidMarkerOpcode(WorldPacket& recvData);

        void HandlePetitionBuyOpcode(WorldPacket& recvData);
        void HandlePetitionShowSignOpcode(WorldPacket& recvData);
        void HandlePetitionQueryOpcode(WorldPacket& recvData);
        void HandlePetitionRenameOpcode(WorldPacket& recvData);
        void HandlePetitionSignOpcode(WorldPacket& recvData);
        void HandlePetitionDeclineOpcode(WorldPacket& recvData);
        void HandleOfferPetitionOpcode(WorldPacket& recvData);
        void HandleTurnInPetitionOpcode(WorldPacket& recvData);
        void SendPetitionSignResults(ObjectGuid petitionGuid, ObjectGuid playerGuid, uint8 result);

        void HandleGuildQueryOpcode(WorldPacket& recvPacket);
        void HandleGuildInviteOpcode(WorldPacket& recvPacket);
        void HandleGuildRemoveOpcode(WorldPacket& recvPacket);
        void HandleGuildAcceptOpcode(WorldPacket& recvPacket);
        void HandleGuildDeclineOpcode(WorldPacket& recvPacket);
        void HandleGuildEventLogQueryOpcode(WorldPacket& recvPacket);
        void HandleGuildRosterOpcode(WorldPacket& recvPacket);
        void HandleGuildRewardsQueryOpcode(WorldPacket& recvPacket);
        void HandleGuildPromoteOpcode(WorldPacket& recvPacket);
        void HandleGuildDemoteOpcode(WorldPacket& recvPacket);
        void HandleGuildAssignRankOpcode(WorldPacket& recvPacket);
        void HandleGuildLeaveOpcode(WorldPacket& recvPacket);
        void HandleGuildDisbandOpcode(WorldPacket& recvPacket);
        void HandleGuildSetAchievementTracking(WorldPacket& recvPacket);
        void HandleGuildSetGuildMaster(WorldPacket& recvPacket);
        void HandleGuildReplaceGuildMaster(WorldPacket& recvPacket);
        void HandleGuildMOTDOpcode(WorldPacket& recvPacket);
        void HandleGuildNewsUpdateStickyOpcode(WorldPacket& recvPacket);
        void HandleGuildSetNoteOpcode(WorldPacket& recvPacket);
        void HandleGuildQueryRanksOpcode(WorldPacket& recvPacket);
        void HandleGuildQueryNewsOpcode(WorldPacket& recvPacket);
        void HandleGuildSetRankPermissionsOpcode(WorldPacket& recvPacket);
        void HandleGuildAddRankOpcode(WorldPacket& recvPacket);
        void HandleGuildDelRankOpcode(WorldPacket& recvPacket);
        void HandleGuildSwitchRank(WorldPacket& recvPacket);
        void HandleGuildChangeInfoTextOpcode(WorldPacket& recvPacket);
        void HandleSaveGuildEmblemOpcode(WorldPacket& recvPacket);
        void HandleGuildRequestPartyState(WorldPacket& recvPacket);
        void HandleGuildRequestMaxDailyXP(WorldPacket& recvPacket);
        void HandleGuildRequestChallengeUpdate(WorldPacket& recvPacket);
        void HandleAutoDeclineGuildInvites(WorldPacket& recvPacket);
        void HandleQueryGuildRecipes(WorldPacket& recvPacket);
        void HandleQueryGuildMembersForRecipe(WorldPacket& recvPacket);
        void HandleQueryGuildMemberRecipes(WorldPacket& recvPacket);

        void HandleGuildFinderAddApplication(WorldPacket& recvPacket);
        void HandleGuildFinderBrowse(WorldPacket& recvPacket);
        void HandleGuildFinderDeclineRecruit(WorldPacket& recvPacket);
        void HandleGuildFinderGetApplications(WorldPacket& recvPacket);
        void HandleGuildFinderGetRecruits(WorldPacket& recvPacket);
        void HandleGuildFinderPostRequest(WorldPacket& recvPacket);
        void HandleGuildFinderRemoveApplication(WorldPacket& recvPacket);
        void HandleGuildFinderSetGuildPost(WorldPacket& recvPacket);

        void HandleTaxiNodeStatusQueryOpcode(WorldPacket& recvPacket);
        void HandleTaxiQueryAvailableNodes(WorldPacket& recvPacket);
        void HandleEnableTaxi(WorldPacket& recvPacket);
        void HandleActivateTaxiOpcode(WorldPacket& recvPacket);
        void HandleActivateTaxiExpressOpcode(WorldPacket& recvPacket);
        void HandleMoveSplineDoneOpcode(WorldPacket& recvPacket);
        void SendActivateTaxiReply(ActivateTaxiReply reply);

        void HandleTabardVendorActivateOpcode(WorldPacket& recvPacket);
        void HandleBankerActivateOpcode(WorldPacket& recvPacket);
        void HandleBuyBankSlotOpcode(WorldPacket& recvPacket);
        void HandleTrainerListOpcode(WorldPacket& recvPacket);
        void HandleTrainerBuySpellOpcode(WorldPacket& recvPacket);
        void HandlePetitionShowListOpcode(WorldPacket& recvPacket);
        void HandleGossipHelloOpcode(WorldPacket& recvPacket);
        void HandleGossipSelectOptionOpcode(WorldPacket& recvPacket);
        void HandleSpiritHealerActivateOpcode(WorldPacket& recvPacket);
        void HandleNpcTextQueryOpcode(WorldPacket& recvPacket);
        void HandleBinderActivateOpcode(WorldPacket& recvPacket);
        void HandleListPets(WorldPacket& recvPacket);
        void HandleSetPetSlot(WorldPacket& recvPacket);
        void HandleStablePetCallback(PreparedQueryResult result);
        void HandleUnstablePet(WorldPacket& recvPacket);
        void HandleUnstablePetCallback(PreparedQueryResult result, uint32 petId);
        void HandleBuyStableSlot(WorldPacket& recvPacket);
        void HandleStableRevivePet(WorldPacket& recvPacket);
        void HandleStableSwapPet(WorldPacket& recvPacket);
        void HandleStableSwapPetCallback(PreparedQueryResult result, uint32 petId);
        void HandleLearnPetSpecialization(WorldPacket& recvPacket);
        void SendTrainerBuyFailed(ObjectGuid guid, uint32 spellId, uint32 reason);

        void HandleDuelProposedOpcode(WorldPacket& recvPacket);
        void HandleDuelResponseOpcode(WorldPacket& recvPacket);

        void HandleAcceptTradeOpcode(WorldPacket& recvPacket);
        void HandleBeginTradeOpcode(WorldPacket& recvPacket);
        void HandleBusyTradeOpcode(WorldPacket& recvPacket);
        void HandleCancelTradeOpcode(WorldPacket& recvPacket);
        void HandleClearTradeItemOpcode(WorldPacket& recvPacket);
        void HandleIgnoreTradeOpcode(WorldPacket& recvPacket);
        void HandleInitiateTradeOpcode(WorldPacket& recvPacket);
        void HandleSetTradeGoldOpcode(WorldPacket& recvPacket);
        void HandleSetTradeItemOpcode(WorldPacket& recvPacket);
        void HandleUnacceptTradeOpcode(WorldPacket& recvPacket);

        void HandleAuctionHelloOpcode(WorldPacket& recvPacket);
        void HandleAuctionListItems(WorldPacket& recvData);
        void HandleAuctionListBidderItems(WorldPacket& recvData);
        void HandleAuctionSellItem(WorldPacket& recvData);
        void HandleAuctionRemoveItem(WorldPacket& recvData);
        void HandleAuctionListOwnerItems(WorldPacket& recvData);
        void HandleAuctionPlaceBid(WorldPacket& recvData);
        void HandleAuctionListPendingSales(WorldPacket& recvData);
        void HandleReplicateItems(WorldPackets::AuctionHouse::AuctionReplicateItems& packet);

        void HandleGetMailList(WorldPacket& recvData);
        void HandleSendMail(WorldPacket& recvData);
        void HandleMailTakeMoney(WorldPacket& recvData);
        void HandleMailTakeItem(WorldPacket& recvData);
        void HandleMailMarkAsRead(WorldPacket& recvData);
        void HandleMailReturnToSender(WorldPacket& recvData);
        void HandleMailDelete(WorldPacket& recvData);
        void HandleItemTextQuery(WorldPacket& recvData);
        void HandleMailCreateTextItem(WorldPacket& recvData);
        void HandleQueryNextMailTime(WorldPacket& recvData);
        void HandleCancelChanneling(WorldPacket& recvData);

        void SendItemPageInfo(ItemTemplate* itemProto);
        void HandleSplitItemOpcode(WorldPacket& recvPacket);
        void HandleSwapInvItemOpcode(WorldPacket& recvPacket);
        void HandleDestroyItemOpcode(WorldPacket& recvPacket);
        void HandleAutoEquipItemOpcode(WorldPacket& recvPacket);
        void HandleSellItemOpcode(WorldPacket& recvPacket);
        void HandleBuyItemInSlotOpcode(WorldPacket& recvPacket);
        void HandleBuyItemOpcode(WorldPacket& recvPacket);
        void HandleListInventoryOpcode(WorldPacket& recvPacket);
        void HandleAutoStoreBagItemOpcode(WorldPacket& recvPacket);
        void HandleReadItem(WorldPacket& recvPacket);
        void HandleAutoEquipItemSlotOpcode(WorldPacket& recvPacket);
        void HandleSwapItem(WorldPacket& recvPacket);
        void HandleBuybackItem(WorldPacket& recvPacket);
        void HandleAutoBankItemOpcode(WorldPacket& recvPacket);
        void HandleAutoStoreBankItemOpcode(WorldPacket& recvPacket);
        void HandleWrapItemOpcode(WorldPacket& recvPacket);
        void HandleUpgradeItemOpcode(WorldPacket& recvPacket);
        void SendItemUpgradeResult(bool success);
        void HandleSetLootSpecialization(WorldPacket& recvData);

        void HandleAttackSwingOpcode(WorldPacket& recvPacket);
        void HandleAttackStopOpcode(WorldPacket& recvPacket);
        void HandleSetSheathedOpcode(WorldPacket& recvPacket);

        void HandleUseItemOpcode(WorldPacket& recvPacket);
        void HandleOpenItemOpcode(WorldPacket& recvPacket);
        void HandleCastSpellOpcode(WorldPacket& recvPacket);
        void HandleCancelCastOpcode(WorldPacket& recvPacket);
        void HandleCancelAuraOpcode(WorldPacket& recvPacket);
        void HandleCancelModSpeedNoControlAuras(WorldPacket& recvPacket);
        void HandleCancelGrowthAuraOpcode(WorldPacket& recvPacket);
        void HandleCancelAutoRepeatSpellOpcode(WorldPacket& recvPacket);

        void HandleLearnTalentOpcode(WorldPacket& recvPacket);
        void HandleLearnPreviewTalents(WorldPacket& recvPacket);
        void HandleRespecWipeConfirmOpcode(WorldPacket& recvPacket);
        void HandleUnlearnSkillOpcode(WorldPacket& recvPacket);
        void HandleUnlearnSpecializationOpcode(WorldPacket& recvPacket);

        void HandleArcheologyRequestHistory(WorldPacket& recvPacket);

        void HandleQuestgiverStatusQueryOpcode(WorldPacket& recvPacket);
        void HandleQuestgiverStatusMultipleQuery(WorldPackets::Quest::QuestGiverStatusMultipleQuery& packet);
        void HandleQuestgiverHelloOpcode(WorldPacket& recvPacket);
        void HandleQuestgiverAcceptQuestOpcode(WorldPacket& recvPacket);
        void HandleQuestgiverQueryQuestOpcode(WorldPacket& recvPacket);
        void HandleQuestgiverChooseRewardOpcode(WorldPacket& recvPacket);
        void HandleQuestgiverRequestRewardOpcode(WorldPacket& recvPacket);
        void HandleQuestQueryOpcode(WorldPacket& recvPacket);
        void HandleQuestgiverCancel(WorldPacket& recvData);
        void HandleQuestLogSwapQuest(WorldPacket& recvData);
        void HandleQuestLogRemoveQuest(WorldPacket& recvData);
        void HandleQuestConfirmAccept(WorldPacket& recvData);
        void HandleQuestgiverCompleteQuest(WorldPacket& recvData);
        void HandleQuestgiverQuestAutoLaunch(WorldPacket& recvPacket);
        void HandlePushQuestToParty(WorldPacket& recvPacket);
        void HandleQuestPushResult(WorldPacket& recvPacket);

        void HandleMessagechatOpcode(WorldPacket& recvPacket);
        void HandleAddonMessagechatOpcode(WorldPacket& recvPacket);
        void SendPlayerNotFoundNotice(std::string const& name);
        void SendPlayerAmbiguousNotice(std::string const& name);
        void SendChatRestrictedNotice(ChatRestrictionType restriction);
        void HandleTextEmoteOpcode(WorldPacket& recvPacket);
        void HandleChatIgnoredOpcode(WorldPacket& recvPacket);

        void HandleUnregisterAddonPrefixesOpcode(WorldPacket& recvPacket);
        void HandleAddonRegisteredPrefixesOpcode(WorldPacket& recvPacket);

        void HandleReclaimCorpseOpcode(WorldPacket& recvPacket);
        void HandleCorpseQueryOpcode(WorldPacket& recvPacket);
        void HandleCorpseMapPositionQuery(WorldPacket& recvPacket);
        void HandleResurrectResponseOpcode(WorldPacket& recvPacket);
        void HandleSummonResponseOpcode(WorldPacket& recvData);

        void HandleJoinChannel(WorldPacket& recvPacket);
        void HandleLeaveChannel(WorldPacket& recvPacket);
        void HandleChannelList(WorldPacket& recvPacket);
        void HandleChannelPassword(WorldPacket& recvPacket);
        void HandleChannelSetOwner(WorldPacket& recvPacket);
        void HandleChannelOwner(WorldPacket& recvPacket);
        void HandleChannelModerator(WorldPacket& recvPacket);
        void HandleChannelUnmoderator(WorldPacket& recvPacket);
        void HandleChannelMute(WorldPacket& recvPacket);
        void HandleChannelUnmute(WorldPacket& recvPacket);
        void HandleChannelInvite(WorldPacket& recvPacket);
        void HandleChannelKick(WorldPacket& recvPacket);
        void HandleChannelBan(WorldPacket& recvPacket);
        void HandleChannelUnban(WorldPacket& recvPacket);
        void HandleChannelAnnouncements(WorldPacket& recvPacket);
        void HandleChannelModerate(WorldPacket& recvPacket);
        void HandleChannelDeclineInvite(WorldPacket& recvPacket);
        void HandleChannelDisplayListQuery(WorldPacket& recvPacket);
        //void HandleGetChannelMemberCount(WorldPacket& recvPacket);
        void HandleSetChannelWatch(WorldPacket& recvPacket);

        void HandleCompleteCinematic(WorldPackets::Misc::CompleteCinematic& packet);
        void HandleCompleteMovie(WorldPackets::Misc::CompleteMovie& packet);    
        void HandleNextCinematicCamera(WorldPackets::Misc::NextCinematicCamera& packet);

        void HandlePageTextQueryOpcode(WorldPacket& recvPacket);

        void HandleTutorialFlag (WorldPacket& recvData);
        void HandleTutorialClear(WorldPacket& recvData);
        void HandleTutorialReset(WorldPacket& recvData);

        void HandlePlayerChoiceResponse(WorldPacket& recvData);

        //Pet
        void HandlePetAction(WorldPacket& recvData);
        void HandlePetStopAttack(WorldPacket& recvData);
        void HandlePetActionHelper(Unit* pet, ObjectGuid guid1, uint32 spellid, uint16 flag, ObjectGuid guid2, float x, float y, float z);
        void HandlePetNameQuery(WorldPacket& recvData);
        void HandlePetSetAction(WorldPacket& recvData);
        void HandlePetAbandon(WorldPacket& recvData);
        void HandlePetRename(WorldPacket& recvData);
        void HandlePetCancelAuraOpcode(WorldPacket& recvPacket);
        void HandlePetSpellAutocastOpcode(WorldPacket& recvPacket);
        void HandlePetCastSpellOpcode(WorldPacket& recvPacket);
        void HandlePetLearnTalent(WorldPacket& recvPacket);
        void HandleLearnPreviewTalentsPet(WorldPacket& recvPacket);

        void HandleSetActionBarToggles(WorldPacket& recvData);

        void HandleCharRenameOpcode(WorldPacket& recvData);
        void HandleChangePlayerNameOpcodeCallBack(PreparedQueryResult result, std::string const& newName);
        void SendRenameResult(uint8 result, ObjectGuid guid = ObjectGuid::Empty, std::string name = "");
        void HandleSetPlayerDeclinedNames(WorldPacket& recvData);
        void HandeSetTalentSpecialization(WorldPacket& recvData);

        void HandleTotemDestroyed(WorldPacket& recvData);
        void HandleDismissCritter(WorldPacket& recvData);

        //Battleground
        void HandleBattlemasterHelloOpcode(WorldPacket& recvData);
        void HandleBattlemasterJoinOpcode(WorldPacket& recvData);
        void HandlePVPLogDataOpcode(WorldPacket& recvData);
        void HandleBattleFieldPortOpcode(WorldPacket& recvData);
        void HandleBattlefieldListOpcode(WorldPacket& recvData);
        void HandleBattlefieldLeaveOpcode(WorldPacket& recvData);
        void HandleBattlemasterJoinArena(WorldPacket& recvData);
        void HandleBattlemasterJoinRated(WorldPacket& recvData);
        void HandleReportPvPAFK(WorldPacket& recvData);
        //void HandleRequestRatedBgInfo(WorldPacket& recvData); 
        void HandleRequestPvpOptions(WorldPacket& recvData);
        void HandleRequestPvpReward(WorldPacket& recvData);
        void HandleRequestRatedBgStats(WorldPacket& recvData);
        void HandleRequestConquestFormulaConstants(WorldPacket& recvData);

        void HandleWardenDataOpcode(WorldPacket& recvData);
        void HandleWorldTeleportOpcode(WorldPacket& recvData);
        void HandleMinimapPingOpcode(WorldPacket& recvData);
        void HandleRandomRollOpcode(WorldPacket& recvData);
        void HandleFarSightOpcode(WorldPackets::Misc::FarSight& packet);
        void HandleSetDungeonDifficultyOpcode(WorldPacket& recvData);
        void HandleSetRaidDifficultyOpcode(WorldPacket& recvData);
        void HandleMoveSetCanFlyAckOpcode(WorldPacket& recvData);
        void HandleRealmSplitOpcode(WorldPacket& recvData);
        void HandleTimeSyncResp(WorldPacket& recvData);
        void HandleWhoisOpcode(WorldPacket& recvData);
        void HandleResetInstancesOpcode(WorldPacket& recvData);
        void HandleHearthAndResurrect(WorldPacket& recvData);
        void HandleInstanceLockResponse(WorldPacket& recvPacket);

        // Battlefield
        void SendBfInvitePlayerToWar(ObjectGuid guid, uint32 zoneId, uint32 time);
        void SendBfInvitePlayerToQueue(ObjectGuid guid);
        void SendBfQueueInviteResponse(ObjectGuid guid, uint32 zoneId, bool canQueue = true, bool full = false);
        void SendBfEntered(ObjectGuid guid);
        void SendBfLeaveMessage(ObjectGuid guid, BFLeaveReason reason = BF_LEAVE_REASON_EXITED);
        void HandleBfQueueInviteResponse(WorldPacket& recvData);
        void HandleBfEntryInviteResponse(WorldPacket& recvData);
        void HandleBfQueueRequest(WorldPacket& recvData);
        void HandleBfExitRequest(WorldPacket& recvData);

        // Looking for Dungeon/Raid
        void HandleLfgGetLockInfoOpcode(WorldPacket& recvData);
        void SendLfgPlayerLockInfo();
        void SendLfgPartyLockInfo();
        void HandleLfgJoinOpcode(WorldPacket& recvData);
        void HandleLfgLeaveOpcode(WorldPacket& recvData);
        void HandleLfgSetRolesOpcode(WorldPacket& recvData);
        void HandleLfgProposalResultOpcode(WorldPacket& recvData);
        void HandleLfgSetBootVoteOpcode(WorldPacket& recvData);
        void HandleLfgTeleportOpcode(WorldPacket& recvData);
        void HandleLfrJoinOpcode(WorldPacket& recvData);
        void HandleLfrLeaveOpcode(WorldPacket& recvData);
        void HandleLfgGetStatus(WorldPacket& recvData);
        void HandleSetLfgBonusFactionId(WorldPacket& recvData);

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

        void HandleAreaSpiritHealerQueryOpcode(WorldPacket& recvData);
        void HandleAreaSpiritHealerQueueOpcode(WorldPacket& recvData);
        void HandleCancelMountAuraOpcode(WorldPacket& recvData);
        void HandleSelfResOpcode(WorldPacket& recvData);
        //void HandleComplainOpcode(WorldPacket& recvData);
        void HandleRequestPetInfoOpcode(WorldPacket& recvData);

        // Socket gem
        void HandleSocketOpcode(WorldPacket& recvData);

        void HandleCancelTempEnchantmentOpcode(WorldPacket& recvData);

        void HandleItemRefundInfoRequest(WorldPacket& recvData);
        void HandleItemRefund(WorldPacket& recvData);
        void SendItemExpirePurchaseRefund(ObjectGuid itemGuid);

        void HandleChannelVoiceOnOpcode(WorldPacket& recvData);
        void HandleVoiceSessionEnableOpcode(WorldPacket& recvData);
        void HandleSetActiveVoiceChannel(WorldPacket& recvData);
        void HandleSetTaxiBenchmarkOpcode(WorldPacket& recvData);
        void HandleSetAdvancedCombatLogging(WorldPacket& recvData);

        // Guild Bank
        void HandleGuildPermissions(WorldPacket& recvData);
        void HandleGuildBankMoneyWithdrawn(WorldPacket& recvData);
        void HandleGuildBankerActivate(WorldPacket& recvData);
        void HandleGuildBankQueryTab(WorldPacket& recvData);
        void HandleGuildBankLogQuery(WorldPacket& recvData);
        void HandleGuildBankDepositMoney(WorldPacket& recvData);
        void HandleGuildBankWithdrawMoney(WorldPacket& recvData);
        void HandleGuildBankSwapItems(WorldPacket& recvData);
        void HandleGuildBankTabNote(WorldPacket& recvData);

        void HandleGuildBankUpdateTab(WorldPacket& recvData);
        void HandleGuildBankBuyTab(WorldPacket& recvData);
        void HandleQueryGuildBankTabText(WorldPacket& recvData);
        void HandleGuildQueryXPOpcode(WorldPacket& recvData);

        // Refer-a-Friend
        void HandleGrantLevel(WorldPacket& recvData);
        void HandleAcceptGrantLevel(WorldPacket& recvData);

        // Calendar
        void HandleCalendarGetCalendar(WorldPacket& recvData);
        void HandleCalendarGetEvent(WorldPacket& recvData);
        void HandleCalendarGuildFilter(WorldPacket& recvData);
        void HandleCalendarAddEvent(WorldPacket& recvData);
        void HandleCalendarUpdateEvent(WorldPacket& recvData);
        void HandleCalendarRemoveEvent(WorldPacket& recvData);
        void HandleCalendarCopyEvent(WorldPacket& recvData);
        void HandleCalendarEventInvite(WorldPacket& recvData);
        void HandleCalendarEventRsvp(WorldPacket& recvData);
        void HandleCalendarEventRemoveInvite(WorldPacket& recvData);
        void HandleCalendarEventStatus(WorldPacket& recvData);
        void HandleCalendarEventModeratorStatus(WorldPacket& recvData);
        void HandleCalendarComplain(WorldPacket& recvData);
        void HandleCalendarGetNumPending(WorldPacket& recvData);
        void HandleCalendarEventSignup(WorldPacket& recvData);

        void SendCalendarRaidLockout(InstanceSave const* save, bool add);
        void SendCalendarRaidLockoutUpdated(InstanceSave const* save);
        void HandleSetSavedInstanceExtend(WorldPacket& recvData);

        // Void Storage
        void HandleVoidStorageUnlock(WorldPacket& recvData);
        void HandleVoidStorageQuery(WorldPacket& recvData);
        void HandleVoidStorageTransfer(WorldPacket& recvData);
        void HandleVoidSwapItem(WorldPacket& recvData);
        void SendVoidStorageTransferResult(VoidTransferError result);

        // Transmogrification
        void HandleTransmogrifyItems(WorldPacket& recvData);

        // Reforge
        void HandleReforgeItemOpcode(WorldPacket& recvData);
        void SendReforgeResult(bool success);

        // BlackMarket
        void HandleBlackMarketHelloOpcode(WorldPacket& recvData);
        void SendBlackMarketHello(ObjectGuid NpcGUID, bool Open);
        void HandleBlackMarketRequestItemOpcode(WorldPacket& recvData);
        void SendBlackMarketRequestItemsResult();

        void HandleBlackMarketBidOnItem(WorldPacket& recvData);
        void SendBlackMarketBidOnItemResult(uint32 auctionId, uint32 ItemID, BlackMarketError result);

        // Miscellaneous
        void HandleSpellClick(WorldPacket& recvData);
        void HandleMirrorImageDataRequest(WorldPacket& recvData);
        void HandleAlterAppearance(WorldPacket& recvData);
        void SendBarberShopResult(BarberShopResult result);
        void HandleRemoveGlyph(WorldPacket& recvData);
        void HandleCharCustomize(WorldPacket& recvData);
        void HandleQueryInspectAchievements(WorldPacket& recvData);
        void HandleGuildAchievementProgressQuery(WorldPacket& recvData);
        void HandleEquipmentSetSave(WorldPacket& recvData);
        void HandleEquipmentSetDelete(WorldPacket& recvData);
        void HandleEquipmentSetUse(WorldPacket& recvData);
        void HandleWorldStateUITimerUpdate(WorldPacket& recvData);
        void HandleReadyForAccountDataTimes(WorldPacket& recvData);
        void HandleQuestNPCQuery(WorldPacket& recvData);
        void HandleQuestPOIQuery(WorldPacket& recvData);
        void HandleEjectPassenger(WorldPacket& data);
        void HandleEnterPlayerVehicle(WorldPacket& recvData);
        void HandleMissileTrajectoryCollision(WorldPacket& recvPacket);
        void HandleRequestHotfix(WorldPacket& recvPacket);
        void HandleUpdateMissileTrajectory(WorldPacket& recvPacket);
        void HandleViolenceLevel(WorldPacket& recvPacket);
        void HandleObjectUpdateFailedOpcode(WorldPacket& recvPacket);
        void HandleSelectFactionOpcode(WorldPacket& recvPacket);
        void HandleRequestCategoryCooldowns(WorldPacket& recvPacket);
        void HandleRequestCemeteryList(WorldPacket& recvPacket);
        void HandleDiscardedTimeSyncAcks(WorldPacket & recvData);
        void HandleLogStreamingError(WorldPacket & recvData);
        void HandleShowTradeSkill(WorldPacket& recvData);
        void SendStreamingMovie();

        void SendBroadcastTextDb2Reply(uint32 entry, ByteBuffer& buffer);
        void SendItemDb2Reply(uint32 entry, ByteBuffer& buffer);
        void SendItemSparseDb2Reply(uint32 entry, ByteBuffer& buffer);

        int32 HandleEnableNagleAlgorithm();

        // Compact Unit Frames (4.x)
        void HandleSaveCUFProfiles(WorldPacket& recvPacket);
        void SendLoadCUFProfiles();

        // Battle Pet
        void HandlePetBattleStartPvpMatchmaking(WorldPacket& recvData);
        void HandlePetBattleStopPvpMatchmaking(WorldPacket& recvData);
        void HandleBattlePetModifyName(WorldPacket& recvData);
        void HandleBattlePetDelete(WorldPacket& recvData);
        void HandleBattlePetSetBattleSlot(WorldPacket& recvData);
        void HandleBattlePetSetFlags(WorldPacket& recvData);
        void HandleBattlePetQueryName(WorldPacket& recvData);
        void HandleBattlePetRequestJournal(WorldPacket& recvData);
        void HandleBattlePetSummonCompanion(WorldPacket& recvData);
        void HandleBattlePetCage(WorldPacket& recvData);
        void HandleBattlePetLearn(WorldPacket& recvData);

        // Pet Battle
        void HandlePetBattleInput(WorldPacket& recvData);
        void HandlePetBattleRequestWild(WorldPacket& recvData);
        void HandlePetBattleSetFrontPet(WorldPacket& recvData);

        void SendPetBattleRequestFailed(uint8 reason);

        // Titles
        void HandleSetTitleOpcode(WorldPacket& recvData);
        void SendTitleEarned(uint32 TitleIndex);
        void SendTitleLost(uint32 TitleIndex);

        // BattlePay
        void HandleBattlePayGetProductList(WorldPacket& recvData);
        void HandleBattlePayGetPurchaseList(WorldPacket& recvData);
        void HandleBattlePayStartPurchase(WorldPacket& recvData);
        void HandleBattlePayConfirmPurchaseResponse(WorldPacket& recvData);
        void HandleBattlePayCharBoost(WorldPacket& recvData);

        // Scene
        void HandleSceneTriggerEvent(WorldPacket& recvData);
        void HandleScenePlaybackComplete(WorldPacket& recvData);
        void HandleScenePlaybackCanceled(WorldPacket& recvData);

        // Scenario
        void HandleQueryScenarioPOI(WorldPacket& recvData);

        // Challenge Mode
        void HandleGetChallengeModeRewards(WorldPacket& recvData);
        void HandleChallengeModeRequestLeaders(WorldPacket& recvData);
        void HandleChallengeModeRequestMapStats(WorldPacket& recvData);
        void HandleResetChallengeMode(WorldPacket& recvData);
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
        std::shared_ptr<WorldSocket> m_Socket;
        std::string m_Address;

        AccountTypes _security;
        uint32 _accountId;
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
