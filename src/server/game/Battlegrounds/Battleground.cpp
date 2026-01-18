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

#include "ArenaTeam.h"
#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "BattlePetMgr.h"
#include "Creature.h"
#include "Chat.h"
#include "Formulas.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "Guild.h"
#include "GuildMgr.h"
#include "MapManager.h"
#include "MiscPackets.h"
#include "Object.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ReputationMgr.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "Util.h"
#include "World.h"
#include "WorldPacket.h"
#include "Transport.h"
#include "Pet.h"
#include "AchievementMgr.h"
#include "BattlegroundTOK.h"
#include "Realm.h"

namespace Trinity
{
    class BattlegroundChatBuilder
    {
        public:
            BattlegroundChatBuilder(ChatMsg msgtype, int32 textId, Player const* source, va_list* args = NULL)
                : _msgtype(msgtype), _textId(textId), _source(source), _args(args) { }

            void operator()(WorldPacket& data, LocaleConstant loc_idx)
            {
                char const* text = sObjectMgr->GetTrinityString(_textId, loc_idx);
                if (_args)
                {
                    // we need copy va_list before use or original va_list will corrupted
                    va_list ap;
                    va_copy(ap, *_args);

                    char str[2048];
                    vsnprintf(str, 2048, text, ap);
                    va_end(ap);

                    do_helper(data, &str[0]);
                }
                else
                    do_helper(data, text);
            }

        private:
            void do_helper(WorldPacket& data, char const* text)
            {
                uint64 target_guid = _source ? _source->GetGUID() : 0;
                ChatHandler::BuildChatPacket(data, _msgtype, LANG_UNIVERSAL, _source, _source, text);
            }

            ChatMsg _msgtype;
            int32 _textId;
            Player const* _source;
            va_list* _args;
    };

    class Battleground2ChatBuilder
    {
        public:
            Battleground2ChatBuilder(ChatMsg msgtype, int32 textId, Player const* source, int32 arg1, int32 arg2)
                : _msgtype(msgtype), _textId(textId), _source(source), _arg1(arg1), _arg2(arg2) { }

            void operator()(WorldPacket& data, LocaleConstant loc_idx)
            {
                char const* text = sObjectMgr->GetTrinityString(_textId, loc_idx);
                char const* arg1str = _arg1 ? sObjectMgr->GetTrinityString(_arg1, loc_idx) : "";
                char const* arg2str = _arg2 ? sObjectMgr->GetTrinityString(_arg2, loc_idx) : "";

                char str[2048];
                snprintf(str, 2048, text, arg1str, arg2str);

                uint64 target_guid = _source  ? _source->GetGUID() : 0;

                ChatHandler::BuildChatPacket(data, _msgtype, LANG_UNIVERSAL, _source, _source, str);
            }

        private:
            ChatMsg _msgtype;
            int32 _textId;
            Player const* _source;
            int32 _arg1;
            int32 _arg2;
    };

    class BattlegroundStartLocalizedAnnounceBuilder
    {
        public:
            explicit BattlegroundStartLocalizedAnnounceBuilder(int32 textId, Battleground* bg, uint32 minLevel, uint32 maxLevel)
                : i_textId(textId), i_bg(bg), i_minLevel(minLevel), i_maxLevel(maxLevel) { }
            void operator()(std::vector<WorldPacket*>& data_list, LocaleConstant loc_idx)
            {
                char const* text = sObjectMgr->GetTrinityString(i_textId, loc_idx);
                char str[2048];
                sprintf(str, text, i_bg->GetName(loc_idx), i_minLevel, i_maxLevel);
               do_helper(data_list, &str[0]);
            }
        private:
            char* lineFromMessage(char*& pos) { char* start = strtok(pos, "\n"); pos = NULL; return start; }
            void do_helper(std::vector<WorldPacket*>& data_list, char* text)
            {
                char* pos = text;
                while (char* line = lineFromMessage(pos))
                {
                    WorldPacket* data = new WorldPacket();
                    ChatHandler::BuildChatPacket(*data, CHAT_MSG_SYSTEM, LANG_UNIVERSAL, 0, 0, std::string(line));
                    data_list.push_back(data);
                }
            }

            int32 i_textId;
            Battleground* i_bg;
            uint32 i_minLevel, i_maxLevel;
    };
}                                                           // namespace Trinity

template<class Do>
void Battleground::BroadcastWorker(Do& _do)
{
    for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        if (Player* player = ObjectAccessor::FindPlayer(itr->first))
            _do(player);
}

Battleground::Battleground()
{
    m_Guid              = ObjectGuid::Empty;
    m_TypeID            = BATTLEGROUND_TYPE_NONE;
    m_RandomTypeID      = BATTLEGROUND_TYPE_NONE;
    m_InstanceID        = 0;
    m_Status            = STATUS_NONE;
    m_ClientInstanceID  = 0;
    m_EndTime           = 0;
    m_LastResurrectTime = 0;
    m_BracketId         = BG_BRACKET_ID_FIRST;
    m_InvitedAlliance   = 0;
    m_InvitedHorde      = 0;
    m_ArenaType         = 0;
    m_StartTime         = 0;
    m_CountdownTimer    = 0;
    m_ResetStatTimer    = 0;
    m_ValidStartPositionTimer = 0;
    m_Events            = 0;
    m_BuffChange        = false;
    m_IsRandom          = false;
    m_Name              = "";
    m_LevelMin          = 0;
    m_LevelMax          = 0;
    m_InBGFreeSlotQueue = false;
    m_SetDeleteThis     = false;

    m_PlayersPositionsTimer = 5000;

    m_MaxPlayersPerTeam = 0;
    m_MaxPlayers        = 0;
    m_MinPlayersPerTeam = 0;
    m_MinPlayers        = 0;

    m_MapId             = 0;
    m_Map               = NULL;
    m_StartMaxDist      = 0.0f;

    m_TeamStartLocX[TEAM_ALLIANCE]   = 0;
    m_TeamStartLocX[TEAM_HORDE]      = 0;

    m_TeamStartLocY[TEAM_ALLIANCE]   = 0;
    m_TeamStartLocY[TEAM_HORDE]      = 0;

    m_TeamStartLocZ[TEAM_ALLIANCE]   = 0;
    m_TeamStartLocZ[TEAM_HORDE]      = 0;

    m_TeamStartLocO[TEAM_ALLIANCE]   = 0;
    m_TeamStartLocO[TEAM_HORDE]      = 0;

    m_ArenaTeamRatingChanges[TEAM_ALLIANCE]   = 0;
    m_ArenaTeamRatingChanges[TEAM_HORDE]      = 0;

    m_BgRaids[TEAM_ALLIANCE]         = NULL;
    m_BgRaids[TEAM_HORDE]            = NULL;

    m_PlayersCount[TEAM_ALLIANCE]    = 0;
    m_PlayersCount[TEAM_HORDE]       = 0;

    m_TeamScores[TEAM_ALLIANCE]      = 0;
    m_TeamScores[TEAM_HORDE]         = 0;

    m_PrematureCountDown = false;

    m_HonorMode = BG_NORMAL;

    StartDelayTimes[BG_STARTING_EVENT_FIRST]  = BG_START_DELAY_2M;
    StartDelayTimes[BG_STARTING_EVENT_SECOND] = BG_START_DELAY_1M;
    StartDelayTimes[BG_STARTING_EVENT_THIRD]  = BG_START_DELAY_30S;
    StartDelayTimes[BG_STARTING_EVENT_FOURTH] = BG_START_DELAY_NONE;
    //we must set to some default existing values
    StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_BG_WS_START_TWO_MINUTES;
    StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_WS_START_ONE_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_WS_START_HALF_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_WS_HAS_BEGUN;
}

Battleground::~Battleground()
{
    // remove objects and creatures
    // (this is done automatically in mapmanager update, when the instance is reset after the reset time)
    uint32 size = uint32(BgCreatures.size());
    for (uint32 i = 0; i < size; ++i)
        DelCreature(i);

    size = uint32(BgObjects.size());
    for (uint32 i = 0; i < size; ++i)
        DelObject(i);

    sBattlegroundMgr->RemoveBattleground(GetTypeID(), GetInstanceID());
    // unload map
    if (m_Map)
    {
        m_Map->SetUnload();
        //unlink to prevent crash, always unlink all pointer reference before destruction
        m_Map->SetBG(NULL);
        m_Map = NULL;
    }
    // remove from bg free slot queue
    RemoveFromBGFreeSlotQueue();

    for (BattlegroundScoreMap::const_iterator itr = PlayerScores.begin(); itr != PlayerScores.end(); ++itr)
        delete itr->second;
}

void Battleground::Update(uint32 diff)
{
    if (!PreUpdateImpl(diff))
        return;

    if (!GetPlayersSize())
    {
        //BG is empty
        // if there are no players invited, delete BG
        // this will delete arena or bg object, where any player entered
        // [[   but if you use battleground object again (more battles possible to be played on 1 instance)
        //      then this condition should be removed and code:
        //      if (!GetInvitedCount(HORDE) && !GetInvitedCount(ALLIANCE))
        //          this->AddToFreeBGObjectsQueue(); // not yet implemented
        //      should be used instead of current
        // ]]
        // Battleground Template instance cannot be updated, because it would be deleted
        if (!GetInvitedCount(HORDE) && !GetInvitedCount(ALLIANCE))
            m_SetDeleteThis = true;
        return;
    }

    switch (GetStatus())
    {
        case STATUS_WAIT_JOIN:
            if (GetPlayersSize())
            {
                ProcessJoin(diff);
                CheckSafePositions(diff);
            }
            break;
        case STATUS_IN_PROGRESS:
            ProcessProgress(diff);
            break;
        case STATUS_WAIT_LEAVE:
            ProcessLeave(diff);
            break;
        default:
            break;
    }

    // Update start time and reset stats timer
    SetElapsedTime(GetElapsedTime() + diff);
    if (GetStatus() == STATUS_WAIT_JOIN)
    {
        m_ResetStatTimer += diff;
        m_CountdownTimer += diff;
    }

    PostUpdateImpl(diff);
}

void Battleground::CheckSafePositions(uint32 diff)
{
    float maxDist = GetStartMaxDist();
    if (!maxDist)
        return;

    m_ValidStartPositionTimer += diff;
    if (m_ValidStartPositionTimer >= CHECK_PLAYER_POSITION_INVERVAL)
    {
        m_ValidStartPositionTimer = 0;

        float x, y, z, o;
        for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
            if (Player* player = ObjectAccessor::FindPlayer(itr->first))
            {
                Position pos = player->GetPosition();
                GetTeamStartLoc(player->GetBGTeam(), x, y, z, o);
                if (pos.GetExactDistSq(x, y, z) > maxDist)
                {
                    TC_LOG_DEBUG("bg.battleground", "BATTLEGROUND: Sending %s back to start location (map: %u) (possible exploit)", player->GetName().c_str(), GetMapId());
                    player->TeleportTo(GetMapId(), x, y, z, o);
                }
            }
    }
}

void ApplyDampening(Unit* target, uint32 amount)
{
    if (AuraEffect* eff = target->GetAuraEffect(SPELL_DAMPENING, EFFECT_0))
        eff->ChangeAmount(amount);
    else
        target->CastCustomSpell(SPELL_DAMPENING, SPELLVALUE_BASE_POINT0, amount, target, true);
}

void Battleground::UpdateDumpenning(uint32 diff)
{
    m_DampeningTimer.Update(diff);
    if (m_DampeningTimer.Passed())
    {
        if (!m_DampeningCounter)
            m_DampeningTimer.SetInterval(10 * IN_MILLISECONDS);
        m_DampeningTimer.Reset();

        ++m_DampeningCounter;

        for (auto&& team : m_arenaTeams)
        {
            for (auto&& it : *team)
            {
                Player* player = ObjectAccessor::FindPlayer(it->Guid);
                if (player && player->GetMap() == GetBgMap())
                {
                    ApplyDampening(player, m_DampeningCounter);
                    if (Pet* pet = player->GetPet())
                        ApplyDampening(pet, m_DampeningCounter);
                    for (auto&& summon : player->GetSummons())
                        if (summon->IsGuardian())
                            ApplyDampening(summon, m_DampeningCounter);
                }
            }
        }
    }
}

void Battleground::UpdateAfkReports()
{
    auto now = TimeValue::Now();
    for (auto itr = m_afkReports.begin(); itr != m_afkReports.end();)
    {
        ObjectGuid guid = itr->first;
        auto& report = itr->second;
        ++itr;
        if (report.ReportTime != TimeValue::zero())
        {
            if (report.ReportTime + Minutes(1) < now)
            {
                if (Player* player = ObjectAccessor::FindConnectedPlayer(guid))
                    m_afkReports.erase(guid);
                else
                    RemovePlayerAtLeave(guid, true, true);
            }
        }
    }
}

void Battleground::ProcessProgress(uint32 diff)
{
    ProcessOfflineQueue();
    // after 22 minutes without one team losing, the arena closes with no winner and no rating change
    if (IsArena())
    {
        UpdateDumpenning(diff);
        if (GetElapsedTime() >= 22 * MINUTE*IN_MILLISECONDS)
        {
            UpdateArenaWorldState();
            CheckArenaAfterTimerConditions();
            return;
        }

        if (GetElapsedTime() >= 61 * IN_MILLISECONDS)
        {
            if (!m_frameIsFixed)
            {
                for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
                {
                    if (Player* player = _GetPlayer(itr, "Fix frame"))
                    {
                        bool hasFlag = player->m_stealth.HasFlag(STEALTH_GENERAL);
                        if (!hasFlag)
                            player->m_stealth.AddFlag(STEALTH_GENERAL);

                        player->UpdateObjectVisibility();
                        player->m_stealth.DelFlag(STEALTH_GENERAL);
                        player->UpdateObjectVisibility();

                        if (hasFlag) // Restore flag if present
                        {
                            player->m_stealth.AddFlag(STEALTH_GENERAL);
                            player->UpdateObjectVisibility();
                        }
                    }
                }

                m_frameIsFixed = true;
            }
        }
    }
    else
    {
        // Since 5.0.X, client doesn't send request to know players flags positions
        // We need to handle it server-side and send new position every ~ 5s
        if (m_PlayersPositionsTimer <= diff)
        {
            SendFlagsPositions();
            m_PlayersPositionsTimer = 5000;
        }
        else
            m_PlayersPositionsTimer -= diff;

        ProcessRessurect(diff);
        if (sBattlegroundMgr->GetPrematureFinishTime() && (GetPlayersCountByTeam(ALLIANCE) < GetMinPlayersPerTeam() || GetPlayersCountByTeam(HORDE) < GetMinPlayersPerTeam()))
            CheckPrematureEnd(diff);
        else if (m_PrematureCountDown)
            m_PrematureCountDown = false;

        UpdateAfkReports();
    }
}

void Battleground::ProcessOfflineQueue()
{
    // remove offline players from bg after 5 minutes
    if (!m_OfflineQueue.empty())
    {
        BattlegroundPlayerMap::iterator itr = m_Players.find(*(m_OfflineQueue.begin()));
        if (itr != m_Players.end())
        {
            if (itr->second.OfflineRemoveTime <= sWorld->GetGameTime())
            {
                // add deserter for offline player
                int32 duration = 0;
                SpellInfo const* spellEntry = sSpellMgr->GetSpellInfo(26013);
                if (spellEntry)
                    duration = spellEntry->DurationEntry->Duration[0];

                sBattlegroundMgr->ApplyDeserter(itr->first, duration);

                RemovePlayerAtLeave(itr->first, true, true);// remove player from BG
                m_OfflineQueue.pop_front();                 // remove from offline queue
                //do not use itr for anything, because it is erased in RemovePlayerAtLeave()
            }
        }
    }
}

void Battleground::ProcessRessurect(uint32 diff)
{
    // *********************************************************
    // ***        BATTLEGROUND RESSURECTION SYSTEM           ***
    // *********************************************************
    // this should be handled by spell system
    m_LastResurrectTime += diff;
    if (m_LastResurrectTime >= RESURRECTION_INTERVAL)
    {
        if (GetReviveQueueSize())
        {
            for (std::map<ObjectGuid, std::vector<ObjectGuid> >::iterator itr = m_ReviveQueue.begin(); itr != m_ReviveQueue.end(); ++itr)
            {
                Creature* sh = NULL;
                for (std::vector<ObjectGuid>::const_iterator itr2 = (itr->second).begin(); itr2 != (itr->second).end(); ++itr2)
                {
                    Player* player = ObjectAccessor::FindPlayer(*itr2);
                    if (!player)
                        continue;

                    if (!sh && player->IsInWorld())
                    {
                        sh = player->GetMap()->GetCreature(itr->first);
                        // only for visual effect
                        if (sh)
                            // Spirit Heal, effect 117
                            sh->CastSpell(sh, SPELL_SPIRIT_HEAL, true);
                    }

                    // Resurrection visual
                    player->CastSpell(player, SPELL_RESURRECTION_VISUAL, true);
                    m_ResurrectQueue.push_back(*itr2);
                }
                (itr->second).clear();
            }

            m_ReviveQueue.clear();
            m_LastResurrectTime = 0;
        }
        else
            // queue is clear and time passed, just update last resurrection time
            m_LastResurrectTime = 0;
    }
    else if (m_LastResurrectTime > 500)    // Resurrect players only half a second later, to see spirit heal effect on NPC
    {
        for (std::vector<ObjectGuid>::const_iterator itr = m_ResurrectQueue.begin(); itr != m_ResurrectQueue.end(); ++itr)
        {
            Player* player = ObjectAccessor::FindPlayer(*itr);
            if (!player)
                continue;
            player->ResurrectPlayer(1.0f);
            player->CastSpell(player, SPELL_SPIRIT_HEAL_MANA, true);
            player->SpawnCorpseBones(false);
        }
        m_ResurrectQueue.clear();
    }
}

uint32 Battleground::GetPrematureWinner()
{
    uint32 winner = 0;
    if (GetPlayersCountByTeam(ALLIANCE) >= GetMinPlayersPerTeam())
        winner = ALLIANCE;
    else if (GetPlayersCountByTeam(HORDE) >= GetMinPlayersPerTeam())
        winner = HORDE;

    return winner;
}

void Battleground::CheckPrematureEnd(uint32 diff)
{
    // *********************************************************
    // ***           BATTLEGROUND BALLANCE SYSTEM            ***
    // *********************************************************
    // if less then minimum players are in on one side, then start premature finish timer
    if (!m_PrematureCountDown)
    {
        m_PrematureCountDown = true;
        m_PrematureCountDownTimer = sBattlegroundMgr->GetPrematureFinishTime();
    }
    else if (m_PrematureCountDownTimer < diff)
    {
        // time's up!
        EndBattleground(GetPrematureWinner());
        m_PrematureCountDown = false;
    }
    else if (!(IsRatedBG() ? sBattlegroundMgr->IsRatedBgTesting() : sBattlegroundMgr->IsTesting()))
    {
        uint32 newtime = m_PrematureCountDownTimer - diff;
        // announce every minute
        if (newtime > (MINUTE * IN_MILLISECONDS))
        {
            if (newtime / (MINUTE * IN_MILLISECONDS) != m_PrematureCountDownTimer / (MINUTE * IN_MILLISECONDS))
                PSendMessageToAll(LANG_BATTLEGROUND_PREMATURE_FINISH_WARNING, CHAT_MSG_SYSTEM, NULL, (uint32)(m_PrematureCountDownTimer / (MINUTE * IN_MILLISECONDS)));
        }
        else
        {
            //announce every 15 seconds
            if (newtime / (15 * IN_MILLISECONDS) != m_PrematureCountDownTimer / (15 * IN_MILLISECONDS))
                PSendMessageToAll(LANG_BATTLEGROUND_PREMATURE_FINISH_WARNING_SECS, CHAT_MSG_SYSTEM, NULL, (uint32)(m_PrematureCountDownTimer / IN_MILLISECONDS));
        }
        m_PrematureCountDownTimer = newtime;
    }
}

void Battleground::ProcessJoin(uint32 diff)
{
    // *********************************************************
    // ***           BATTLEGROUND STARTING SYSTEM            ***
    // *********************************************************
    ModifyStartDelayTime(diff);

    UpdateAfkReports();

    if (!IsArena())
        SetRemainingTime(300000);

    if (m_ResetStatTimer > 5000)
    {
        m_ResetStatTimer = 0;
        for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
            if (Player* player = ObjectAccessor::FindPlayer(itr->first))
                player->ResetAllPowers();
    }

    // Send packet every 10 seconds until the 2nd field reach 0
    if (m_CountdownTimer >= 10000)
    {
        uint32 countdownMaxForBGType = IsArena() || GetTypeID() == BATTLEGROUND_SA ? ARENA_COUNTDOWN_MAX : BATTLEGROUND_COUNTDOWN_MAX;
        uint32 timer = countdownMaxForBGType - (GetElapsedTime() / 1000);

        for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
            if (Player* player = ObjectAccessor::FindPlayer(itr->first))
                player->SendStartTimer(timer, countdownMaxForBGType, TIMER_PVP);

        m_CountdownTimer = 0;
    }

    if (!(m_Events & BG_STARTING_EVENT_1))
    {
        m_Events |= BG_STARTING_EVENT_1;

        if (!FindBgMap())
        {
            TC_LOG_ERROR("bg.battleground", "Battleground::_ProcessJoin: map (map id: %u, instance id: %u) is not created!", m_MapId, m_InstanceID);
            EndNow();
            return;
        }

        // Setup here, only when at least one player has ported to the map
        if (!SetupBattleground())
        {
            EndNow();
            return;
        }

        StartingEventCloseDoors();
        SetStartDelayTime(StartDelayTimes[BG_STARTING_EVENT_FIRST]);
        // First start warning - 2 or 1 minute
        SendMessageToAll(StartMessageIds[BG_STARTING_EVENT_FIRST], CHAT_MSG_BG_SYSTEM_NEUTRAL);
    }
    // After 1 minute or 30 seconds, warning is signaled
    else if (GetStartDelayTime() <= StartDelayTimes[BG_STARTING_EVENT_SECOND] && !(m_Events & BG_STARTING_EVENT_2))
    {
        m_Events |= BG_STARTING_EVENT_2;
        SendMessageToAll(StartMessageIds[BG_STARTING_EVENT_SECOND], CHAT_MSG_BG_SYSTEM_NEUTRAL);
    }
    // After 30 or 15 seconds, warning is signaled
    else if (GetStartDelayTime() <= StartDelayTimes[BG_STARTING_EVENT_THIRD] && !(m_Events & BG_STARTING_EVENT_3))
    {
        m_Events |= BG_STARTING_EVENT_3;
        SendMessageToAll(StartMessageIds[BG_STARTING_EVENT_THIRD], CHAT_MSG_BG_SYSTEM_NEUTRAL);
    }
    // Delay expired (after 2 or 1 minute)
    else if (GetStartDelayTime() <= 0 && !(m_Events & BG_STARTING_EVENT_4))
    {
        m_Events |= BG_STARTING_EVENT_4;

        StartingEventOpenDoors();

        SendWarningToAll(StartMessageIds[BG_STARTING_EVENT_FOURTH]);
        SetStatus(STATUS_IN_PROGRESS);
        SetStartDelayTime(StartDelayTimes[BG_STARTING_EVENT_FOURTH]);

        m_doorOpeningTime = TimeValue::Now();

        // Remove preparation
        if (IsArena())
        {
            m_DampeningTimer.SetInterval(5 * MINUTE * IN_MILLISECONDS);
            m_DampeningTimer.SetCurrent(0);

            /// @todo add arena sound PlaySoundToAll(SOUND_ARENA_START);
            for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
                if (Player* player = ObjectAccessor::FindPlayer(itr->first))
                {
                    // BG Status packet
                    WorldPacket status;
                    BattlegroundQueueTypeId bgQueueTypeId = sBattlegroundMgr->BGQueueTypeId(m_TypeID, GetArenaType());
                    uint32 queueSlot = player->GetBattlegroundQueueIndex(bgQueueTypeId);

                    sBattlegroundMgr->BuildBattlegroundStatusPacket(&status, this, player, queueSlot, STATUS_IN_PROGRESS, player->GetBattlegroundQueueJoinTime(m_TypeID), GetElapsedTime(), GetArenaType());
                    player->GetSession()->SendPacket(&status);

                    player->RemoveAurasDueToSpell(SPELL_ARENA_PREPARATION);
                    if (Pet* pet = player->GetPet())
                        pet->RemoveAurasDueToSpell(SPELL_ARENA_PREPARATION);
                    for (auto&& itr : player->GetSummons())
                        itr->RemoveAurasDueToSpell(SPELL_ARENA_PREPARATION);

                    player->ResetAllPowers();
                    if (!player->IsGameMaster())
                    {
                        player->RemoveBattlegroundStartingAuras();
                        if (Pet* pet = player->GetPet())
                            pet->RemoveBattlegroundStartingAuras();
                    }
                }

            if (IsSoloQueueMatch())
            {
                if (m_Players.size() != SoloQueue::TeamSize() * 2)
                {
                    SetStatus(STATUS_WAIT_JOIN);
                    EndBattleground(WINNER_NONE);
                    return;
                }
                for (auto&& it : m_Players)
                {
                    if (it.second.OfflineRemoveTime)
                    {
                        SetStatus(STATUS_WAIT_JOIN);
                        EndBattleground(WINNER_NONE);
                        return;
                    }
                }
            }

            CheckArenaWinConditions();
        }
        else
        {
            PlaySoundToAll(SOUND_BG_START);

            for (auto&& itr : GetPlayers())
            {
                if (Player* player = ObjectAccessor::FindPlayer(itr.first))
                {
                    player->RemoveAurasDueToSpell(SPELL_PREPARATION);
                    player->ResetAllPowers();
                    if (IsRatedBG() && !player->IsGameMaster())
                    {
                        player->RemoveBattlegroundStartingAuras();
                        if (Pet* pet = player->GetPet())
                            pet->RemoveBattlegroundStartingAuras();
                    }
                }
            }
            // Announce BG starting
            if (sWorld->getBoolConfig(CONFIG_BATTLEGROUND_QUEUE_ANNOUNCER_ENABLE))
            {
                Trinity::BattlegroundStartLocalizedAnnounceBuilder builder(LANG_BG_STARTED_ANNOUNCE_WORLD, this, GetMinLevel(), GetMaxLevel());
                Trinity::LocalizedPacketListDo<Trinity::BattlegroundStartLocalizedAnnounceBuilder> sender(builder);
                SessionMap const& sessions = sWorld->GetAllSessions();
                for (SessionMap::const_iterator itr = sessions.begin(); itr != sessions.end(); ++itr)
                    if (itr->second && itr->second->GetPlayer() && itr->second->GetPlayer()->IsInWorld())
                        sender(itr->second->GetPlayer());
            }
        }
    }

    if (GetRemainingTime() > 0 && (m_EndTime -= diff) > 0)
        SetRemainingTime(GetRemainingTime() - diff);
}

void Battleground::ProcessLeave(uint32 diff)
{
    // *********************************************************
    // ***           BATTLEGROUND ENDING SYSTEM              ***
    // *********************************************************
    // remove all players from battleground after 2 minutes
    SetRemainingTime(GetRemainingTime() - diff);
    if (GetRemainingTime() <= 0)
    {
        SetRemainingTime(0);
        BattlegroundPlayerMap::iterator itr, next;
        for (itr = m_Players.begin(); itr != m_Players.end(); itr = next)
        {
            next = itr;
            ++next;
            //itr is erased here!
            RemovePlayerAtLeave(itr->first, true, true);// remove player from BG
            // do not change any battleground's private variables
        }
    }
}

inline Player* Battleground::_GetPlayer(ObjectGuid guid, bool offlineRemove, char const* context) const
{
    Player* player = NULL;
    if (!offlineRemove)
    {
        player = ObjectAccessor::FindPlayer(guid);
        if (!player)
            TC_LOG_ERROR("bg.battleground", "Battleground::%s: player (GUID: %u) not found for BG (map: %u, instance id: %u)!",
                context, guid.GetCounter(), m_MapId, m_InstanceID);
    }
    return player;
}

inline Player* Battleground::_GetPlayer(BattlegroundPlayerMap::iterator itr, char const* context)
{
    return _GetPlayer(itr->first, itr->second.OfflineRemoveTime, context);
}

inline Player* Battleground::_GetPlayer(BattlegroundPlayerMap::const_iterator itr, char const* context) const
{
    return _GetPlayer(itr->first, itr->second.OfflineRemoveTime, context);
}

inline Player* Battleground::_GetPlayerForTeam(uint32 teamId, BattlegroundPlayerMap::const_iterator itr, char const* context) const
{
    Player* player = _GetPlayer(itr, context);
    if (player)
    {
        uint32 team = itr->second.Team;
        if (!team)
            team = player->GetBGTeam();
        if (team != teamId)
            player = NULL;
    }
    return player;
}

void Battleground::SetTeamStartLoc(uint32 TeamID, float X, float Y, float Z, float O)
{
    TeamId idx = GetTeamIndexByTeamId(TeamID);
    m_TeamStartLocX[idx] = X;
    m_TeamStartLocY[idx] = Y;
    m_TeamStartLocZ[idx] = Z;
    m_TeamStartLocO[idx] = O;
}

void Battleground::SendPacketToAll(WorldPacket const* packet)
{
    for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        if (Player* player = _GetPlayer(itr, "SendPacketToAll"))
            player->SendDirectMessage(packet);
}

void Battleground::SendPacketToTeam(uint32 TeamID, WorldPacket* packet, Player* sender, bool self)
{
    for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (Player* player = _GetPlayerForTeam(TeamID, itr, "SendPacketToTeam"))
        {
            if (self || sender != player)
                player->SendDirectMessage(packet);
        }
    }
}

void Battleground::PlaySoundToAll(uint32 SoundID)
{
    SendPacketToAll(WorldPackets::Misc::PlaySound(ObjectGuid::Empty, SoundID).Write());
}

void Battleground::PlaySoundToTeam(uint32 SoundID, uint32 TeamID)
{
    for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        if (Player* player = _GetPlayerForTeam(TeamID, itr, "PlaySoundToTeam"))
            player->SendDirectMessage(WorldPackets::Misc::PlaySound(ObjectGuid::Empty, SoundID).Write());
}

void Battleground::CastSpellOnTeam(uint32 SpellID, uint32 TeamID)
{
    for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        if (Player* player = _GetPlayerForTeam(TeamID, itr, "CastSpellOnTeam"))
            player->CastSpell(player, SpellID, true);
}

void Battleground::RemoveAuraOnTeam(uint32 SpellID, uint32 TeamID)
{
    for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        if (Player* player = _GetPlayerForTeam(TeamID, itr, "RemoveAuraOnTeam"))
            player->RemoveAura(SpellID);
}

void Battleground::RewardHonorToTeam(uint32 Honor, uint32 TeamID)
{
    for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        if (Player* player = _GetPlayerForTeam(TeamID, itr, "RewardHonorToTeam"))
            UpdatePlayerScore(player, SCORE_BONUS_HONOR, Honor);
}

void Battleground::RewardReputationToTeam(uint32 faction_idA, uint32 faction_idH, uint32 Reputation, uint32 TeamID)
{
    for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        if (Player* player = _GetPlayerForTeam(TeamID, itr, "RewardReputationToTeam"))
            if (FactionEntry const* factionEntry = sFactionStore.LookupEntry(player->GetTeam() == ALLIANCE ? faction_idA : faction_idH))
                player->GetReputationMgr().ModifyReputation(factionEntry, Reputation);
}

void Battleground::UpdateWorldState(uint32 Field, uint32 Value)
{
    WorldPacket data;
    sBattlegroundMgr->BuildUpdateWorldStatePacket(&data, Field, Value);
    SendPacketToAll(&data);
}

void Battleground::UpdateWorldStateForPlayer(uint32 field, uint32 value, Player* player)
{
    WorldPacket data;
    sBattlegroundMgr->BuildUpdateWorldStatePacket(&data, field, value);
    player->SendDirectMessage(&data);
}

enum RoundResult
{
    BG_DEFAULT = 0x01,
    ARENA_NORMAL = 0x02,
    ARENA_BEFOR_BEGIN = 0x04,
    ARENA_TIMEOUT = 0x08,
    ARENA_NO_WINNERS = 0x10,
    ARENA_SOLO = 0x20,
    ARENA_NEED_REWARD = (ARENA_NORMAL | ARENA_TIMEOUT)
};

void Battleground::EndBattleground(uint32 winner)
{
    RemoveFromBGFreeSlotQueue();

    ArenaTeam* winnerArenaTeam = nullptr;
    ArenaTeam* loserArenaTeam = nullptr;

    uint32 loserTeamRating = 0;
    uint32 loserMatchmakerRating = 0;
    int32  loserChange = 0;
    int32  loserMatchmakerChange = 0;
    uint32 winnerTeamRating = 0;
    uint32 winnerMatchmakerRating = 0;
    int32  winnerChange = 0;
    int32  winnerMatchmakerChange = 0;

    int32 winmsg = 0;

    RoundResult res = BG_DEFAULT;

    if (winner == ALLIANCE)
    {
        winmsg = IsBattleground() ? LANG_BG_A_WINS : LANG_ARENA_GOLD_WINS;

        PlaySoundToAll(SOUND_ALLIANCE_WINS);                // alliance wins sound

        SetWinner(WINNER_ALLIANCE);
    }
    else if (winner == HORDE)
    {
        winmsg = IsBattleground() ? LANG_BG_H_WINS : LANG_ARENA_GREEN_WINS;

        PlaySoundToAll(SOUND_HORDE_WINS);                   // horde wins sound

        SetWinner(WINNER_HORDE);
    }
    else
    {
        SetWinner(WINNER_NONE);
    }

    BattlegroundStatus oldstatus = GetStatus();
    SetStatus(STATUS_WAIT_LEAVE);

    //we must set it this way, because end time is sent in packet!
    SetRemainingTime(TIME_AUTOCLOSE_BATTLEGROUND);

    // arena rating calculation
    if ((IsArena() && IsRated()) || IsRatedBG())
    {
        winnerArenaTeam = GetArenaTeam(winner);
        uint32 loser = winner != ALLIANCE ? ALLIANCE : HORDE;
        loserArenaTeam = GetArenaTeam(loser);

        loserTeamRating = loserArenaTeam->GetRating();
        loserMatchmakerRating = loserArenaTeam->GetMatchmakerRating();
        winnerTeamRating = winnerArenaTeam->GetRating();
        winnerMatchmakerRating = winnerArenaTeam->GetMatchmakerRating();

        if (IsSoloQueueMatch())
            res = ARENA_SOLO;
        else if (!IsRatedBG())
        {
            BattlegroundPlayerMap offline;

            res = ARENA_NORMAL;
            if (GetStatus() == STATUS_WAIT_JOIN)
                res = ARENA_BEFOR_BEGIN;
            else if (m_winner == WINNER_NONE)
                res = ARENA_TIMEOUT;

            for (auto&& itr : GetPlayers())
                if (itr.second.OfflineRemoveTime)
                    offline[itr.first] = itr.second;

            if (GetPlayers().empty() || offline.size() == GetPlayers().size())
                res = ARENA_NO_WINNERS;
            else
            {
                uint8 hordeOffline = 0;
                uint8 allianceOffline = 0;
                for (BattlegroundPlayerMap::iterator iter = offline.begin(); iter != offline.end(); ++iter)
                    if (iter->second.OfflineRemoveTime - MAX_OFFLINE_TIME < time_t(m_doorOpeningTime.ToSeconds()))
                        iter->second.Team == HORDE ? hordeOffline++ : allianceOffline++;

                if (allianceOffline == m_MaxPlayersPerTeam || hordeOffline == m_MaxPlayersPerTeam)
                    res = ARENA_BEFOR_BEGIN;
            }

            switch (res)
            {
                case ARENA_NORMAL:
                    winnerMatchmakerChange = winnerArenaTeam->WonAgainst(winnerMatchmakerRating, loserMatchmakerRating, winnerChange);
                    loserMatchmakerChange = loserArenaTeam->LostAgainst(loserMatchmakerRating, winnerMatchmakerRating, loserChange);
                    TC_LOG_DEBUG("bg.arena", "match Type: %u --- Winner: old rating: %u, rating gain: %d, old MMR: %u, MMR gain: %d --- Loser: old rating: %u, rating loss: %d, old MMR: %u, MMR loss: %d ---", m_ArenaType, winnerTeamRating, winnerChange, winnerMatchmakerRating,
                        winnerMatchmakerChange, loserTeamRating, loserChange, loserMatchmakerRating, loserMatchmakerChange);
                    if (sWorld->getBoolConfig(CONFIG_ARENA_LOG_EXTENDED_INFO))
                    {
                        for (Battleground::BattlegroundScoreMap::const_iterator itr = GetPlayerScoresBegin(); itr != GetPlayerScoresEnd(); ++itr)
                            if (Player* player = ObjectAccessor::FindPlayer(itr->first))
                            {
                                TC_LOG_DEBUG("bg.arena", "Statistics match Type: %u for %s (GUID: " UI64FMTD ", IP: %s): %u damage, %u healing, %u killing blows",
                                    m_ArenaType, player->GetName().c_str(), itr->first.GetRawValue(), player->GetSession()->GetRemoteAddress().c_str(), itr->second->DamageDone, itr->second->HealingDone,
                                    itr->second->KillingBlows);
                            }
                    }
                    break;
                case ARENA_BEFOR_BEGIN:
                    loserMatchmakerChange = loserArenaTeam->LostAgainst(loserMatchmakerRating, winnerMatchmakerRating, loserChange);

                    for (auto&& itr : *loserArenaTeam)
                        loserArenaTeam->MemberLost(itr->Guid, winnerMatchmakerRating, loserMatchmakerChange, false);
                    break;
                case ARENA_TIMEOUT:
                    // Deduct 16 points from each teams arena-rating if there are no winners after 45+2 minutes
                    winnerMatchmakerChange = ARENA_TIMELIMIT_POINTS_LOSS;
                    loserMatchmakerChange = ARENA_TIMELIMIT_POINTS_LOSS;
                    winnerChange = ARENA_TIMELIMIT_POINTS_LOSS;
                    loserChange = ARENA_TIMELIMIT_POINTS_LOSS;
                    break;
            }
        }
        m_arenaRatingInfo[GetTeamIndexByTeamId(winner)] = { winnerTeamRating, winnerTeamRating + winnerChange, winnerMatchmakerRating };
        m_arenaRatingInfo[GetTeamIndexByTeamId(loser)] = { loserTeamRating, loserTeamRating + loserChange, loserMatchmakerRating };
    }

    Group* winnerGroup = nullptr;

    BattlegroundQueueTypeId bgQueueTypeId = BattlegroundMgr::BGQueueTypeId(GetTypeID(), GetArenaType());

    uint8 aliveWinners = GetAlivePlayersCountByTeam(winner);

    for (auto&& it : m_Players)
    {
        uint32 team = it.second.Team;
        int32 ratingChanges = 0;
            
        if (it.second.OfflineRemoveTime)
        {
            // If rated arena match - make member lost!
            if (res & ARENA_NEED_REWARD)
            {
                if (IsArena() && IsRated() && winnerArenaTeam && loserArenaTeam && !IsSoloQueueMatch())
                {
                    auto arenaTeam = GetArenaTeam(team);
                    bool beforbegin = it.second.OfflineRemoveTime - MAX_OFFLINE_TIME < time_t(m_doorOpeningTime.ToSeconds());
                    if (GetArenaTeam(team) == winnerArenaTeam)
                        GetArenaTeam(team)->MemberLost(it.first, loserMatchmakerRating, winnerMatchmakerChange);
                    else
                        GetArenaTeam(team)->MemberLost(it.first, winnerMatchmakerRating, beforbegin ? 0 : loserMatchmakerChange);
                }
            }
            continue;
        }

        Player* player = _GetPlayer(it.first, it.second.OfflineRemoveTime, "EndBattleground");
        if (!player)
            continue;

        // should remove spirit of redemption
        if (player->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION))
            player->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);

        // Last standing - Rated 5v5 arena & be solely alive player
        if (team == winner && IsArena() && IsRated() && GetArenaType() == ARENA_TYPE_5v5 && aliveWinners == 1 && player->IsAlive())
            player->CastSpell(player, SPELL_THE_LAST_STANDING, true);

        if (!player->IsAlive())
        {
            player->ResurrectPlayer(100.0f); // Just to prevent a player from being killed again
            player->SpawnCorpseBones();
        }
        else
        {
            //needed cause else in av some creatures will kill the players at the end
            player->CombatStop();
            player->getHostileRefManager().deleteReferences();
        }

        // per player calculation
        if (IsArena() && IsRated() && !IsSoloQueueMatch() && res & ARENA_NEED_REWARD)
        {
            if (team == winner)
            {
                // update achievement BEFORE personal rating update
                uint32 rating = player->GetArenaPersonalRating(GetArenaTeam(team)->GetSlot());
                player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_ARENA, rating ? rating : 1, GetArenaType());
                player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_ARENA, GetMapId());
                player->ModifyCurrency(CURRENCY_TYPE_CONQUEST_META_ARENA, sWorld->getIntConfig(CONFIG_CURRENCY_CONQUEST_POINTS_ARENA_REWARD));

                // credit quest PVP-ARENA
                if ((player->GetQuestStatus(40313) == QUEST_STATUS_INCOMPLETE))
                    player->KilledMonsterCredit(300001);
                
                GetArenaTeam(team)->MemberWon(player, loserMatchmakerRating, winnerMatchmakerChange);
            }
            else
            {
                GetArenaTeam(team)->MemberLost(player->GetGUID(), winnerMatchmakerRating, loserMatchmakerChange);

                // Arena lost => reset the win_rated_arena having the "no_lose" condition
                player->ResetAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_ARENA, ACHIEVEMENT_CRITERIA_CONDITION_NO_LOSE);
            }
        }
        if (IsRatedBG())
        {
            if (team == winner)
            {
                // update achievement BEFORE personal rating update
                uint32 rating = player->GetArenaPersonalRating(GetArenaTeam(team)->GetSlot());
                player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_BATTLEGROUND, 1);
                player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_REACH_BG_RATING, rating);

                player->ModifyCurrency(CURRENCY_TYPE_CONQUEST_META_RATED_BG, sWorld->getIntConfig(CONFIG_CURRENCY_CONQUEST_POINTS_RATED_BG_REWARD));

                // credit quest PVP-RBG
                if ((player->GetQuestStatus(40325) == QUEST_STATUS_INCOMPLETE))
                    player->KilledMonsterCredit(300002);

                GetArenaTeam(team)->MemberWon(player, loserMatchmakerRating, winnerMatchmakerChange);
            }
            else
            {
                player->ModifyCurrency(CURRENCY_TYPE_CONQUEST_META_RATED_BG, GetRBGLoserReward(team));
                GetArenaTeam(team)->MemberLost(player->GetGUID(), winnerMatchmakerRating, loserMatchmakerChange);
            }
        }
        if(IsBattleground())
        {
            if (team == winner)
            {
                // credit quest PVP-BG
                if ((player->GetQuestStatus(40326) == QUEST_STATUS_INCOMPLETE))
                    player->KilledMonsterCredit(300003);
            }
        }

        bool wonRandomOrCallToArmsBG = player->GetRandomWinner() || player->GetBgWeekendWinner();
        uint32 winnerConquestBonus = sWorld->getIntConfig(wonRandomOrCallToArmsBG ? CONFIG_BG_REWARD_WINNER_CONQUEST_LAST : CONFIG_BG_REWARD_WINNER_CONQUEST_FIRST);
        uint32 winnerHonor = sWorld->getIntConfig(wonRandomOrCallToArmsBG ? CONFIG_BG_REWARD_WINNER_HONOR_LAST : CONFIG_BG_REWARD_WINNER_HONOR_FIRST);
        uint32 loserHonor = sWorld->getIntConfig(wonRandomOrCallToArmsBG ? CONFIG_BG_REWARD_LOSER_HONOR_LAST : CONFIG_BG_REWARD_LOSER_HONOR_FIRST);

        // remove temporary currency bonus auras before rewarding player
        player->RemoveAura(SPELL_HONORABLE_DEFENDER_25Y);
        player->RemoveAura(SPELL_HONORABLE_DEFENDER_60Y);

        // Reward winner team
        if (team == winner)
        {
            if (IsRandom() || (BattlegroundMgr::IsBGWeekend(GetTypeID())) && !IsRatedBG())
            {
                UpdatePlayerScore(player, SCORE_BONUS_HONOR, winnerHonor);
                player->ModifyCurrency(CURRENCY_TYPE_CONQUEST_META_ARENA, winnerConquestBonus, true, true);

                if (!wonRandomOrCallToArmsBG)
                {
                    if (IsRandom())
                        player->SetRandomWinner(true);
                    else
                        player->SetBgWeekendWinner(true);
                }
            }

            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_BG, 1, 0, 0, player);

            if (!winnerGroup)
                winnerGroup = player->GetGroup();
        }
        else
        {
            if (IsRandom() || (BattlegroundMgr::IsBGWeekend(GetTypeID()) && !IsRatedBG()))
                UpdatePlayerScore(player, SCORE_BONUS_HONOR, loserHonor);
        }

        if (!IsRatedBG() && !IsArena())
            player->AddTotalBGCount(team == winner);

        player->ResetAllPowers();
        player->CombatStopWithPets(true);

        BlockMovement(player);

        WorldPacket data;
        sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, this, player, player->GetBattlegroundQueueIndex(bgQueueTypeId), STATUS_IN_PROGRESS, player->GetBattlegroundQueueJoinTime(GetTypeID()), GetElapsedTime(), GetArenaType());
        player->SendDirectMessage(&data);

        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_BATTLEGROUND, 1);
        player->SetSaveTimer(1);
    }

    if (winnerGroup && (IsArena() || IsRatedBG()) && res & ARENA_NEED_REWARD)
    {
        Player* player;
        if (Guild* guild = winnerGroup->GetGroupGuild(GetBgMap()->GetPlayers().getFirst()->GetSource(), &player))
        {
            uint32 rating = 1;
            for (auto&& itr : *winnerArenaTeam)
                rating = std::max(itr->Rating, rating);
            if (IsArena())
            {
                guild->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_ARENA, rating, GetArenaType(), 0, nullptr, player);
                if (rating >= 2200)
                    if (AchievementEntry const* achievement = sAchievementStore.LookupEntry(4955)) // Are You Not Entertained? has no criteria tree
                        guild->GetAchievementMgr().CompletedAchievement(achievement, player);
            }
            else
            {
                guild->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_BATTLEGROUND, 1, 0, 0, nullptr, player);
                guild->CompleteGuildChallenge(CHALLENGE_RATED_BG, player);
            }
        }
    }

    if (IsSoloQueueMatch())
    {
        SetStatus(oldstatus);
        RewardSolo();
        SetStatus(STATUS_WAIT_LEAVE);
    }

    WorldPacket pvpLogData;
    sBattlegroundMgr->BuildPvpLogDataPacket(&pvpLogData, this);
    if (FindBgMap()) // Map is created only when player enter
        for (auto&& it : GetBgMap()->GetPlayers())
            it.GetSource()->SendDirectMessage(&pvpLogData);

    if (winmsg)
        SendMessageToAll(winmsg, CHAT_MSG_BG_SYSTEM_NEUTRAL);

    if ((IsRated() || IsRatedBG()) && winnerArenaTeam && loserArenaTeam)
    {
        sRatedPvpMgr->RecalcuateRank(winnerArenaTeam->GetSlot());

        TimeValue now = TimeValue::Now();
        ArenaGameStatistic stat;
        stat.ArenaType = GetArenaType();
        for (auto&& it : PlayerScores)
        {
            Player* player = ObjectAccessor::FindPlayer(it.first);

            ArenaTeam* at = winnerArenaTeam->GetMember(it.first) ? winnerArenaTeam : loserArenaTeam;
            uint8 classID;

            if (player)
            {
                classID = player->GetClass();
            }
            else
            {
                if (CharacterNameData const* data = sWorld->GetCharacterNameData(it.first))
                    classID = data->m_class;
            }

            int32 changeType;
            uint32 rating = 0;
            uint32 mmr = 0;
            if (RatedPvpInfo* member = at->GetMember(it.first))
            {
                mmr = member->MatchmakerRating;
                rating = member->Rating;
            }

            if (at == winnerArenaTeam)
            {
                changeType = 1; //win
            }
            else
            {
                changeType = 2; //lose
            }

            uint8 arenaType = IsRatedBG() ? 10 : ArenaTeam::GetTypeBySlot(at->GetSlot());

            LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_ARENA_GAMES);
            int32 index = 1;        // gameid is not known at this time.
            stmt->setInt64(  index, (at->GetSlot() + 1) * 100000000000LL + it.first.GetCounter());
            stmt->setInt32(++index, it.first.GetCounter());
            stmt->setInt32(++index, changeType);
            stmt->setInt32(++index, at->GetRatingChanges(it.first));
            stmt->setInt32(++index, rating);
            stmt->setInt32(++index, it.second->DamageDone);
            stmt->setInt32(++index, it.second->Deaths);
            stmt->setInt32(++index, it.second->HealingDone);
            stmt->setInt32(++index, it.second->DamageTaken);
            stmt->setInt32(++index, it.second->HealingTaken);
            stmt->setInt32(++index, it.second->KillingBlows);
            stmt->setInt32(++index, it.second->DamageAbsorbed);
            stmt->setInt32(++index, it.second->TimeControlled);
            stmt->setInt32(++index, it.second->AurasDispelled);
            stmt->setInt32(++index, it.second->AurasStolen);
            stmt->setInt32(++index, it.second->HighLatencyTimes);
            stmt->setInt32(++index, it.second->SpellsPrecast);
            stmt->setInt32(++index, m_MapId);
            stmt->setInt32(++index, uint32(m_doorOpeningTime ? m_doorOpeningTime.ToSeconds() : now.ToSeconds()));
            stmt->setInt32(++index, uint32(now.ToSeconds()));
            stmt->setUInt8(++index, classID);
            stmt->setUInt16(++index, sWorld->getIntConfig(CONFIG_ARENA_SEASON_ID));
            stmt->setUInt8(++index, arenaType);
            stmt->setUInt8(++index, realm.Id.Realm);
            stmt->setUInt16(++index, mmr);
            stat.Data.push_back(stmt);
        }
        sBattlegroundMgr->EnqueueNewGameStat(stat);
    }

}

uint32 Battleground::GetBonusHonorFromKill(uint32 kills) const
{
    //variable kills means how many honorable kills you scored (so we need kills * honor_for_one_kill)
    uint32 maxLevel = std::min<uint32>(GetMaxLevel(), 90U);
    return Trinity::Honor::hk_honor_at_level(maxLevel, float(kills));
}

void Battleground::BlockMovement(Player* player)
{
    player->SetClientControl(player, 0);                          // movement disabled NOTE: the effect will be automatically removed by client when the player is teleported from the battleground, so no need to send with uint8(1) in RemovePlayerAtLeave()
}

void Battleground::RemovePlayerAtLeave(ObjectGuid guid, bool Transport, bool SendPacket)
{
    uint32 team = GetPlayerTeam(guid);
    bool participant = false;
    bool deserter = false;

    // Remove from lists/maps
    BattlegroundPlayerMap::iterator itr = m_Players.find(guid);
    if (itr != m_Players.end())
    {
        UpdatePlayersCountByTeam(team, true);               // -1 player
        m_Players.erase(itr);
        // check if the player was a participant of the match, or only entered through gm command (goname)
        participant = true;
    }

    m_afkReports.erase(guid);

    if (IsBattleground() && !IsRatedBG())
    {
        BattlegroundScoreMap::iterator itr2 = PlayerScores.find(guid);
        if (itr2 != PlayerScores.end())
        {
            delete itr2->second;                                // delete player's score
            PlayerScores.erase(itr2);
        }
    }

    RemovePlayerFromResurrectQueue(guid);
    BattlegroundStatus status = GetStatus();

    Player* player = ObjectAccessor::FindPlayer(guid);

    // should remove spirit of redemption
    if (player)
    {
        player->ResetCriterias(CRITERIA_RESET_TYPE_LEAVE_BATTLEGORUND, GetMapId());

        if (player->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION))
            player->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);

        player->RemoveAurasDueToSpell(SPELL_DAMPENING);

        if (!player->IsAlive())                              // resurrect on exit
        {
            player->ResurrectPlayer(1.0f);
            player->SpawnCorpseBones();
        }
        else if (participant && IsSoloQueueMatch())
            if (status == STATUS_WAIT_JOIN || (status == STATUS_IN_PROGRESS && GetAlivePlayersCountByTeam(player->GetBGTeam()) == SoloQueue::TeamSize() - 1))
                deserter = true; // player already removed

        if (IsSoloQueueMatch())
            player->setFactionForRace(player->GetRace());
    }
    else
    {
        CharacterDatabaseTransaction trans(nullptr);
        Player::OfflineResurrect(guid, trans);
    }

    RemovePlayer(player, guid, team);                           // BG subclass specific code

    BattlegroundTypeId bgTypeId = GetTypeID();
    BattlegroundQueueTypeId bgQueueTypeId = BattlegroundMgr::BGQueueTypeId(GetTypeID(), GetArenaType());
    if (IsSoloQueueMatch())
        bgQueueTypeId = BATTLEGROUND_QUEUE_SOLO;

    if (participant) // if the player was a match participant, remove auras, calc rating, update queue
    {
        if (player)
        {
            if (!team) team = player->GetBGTeam();

            // if arena, remove the specific arena auras
            if (IsArena())
            {
                bgTypeId=BATTLEGROUND_AA;                   // set the bg type to all arenas (it will be used for queue refreshing)

                // unsummon current and summon old pet if there was one and there isn't a current pet
                player->RemovePet(PET_REMOVE_DISMISS);
                player->ResummonPetTemporaryUnSummonedIfAny();
            }
            if (SendPacket)
            {
                WorldPacket data;
                sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, this, player, player->GetBattlegroundQueueIndex(bgQueueTypeId), STATUS_NONE, player->GetBattlegroundQueueJoinTime(bgTypeId), 0, 0);
                player->SendDirectMessage(&data);
            }

            // this call is important, because player, when joins to battleground, this method is not called, so it must be called when leaving bg
            player->RemoveBattlegroundQueueId(bgQueueTypeId);
        }

        bool rated = (IsArena() && IsRated()) || IsRatedBG();
        if (rated && participant && status != STATUS_WAIT_LEAVE && !IsSoloQueueMatch())
        {
            // Arena lost => reset the win_rated_arena having the "no_loose" condition
            if (player && IsArena())
                player->ResetAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_ARENA, ACHIEVEMENT_CRITERIA_CONDITION_NO_LOSE);

            ArenaTeam* winnerArenaTeam = GetArenaTeam(GetOtherTeam(team));
            ArenaTeam* loserArenaTeam = GetArenaTeam(team);
            if (winnerArenaTeam && loserArenaTeam)
                loserArenaTeam->MemberLost(guid, winnerArenaTeam->GetMatchmakerRating());
        }

        // remove from raid group if player is member
        if (Group* group = GetBgRaid(team))
        {
            if (!group->RemoveMember(guid))                // group was disbanded
            {
                SetBgRaid(team, NULL);
            }
        }
        DecreaseInvitedCount(team);
        //we should update battleground queue, but only if bg isn't ending
        if (IsBattleground() && GetStatus() < STATUS_WAIT_LEAVE)
        {
            // a player has left the battleground, so there are free slots -> add to queue
            AddToBGFreeSlotQueue();
            sBattlegroundMgr->ScheduleQueueUpdate(0, 0, bgQueueTypeId, bgTypeId, GetBracketId());
        }
        // Let others know
        WorldPacket data;
        sBattlegroundMgr->BuildPlayerLeftBattlegroundPacket(&data, guid);
        SendPacketToTeam(team, &data, player, false);

        if (deserter)
        {
            sBattlegroundMgr->ApplyDeserter(player->GetGUID(), sWorld->getIntConfig(CONFIG_SOLO_QUEUE_DESERTER_TIMER_LEAVE));
            if (status == STATUS_WAIT_JOIN)
            {
                if (ArenaTeam* ateam = GetArenaTeam(team))
                {
                    int32 change = -int32(sWorld->getIntConfig(CONFIG_SOLO_QUEUE_DESERTER_RATING_PENALTY));
                    ateam->FinishSolo(guid, change, change, false);
                }
            }
        }
    }

    if (player)
    {
        // Do next only if found in battleground
        player->SetBattlegroundId(0, BATTLEGROUND_TYPE_NONE);  // We're not in BG.
        // reset destination bg team
        player->SetBGTeam(0);

        player->SetByteValue(PLAYER_BYTES_3, 3, 0);
        player->RemoveBattlegroundQueueJoinTime(bgTypeId);

        if (Transport)
            player->TeleportToBGEntryPoint();

        TC_LOG_DEBUG("bg.battleground", "Removed player %s from Battleground.", player->GetName().c_str());
    }

    //battleground object will be deleted next Battleground::Update() call
}

// this method is called when no players remains in battleground
void Battleground::Reset()
{
    SetWinner(WINNER_NONE);
    SetStatus(STATUS_WAIT_QUEUE);
    SetElapsedTime(0);
    SetRemainingTime(0);
    SetLastResurrectTime(0);
    m_Events = 0;

    if (m_InvitedAlliance > 0 || m_InvitedHorde > 0)
        TC_LOG_ERROR("bg.battleground", "Battleground::Reset: one of the counters is not 0 (alliance: %u, horde: %u) for BG (map: %u, instance id: %u)!",
            m_InvitedAlliance, m_InvitedHorde, m_MapId, m_InstanceID);

    m_InvitedAlliance = 0;
    m_InvitedHorde = 0;
    m_InBGFreeSlotQueue = false;

    m_Players.clear();

    for (BattlegroundScoreMap::const_iterator itr = PlayerScores.begin(); itr != PlayerScores.end(); ++itr)
        delete itr->second;
    PlayerScores.clear();

    ResetBGSubclass();
}

void Battleground::StartBattleground()
{
    SetElapsedTime(0);
    SetLastResurrectTime(0);
    // add BG to free slot queue
    AddToBGFreeSlotQueue();

    // add bg to update list
    // This must be done here, because we need to have already invited some players when first BG::Update() method is executed
    // and it doesn't matter if we call StartBattleground() more times, because m_Battlegrounds is a map and instance id never changes
    sBattlegroundMgr->AddBattleground(this);
}

void Battleground::AddPlayer(Player* player)
{
    // remove afk from player
    if (player->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_AFK))
        player->ToggleAFK();

    // score struct must be created in inherited class

    ObjectGuid guid = player->GetGUID();
    uint32 team = player->GetBGTeam();

    BattlegroundPlayer bp;
    bp.OfflineRemoveTime = 0;
    bp.Team = team;

    // Add to list/maps
    m_Players[guid] = bp;

    UpdatePlayersCountByTeam(team, false);                  // +1 player

    WorldPacket data;
    sBattlegroundMgr->BuildPlayerJoinedBattlegroundPacket(&data, player->GetGUID());
    SendPacketToTeam(team, &data, player, false);

    // BG Status packet
    BattlegroundQueueTypeId bgQueueTypeId = sBattlegroundMgr->BGQueueTypeId(m_TypeID, GetArenaType());
    uint32 queueSlot = player->GetBattlegroundQueueIndex(bgQueueTypeId);

    sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, this, player, queueSlot, STATUS_IN_PROGRESS, player->GetBattlegroundQueueJoinTime(m_TypeID), GetElapsedTime(), GetArenaType());
    player->GetSession()->SendPacket(&data);

    player->RemoveAurasByType(SPELL_AURA_MOUNTED);

    // add arena specific auras
    if (IsArena())
    {
        player->RemoveArenaEnchantments(TEMP_ENCHANTMENT_SLOT);
        if (team == ALLIANCE)                                // gold
        {
            if (team == HORDE)
                player->CastSpell(player, SPELL_HORDE_GOLD_FLAG, true);
            else
                player->CastSpell(player, SPELL_ALLIANCE_GOLD_FLAG, true);
        }
        else                                                // green
        {
            if (team == HORDE)
                player->CastSpell(player, SPELL_HORDE_GREEN_FLAG, true);
            else
                player->CastSpell(player, SPELL_ALLIANCE_GREEN_FLAG, true);
        }

        player->DestroyConjuredItems(true);
        player->UnsummonPetTemporaryIfAny();
        player->GetBattlePetMgr().UnSummonCurrentBattlePet(false);

        if (GetStatus() == STATUS_WAIT_JOIN)                 // not started yet
        {
            player->CastSpell(player, SPELL_ARENA_PREPARATION, true);
            player->ResetAllPowers();
        }

        // Set arena faction client-side to display arena unit frame
        player->SetByteValue(PLAYER_BYTES_3, 3, team == HORDE ? 0 : 1);

        if (BuildArenaPrepOpponentSpecPacket(&data, team))
            SendPacketToTeam(GetOtherTeam(team), &data);

        if (BuildArenaPrepOpponentSpecPacket(&data, GetOtherTeam(team)))
            SendPacketToTeam(team, &data);

        if (IsSoloQueueMatch() && sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_INTERFACTIONAL_TEAMS))
            player->SetFaction(team == ALLIANCE ? 1 : 2);
    }
    else
    {
        if (GetStatus() == STATUS_WAIT_JOIN)                 // not started yet
        {
            player->CastSpell(player, SPELL_PREPARATION, true);   // reduces all mana cost of spells.

            uint32 countdownMaxForBGType = IsArena() || GetTypeID() == BATTLEGROUND_SA ? ARENA_COUNTDOWN_MAX : BATTLEGROUND_COUNTDOWN_MAX;
            uint32 timer = countdownMaxForBGType - (GetElapsedTime() / 1000);
            player->SendStartTimer(timer, countdownMaxForBGType, TIMER_PVP);
        }
        if (IsRatedBG() || sWorld->getBoolConfig(CONFIG_BATTLEGROUND_IGNORE_FACTION))
        {
            uint32 spell = team == HORDE ? SPELL_BG_HORDE : SPELL_BG_ALLIANCE;
            player->CastSpell(player, spell, true);
        }
    }

    player->ResetCriterias(CRITERIA_START_TYPE_BATTLEGROUND, GetMapId());

    // setup BG group membership
    PlayerAddedToBGCheckIfBGIsRunning(player);
    AddOrSetPlayerToCorrectBgGroup(player, team);
}

// this method adds player to his team's bg group, or sets his correct group if player is already in bg group
void Battleground::AddOrSetPlayerToCorrectBgGroup(Player* player, uint32 team)
{
    ObjectGuid playerGuid = player->GetGUID();
    Group* group = GetBgRaid(team);
    if (!group)                                      // first player joined
    {
        group = new Group;
        SetBgRaid(team, group);
        group->Create(player);
    }
    else                                            // raid already exist
    {
        if (group->IsMember(playerGuid))
        {
            uint8 subgroup = group->GetMemberGroup(playerGuid);
            player->SetGroup(GroupSlot::Instance, group, subgroup);
        }
        else
        {
            group->AddMember(player);
            if (Group* originalGroup = player->GetOriginalGroup())
                if (originalGroup->IsLeader(playerGuid))
                {
                    group->ChangeLeader(playerGuid);
                    group->SendUpdate();
                }
        }
    }
}

// This method should be called when player logs into running battleground
void Battleground::EventPlayerLoggedIn(Player* player)
{
    ObjectGuid guid = player->GetGUID();
    // player is correct pointer
    for (std::deque<ObjectGuid>::iterator itr = m_OfflineQueue.begin(); itr != m_OfflineQueue.end(); ++itr)
    {
        if (*itr == guid)
        {
            m_OfflineQueue.erase(itr);
            break;
        }
    }
    m_Players[guid].OfflineRemoveTime = 0;
    if (IsBattleground() && (IsRatedBG() || sWorld->getBoolConfig(CONFIG_BATTLEGROUND_IGNORE_FACTION)))
    {
        uint32 spell = player->GetBGTeam() == HORDE ? SPELL_BG_HORDE : SPELL_BG_ALLIANCE;
        player->CastSpell(player, spell, true);
    }
    if (IsSoloQueueMatch() && sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_INTERFACTIONAL_TEAMS))
        player->SetFaction(player->GetBGTeam() == ALLIANCE ? 1 : 2);

    PlayerAddedToBGCheckIfBGIsRunning(player);
    // if battleground is starting, then add preparation aura
    // we don't have to do that, because preparation aura isn't removed when player logs out
    if (m_DampeningCounter)
        player->CastCustomSpell(SPELL_DAMPENING, SPELLVALUE_BASE_POINT0, m_DampeningCounter, player, true);
}

// This method should be called when player logs out from running battleground
void Battleground::EventPlayerLoggedOut(Player* player)
{
    ObjectGuid guid = player->GetGUID();
    if (!IsPlayerInBattleground(guid))  // Check if this player really is in battleground (might be a GM who teleported inside)
        return;

    // player is correct pointer, it is checked in WorldSession::LogoutPlayer()
    m_OfflineQueue.push_back(player->GetGUID());
    m_Players[guid].OfflineRemoveTime = sWorld->GetGameTime() + MAX_OFFLINE_TIME;
    if (GetStatus() == STATUS_IN_PROGRESS)
    {
        // drop flag and handle other cleanups
        RemovePlayer(player, guid, GetPlayerTeam(guid));

        // 1 player is logging out, if it is the last, then end arena!
        if (IsArena())
            if (GetAlivePlayersCountByTeam(player->GetBGTeam()) <= 1 && GetPlayersCountByTeam(GetOtherTeam(player->GetBGTeam())))
                EndBattleground(GetOtherTeam(player->GetBGTeam()));
    }
}

// This method should be called only once ... it adds pointer to queue
void Battleground::AddToBGFreeSlotQueue()
{
    if (!m_InBGFreeSlotQueue && IsBattleground())
    {
        sBattlegroundMgr->AddToBGFreeSlotQueue(m_TypeID, this);
        m_InBGFreeSlotQueue = true;
    }
}

// This method removes this battleground from free queue - it must be called when deleting battleground
void Battleground::RemoveFromBGFreeSlotQueue()
{
    if (m_InBGFreeSlotQueue)
    {
        sBattlegroundMgr->RemoveFromBGFreeSlotQueue(m_TypeID, m_InstanceID);
        m_InBGFreeSlotQueue = false;
    }
}

// get the number of free slots for team
// returns the number how many players can join battleground to MaxPlayersPerTeam
uint32 Battleground::GetFreeSlotsForTeam(uint32 Team) const
{
    // if BG is starting ... invite anyone
    if (GetStatus() == STATUS_WAIT_JOIN)
        return (GetInvitedCount(Team) < GetMaxPlayersPerTeam()) ? GetMaxPlayersPerTeam() - GetInvitedCount(Team) : 0;
    // if BG is already started .. do not allow to join too much players of one faction
    uint32 otherTeam;
    uint32 otherIn;
    if (Team == ALLIANCE)
    {
        otherTeam = GetInvitedCount(HORDE);
        otherIn = GetPlayersCountByTeam(HORDE);
    }
    else
    {
        otherTeam = GetInvitedCount(ALLIANCE);
        otherIn = GetPlayersCountByTeam(ALLIANCE);
    }
    if (GetStatus() == STATUS_IN_PROGRESS)
    {
        // difference based on ppl invited (not necessarily entered battle)
        // default: allow 0
        uint32 diff = 0;
        // allow join one person if the sides are equal (to fill up bg to minplayersperteam)
        if (otherTeam == GetInvitedCount(Team))
            diff = 1;
        // allow join more ppl if the other side has more players
        else if (otherTeam > GetInvitedCount(Team))
            diff = otherTeam - GetInvitedCount(Team);

        // difference based on max players per team (don't allow inviting more)
        uint32 diff2 = (GetInvitedCount(Team) < GetMaxPlayersPerTeam()) ? GetMaxPlayersPerTeam() - GetInvitedCount(Team) : 0;
        // difference based on players who already entered
        // default: allow 0
        uint32 diff3 = 0;
        // allow join one person if the sides are equal (to fill up bg minplayersperteam)
        if (otherIn == GetPlayersCountByTeam(Team))
            diff3 = 1;
        // allow join more ppl if the other side has more players
        else if (otherIn > GetPlayersCountByTeam(Team))
            diff3 = otherIn - GetPlayersCountByTeam(Team);
        // or other side has less than minPlayersPerTeam
        else if (GetInvitedCount(Team) <= GetMinPlayersPerTeam())
            diff3 = GetMinPlayersPerTeam() - GetInvitedCount(Team) + 1;

        // return the minimum of the 3 differences

        // min of diff and diff 2
        diff = std::min(diff, diff2);
        // min of diff, diff2 and diff3
        return std::min(diff, diff3);
    }
    return 0;
}

bool Battleground::HasFreeSlots() const
{
    return GetPlayersSize() < GetMaxPlayers();
}

void Battleground::UpdatePlayerScore(Player* Source, uint32 type, uint32 value, bool doAddHonor)
{
    //this procedure is called from virtual function implemented in bg subclass
    BattlegroundScoreMap::const_iterator itr = PlayerScores.find(Source->GetGUID());
    if (itr == PlayerScores.end())                         // player not found...
        return;

    switch (type)
    {
        case SCORE_KILLING_BLOWS:                           // Killing blows
            itr->second->KillingBlows += value;
            break;
        case SCORE_DEATHS:                                  // Deaths
            itr->second->Deaths += value;
            break;
        case SCORE_HONORABLE_KILLS:                         // Honorable kills
            itr->second->HonorableKills += value;
            break;
        case SCORE_BONUS_HONOR:                             // Honor bonus
            // do not add honor in arenas
            if (IsBattleground())
            {
                // reward honor instantly
                if (doAddHonor)
                    Source->RewardHonor(NULL, 1, value);    // RewardHonor calls UpdatePlayerScore with doAddHonor = false
                else
                    itr->second->BonusHonor += value;
            }
            break;
            // used only in EY, but in MSG_PVP_LOG_DATA opcode
        case SCORE_DAMAGE_DONE:                             // Damage Done
            itr->second->DamageDone += value;
            break;
        case SCORE_HEALING_DONE:                            // Healing Done
            itr->second->HealingDone += value;
            break;
        case SCORE_DAMAGE_TAKEN:
            itr->second->DamageTaken += value;              // Damage Taken
            break;
        case SCORE_HEALING_TAKEN:
            itr->second->HealingTaken += value;             // Healing Taken
            break;
        case SCORE_DAMAGE_ABSORBED:
            itr->second->DamageAbsorbed += value;
            break;
        case SCORE_TIME_CONTROLLED:
            itr->second->TimeControlled += value;
            break;
        case SCORE_AURAS_DISPELLED:
            itr->second->AurasDispelled += value;
            break;
        case SCORE_AURAS_STOLEN:
            itr->second->AurasStolen += value;
            break;
        case SCORE_HIGH_LATENCY_TIMES:
            itr->second->HighLatencyTimes += value;
            break;
        case SCORE_SPELLS_PRECAST:
            itr->second->SpellsPrecast += value;
            break;
        default:
            TC_LOG_ERROR("bg.battleground", "Battleground::UpdatePlayerScore: unknown score type (%u) for BG (map: %u, instance id: %u)!",
                type, m_MapId, m_InstanceID);
            break;
    }
}

void Battleground::AddPlayerToResurrectQueue(ObjectGuid npc_guid, ObjectGuid player_guid)
{
    m_ReviveQueue[npc_guid].push_back(player_guid);

    Player* player = ObjectAccessor::FindPlayer(player_guid);
    if (!player)
        return;

    player->CastSpell(player, SPELL_WAITING_FOR_RESURRECT, true);
}

void Battleground::RemovePlayerFromResurrectQueue(ObjectGuid player_guid)
{
    for (std::map<ObjectGuid, std::vector<ObjectGuid> >::iterator itr = m_ReviveQueue.begin(); itr != m_ReviveQueue.end(); ++itr)
    {
        for (std::vector<ObjectGuid>::iterator itr2 = (itr->second).begin(); itr2 != (itr->second).end(); ++itr2)
        {
            if (*itr2 == player_guid)
            {
                (itr->second).erase(itr2);
                if (Player* player = ObjectAccessor::FindPlayer(player_guid))
                    player->RemoveAurasDueToSpell(SPELL_WAITING_FOR_RESURRECT);
                return;
            }
        }
    }
}

void Battleground::RelocateDeadPlayers(ObjectGuid queueIndex)
{
    // Those who are waiting to resurrect at this node are taken to the closest own node's graveyard
    std::vector<ObjectGuid>& ghostList = m_ReviveQueue[queueIndex];
    if (!ghostList.empty())
    {
        WorldSafeLocsEntry const* closestGrave = NULL;
        for (std::vector<ObjectGuid>::const_iterator itr = ghostList.begin(); itr != ghostList.end(); ++itr)
        {
            Player* player = ObjectAccessor::FindPlayer(*itr);
            if (!player)
                continue;

            if (!closestGrave)
                closestGrave = GetClosestGraveYard(player);

            if (closestGrave)
                player->TeleportTo(GetMapId(), closestGrave->x, closestGrave->y, closestGrave->z, player->GetOrientation());
        }
        ghostList.clear();
    }
}

bool Battleground::AddObject(uint32 type, uint32 entry, float x, float y, float z, float o, G3D::Quat const& rotation, uint32 /*respawnTime*/, GOState goState)
{
    // If the assert is called, means that BgObjects must be resized!
    ASSERT(type < BgObjects.size());

    Map* map = FindBgMap();
    if (!map)
        return false;

    if (GameObjectTemplate const* info = sObjectMgr->GetGameObjectTemplate(entry))
    {
        if (info->type == GAMEOBJECT_TYPE_TRANSPORT)
        {
            if (Transport* transport = sTransportMgr->CreateLocalTransport(entry, GetBgMap(), x, y, z, o, 255))
            {
                BgObjects[type] = transport->GetGUID();
                return true;
            }
            return false;
        }
    }

    // Must be created this way, adding to godatamap would add it to the base map of the instance
    // and when loading it (in go::LoadFromDB()), a new guid would be assigned to the object, and a new object would be created
    // So we must create it specific for this instance
    GameObject* go = new GameObject;
    if (!go->Create(m_Map->GenerateLowGuid<HighGuid::GameObject>(), entry, GetBgMap(), PHASEMASK_NORMAL, x, y, z, o, rotation, 100, goState))
    {
        TC_LOG_ERROR("sql.sql", "Battleground::AddObject: cannot create gameobject (entry: %u) for BG (map: %u, instance id: %u)!",
                entry, m_MapId, m_InstanceID);
        TC_LOG_ERROR("bg.battleground", "Battleground::AddObject: cannot create gameobject (entry: %u) for BG (map: %u, instance id: %u)!",
                entry, m_MapId, m_InstanceID);
        delete go;
        return false;
    }

    for (auto phase : go->GetPhases())
        go->SetPhased(phase, false, true);
/*
    uint32 guid = go->GetGUIDLow();

    // without this, UseButtonOrDoor caused the crash, since it tried to get go info from godata
    // iirc that was changed, so adding to go data map is no longer required if that was the only function using godata from GameObject without checking if it existed
    GameObjectData& data = sObjectMgr->NewGOData(guid);

    data.id             = entry;
    data.mapid          = GetMapId();
    data.posX           = x;
    data.posY           = y;
    data.posZ           = z;
    data.orientation    = o;
    data.rotation0      = rotation0;
    data.rotation1      = rotation1;
    data.rotation2      = rotation2;
    data.rotation3      = rotation3;
    data.spawntimesecs  = respawnTime;
    data.spawnMask      = 1;
    data.animprogress   = 100;
    data.go_state       = 1;
*/
    // Add to world, so it can be later looked up from HashMapHolder
    if (!map->AddToMap(go))
    {
        delete go;
        return false;
    }
    BgObjects[type] = go->GetGUID();
    return true;
}

// Some doors aren't despawned so we cannot handle their closing in gameobject::update()
// It would be nice to correctly implement GO_ACTIVATED state and open/close doors in gameobject code
void Battleground::DoorClose(uint32 type)
{
    if (GameObject* obj = GetBgMap()->GetGameObject(BgObjects[type]))
    {
        // If doors are open, close it
        if (obj->getLootState() == GO_ACTIVATED && obj->GetGoState() != GO_STATE_READY)
        {
            obj->SetLootState(GO_READY);
            obj->SetGoState(GO_STATE_READY);
        }
    }
    else
        TC_LOG_ERROR("bg.battleground", "Battleground::DoorClose: door gameobject (type: %u, GUID: %u) not found for BG (map: %u, instance id: %u)!",
            type, BgObjects[type].GetCounter(), m_MapId, m_InstanceID);
}

void Battleground::DoorOpen(uint32 type)
{
    if (GameObject* obj = GetBgMap()->GetGameObject(BgObjects[type]))
    {
        obj->SetLootState(GO_ACTIVATED);
        obj->SetGoState(GO_STATE_ACTIVE);
    }
    else
        TC_LOG_ERROR("bg.battleground", "Battleground::DoorOpen: door gameobject (type: %u, GUID: %u) not found for BG (map: %u, instance id: %u)!",
            type, BgObjects[type].GetCounter(), m_MapId, m_InstanceID);
}

GameObject* Battleground::GetBGObject(uint32 type)
{
    GameObject* obj = GetBgMap()->GetGameObject(BgObjects[type]);
    if (!obj)
        TC_LOG_ERROR("bg.battleground", "Battleground::GetBGObject: gameobject (type: %u, GUID: %u) not found for BG (map: %u, instance id: %u)!",
            type, BgObjects[type].GetCounter(), m_MapId, m_InstanceID);
    return obj;
}

Creature* Battleground::GetBGCreature(uint32 type)
{
    Creature* creature = GetBgMap()->GetCreature(BgCreatures[type]);
    if (!creature)
        TC_LOG_ERROR("bg.battleground", "Battleground::GetBGCreature: creature (type: %u, GUID: %u) not found for BG (map: %u, instance id: %u)!",
            type, BgCreatures[type].GetCounter(), m_MapId, m_InstanceID);
    return creature;
}

void Battleground::SpawnBGObject(uint32 type, uint32 respawntime)
{
    if (Map* map = FindBgMap())
        if (GameObject* obj = map->GetGameObject(BgObjects[type]))
        {
            if (respawntime)
                obj->SetLootState(GO_JUST_DEACTIVATED);
            else
                if (obj->getLootState() == GO_JUST_DEACTIVATED)
                    // Change state from GO_JUST_DEACTIVATED to GO_READY in case battleground is starting again
                    obj->SetLootState(GO_READY);
            obj->SetRespawnTime(respawntime);
            map->AddToMap(obj);
        }
}

Creature* Battleground::AddCreature(uint32 entry, uint32 type, uint32 teamval, float x, float y, float z, float o, uint32 respawntime, Transport* transport)
{
    // If the assert is called, means that BgCreatures must be resized!
    ASSERT(type < BgCreatures.size());

    Map* map = FindBgMap();
    if (!map)
        return NULL;

    if (transport)
    {
        if (Creature* creature = transport->SummonPassenger(entry, { x, y, z, o }, TEMPSUMMON_MANUAL_DESPAWN))
        {
            BgCreatures[type] = creature->GetGUID();
            return creature;
        }

        return NULL;
    }

    Creature* creature = new Creature;
    if (!creature->Create(map->GenerateLowGuid<HighGuid::Unit>(), map, PHASEMASK_NORMAL, entry, 0, teamval, x, y, z, o))
    {
        TC_LOG_ERROR("bg.battleground", "Battleground::AddCreature: cannot create creature (entry: %u) for BG (map: %u, instance id: %u)!",
            entry, m_MapId, m_InstanceID);
        delete creature;
        return NULL;
    }

    creature->SetHomePosition(x, y, z, o);

    CreatureTemplate const* cinfo = sObjectMgr->GetCreatureTemplate(entry);
    if (!cinfo)
    {
        TC_LOG_ERROR("bg.battleground", "Battleground::AddCreature: creature template (entry: %u) does not exist for BG (map: %u, instance id: %u)!",
            entry, m_MapId, m_InstanceID);
        delete creature;
        return NULL;
    }
    // Force using DB speeds
    creature->SetSpeed(MOVE_WALK,  cinfo->speed_walk);
    creature->SetSpeed(MOVE_RUN,   cinfo->speed_run);

    for (auto phase : creature->GetPhases())
        creature->SetPhased(phase, false, true);

    if (!map->AddToMap(creature))
    {
        delete creature;
        return NULL;
    }

    BgCreatures[type] = creature->GetGUID();

    if (respawntime)
        creature->SetRespawnDelay(respawntime);

    return creature;
}

bool Battleground::DelCreature(uint32 type)
{
    if (!BgCreatures[type])
        return true;

    if (Creature* creature = GetBgMap()->GetCreature(BgCreatures[type]))
    {
        creature->AddObjectToRemoveList();
        BgCreatures[type].Clear();
        return true;
    }

    TC_LOG_ERROR("bg.battleground", "Battleground::DelCreature: creature (type: %u, GUID: %u) not found for BG (map: %u, instance id: %u)!",
        type, BgCreatures[type].GetCounter(), m_MapId, m_InstanceID);
    BgCreatures[type].Clear();
    return false;
}

bool Battleground::DelObject(uint32 type)
{
    if (!BgObjects[type])
        return true;

    if (GameObject* obj = GetBgMap()->GetGameObject(BgObjects[type]))
    {
        if (obj->GetGoType() == GAMEOBJECT_TYPE_TRANSPORT)
        {
            obj->AddObjectToRemoveList();
            BgObjects[type].Clear();
            return true;
        }
        obj->SetRespawnTime(0);                                 // not save respawn time
        obj->Delete();
        BgObjects[type].Clear();
        return true;
    }
    TC_LOG_ERROR("bg.battleground", "Battleground::DelObject: gameobject (type: %u, GUID: %u) not found for BG (map: %u, instance id: %u)!",
        type, BgObjects[type].GetCounter(), m_MapId, m_InstanceID);
    BgObjects[type].Clear();
    return false;
}

bool Battleground::AddSpiritGuide(uint32 type, float x, float y, float z, float o, uint32 team)
{
    uint32 entry = (team == ALLIANCE) ?
        BG_CREATURE_ENTRY_A_SPIRITGUIDE :
        BG_CREATURE_ENTRY_H_SPIRITGUIDE;

    if (Creature* creature = AddCreature(entry, type, team, x, y, z, o))
    {
        creature->setDeathState(DEAD);
        creature->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, creature->GetGUID());
        // aura
        /// @todo Fix display here
        // creature->SetVisibleAura(0, SPELL_SPIRIT_HEAL_CHANNEL);
        // casting visual effect
        creature->SetUInt32Value(UNIT_FIELD_CHANNEL_SPELL, SPELL_SPIRIT_HEAL_CHANNEL);
        // correct cast speed
        creature->SetFloatValue(UNIT_FIELD_MOD_CASTING_SPEED, 1.0f);
        creature->SetFloatValue(UNIT_FIELD_MOD_SPELL_HASTE, 1.0f);
        //creature->CastSpell(creature, SPELL_SPIRIT_HEAL_CHANNEL, true);
        return true;
    }
    TC_LOG_ERROR("bg.battleground", "Battleground::AddSpiritGuide: cannot create spirit guide (type: %u, entry: %u) for BG (map: %u, instance id: %u)!",
        type, entry, m_MapId, m_InstanceID);
    EndNow();
    return false;
}

void Battleground::SendMessageToAll(int32 entry, ChatMsg type, Player const* source)
{
    if (!entry)
        return;

    Trinity::BattlegroundChatBuilder bg_builder(type, entry, source);
    Trinity::LocalizedPacketDo<Trinity::BattlegroundChatBuilder> bg_do(bg_builder);
    BroadcastWorker(bg_do);
}

void Battleground::PSendMessageToAll(int32 entry, ChatMsg type, Player const* source, ...)
{
    if (!entry)
        return;

    va_list ap;
    va_start(ap, source);

    Trinity::BattlegroundChatBuilder bg_builder(type, entry, source, &ap);
    Trinity::LocalizedPacketDo<Trinity::BattlegroundChatBuilder> bg_do(bg_builder);
    BroadcastWorker(bg_do);

    va_end(ap);
}

void Battleground::SendWarningToAll(int32 entry, ...)
{
    if (!entry)
        return;

    std::map<uint32, WorldPacket> localizedPackets;

    for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        if (Player* player = ObjectAccessor::FindPlayer(itr->first))
        {
            if (localizedPackets.find(player->GetSession()->GetSessionDbLocaleIndex()) == localizedPackets.end())
            {
                char const* format = sObjectMgr->GetTrinityString(entry, player->GetSession()->GetSessionDbLocaleIndex());

                char str[1024];
                va_list ap;
                va_start(ap, entry);
                vsnprintf(str, 1024, format, ap);
                va_end(ap);

                ChatHandler::BuildChatPacket(localizedPackets[player->GetSession()->GetSessionDbLocaleIndex()], CHAT_MSG_RAID_BOSS_EMOTE, LANG_UNIVERSAL, NULL, NULL, str);
            }

            player->SendDirectMessage(&localizedPackets[player->GetSession()->GetSessionDbLocaleIndex()]);
        }
}

void Battleground::SendMessage2ToAll(int32 entry, ChatMsg type, Player const* source, int32 arg1, int32 arg2)
{
    Trinity::Battleground2ChatBuilder bg_builder(type, entry, source, arg1, arg2);
    Trinity::LocalizedPacketDo<Trinity::Battleground2ChatBuilder> bg_do(bg_builder);
    BroadcastWorker(bg_do);
}

void Battleground::ReportAfk(ObjectGuid target, Player* reporter)
{
    if (m_Players.find(target) == m_Players.end())
        return;

    auto& report = m_afkReports[target];
    if (report.Reporters.find(reporter->GetGUID()) != report.Reporters.end())
        return;

    Player* player = ObjectAccessor::FindPlayer(target);

    if (player)
    {
        if (GetStatus() != STATUS_IN_PROGRESS)
            return;
        if (player->HasAura(43680) || player->HasAura(43681))
            return;
    }
    else if (report.ReportTime != TimeValue::zero())
        return;

    if (!reporter->CanReportAfkDueToLimit())
        return;

    report.Reporters.insert(reporter->GetGUID());
    if (report.Reporters.size() >= 3)
    {
        if (player)
        {
            // cast 'Idle' spell
            player->CastSpell(player, 43680, true);
        }
        else
            report.ReportTime = TimeValue::Now();

        report.Reporters.clear();
    }
}

void Battleground::RewardSolo()
{
    ArenaTeam* winnerArenaTeam = GetArenaTeam(m_winner == WINNER_ALLIANCE ? ALLIANCE : HORDE);
    ArenaTeam* loserArenaTeam = GetArenaTeam(m_winner == WINNER_ALLIANCE ? HORDE : ALLIANCE);

    uint32 winnerArenaTeamRating = winnerArenaTeam->GetRating();
    uint32 winnerMatchMakerRating = winnerArenaTeam->GetMatchmakerRating();

    uint32 loserArenaTeamRating = loserArenaTeam->GetRating();
    uint32 loserMatchMakerRating = loserArenaTeam->GetMatchmakerRating();

    enum class RoundResult
    {
        Normal,
        NoWinner,
        Unfulfilled,
    };

    BattlegroundPlayerMap offline;
    RoundResult res = RoundResult::Normal;
    if (GetStatus() == STATUS_WAIT_JOIN)
    {
        for (auto&& itr : m_Players)
        {
            if (itr.second.OfflineRemoveTime)
            {
                sBattlegroundMgr->ApplyDeserter(itr.first, sWorld->getIntConfig(CONFIG_SOLO_QUEUE_DESERTER_TIMER_LEAVE));
                if (ArenaTeam* at = GetArenaTeam(itr.second.Team))
                {
                    int32 change = -int32(sWorld->getIntConfig(CONFIG_SOLO_QUEUE_DESERTER_RATING_PENALTY));
                    at->FinishSolo(itr.first, change, change, false);
                }
            }
        }
        return;
    }
    else if (m_winner == WINNER_NONE)
        res = RoundResult::NoWinner;

    for (auto&& it : m_Players)
        if (it.second.OfflineRemoveTime)
            offline[it.first] = it.second;

    if (m_Players.empty() || offline.size() == m_Players.size())
        return;
    else
    {
        uint8 hordeOffline = 0;
        uint8 allianceOffline = 0;
        for (BattlegroundPlayerMap::iterator iter = offline.begin(); iter != offline.end(); ++iter)
            if (iter->second.OfflineRemoveTime - MAX_OFFLINE_TIME < m_StartTime)
                iter->second.Team == HORDE ? hordeOffline++ : allianceOffline++;

        if (allianceOffline == m_MaxPlayersPerTeam || hordeOffline == m_MaxPlayersPerTeam)
            res = RoundResult::Unfulfilled;
    }

    auto& queue = sBattlegroundMgr->GetSoloQueue();

    std::vector<RatedPvpInfo*> winners;
    std::vector<RatedPvpInfo*> losers;
    time_t t = time(nullptr) + sWorld->getIntConfig(CONFIG_SOLO_QUEUE_TEAMMATE_BLOCKING_TIMER);
    for (auto&& itr : *winnerArenaTeam)
    {
        if (res == RoundResult::NoWinner)
            losers.push_back(itr);
        else
            winners.push_back(itr);
    }

    for (auto&& itr : *loserArenaTeam)
        losers.push_back(itr);

    for (auto&& at : { winnerArenaTeam, loserArenaTeam })
    {
        for (auto&& itr : *at)
        {
            auto& rule = queue.GetRuleFor(itr->Guid);
            rule.flush();
            for (auto&& itr2 : *GetArenaTeam(m_winner == WINNER_ALLIANCE ? HORDE : ALLIANCE))
                if (itr2->Guid != itr->Guid)
                    rule[itr2->Guid] = t;
        }
    }

    auto minelement = [](RatedPvpInfo const* lhs, RatedPvpInfo const* rhs)
    {
        return lhs->MatchmakerRating < rhs->MatchmakerRating;
    };

    auto minitr = std::min_element(losers.begin(), losers.end(), minelement);
    uint32 min = (*minitr)->MatchmakerRating;
    uint32 firstthreshold = 0.5 * sWorld->getIntConfig(CONFIG_SOLO_QUEUE_MATCH_MAKER_RATING_DIFFERENCE);
    uint32 secondthreshold = 0.66 * sWorld->getIntConfig(CONFIG_SOLO_QUEUE_MATCH_MAKER_RATING_DIFFERENCE);

    for (auto&& itr : winners)
    {
        winnerArenaTeam->WinSolo(itr->Guid, loserMatchMakerRating);

        Player* player = ObjectAccessor::FindPlayer(itr->Guid);
        if (player && player->GetMap() == GetBgMap())
        {
            if (sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_ALLOW_ACHIEVEMENTS))
            {
                uint32 rating = itr->Rating;
                player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_ARENA, rating ? rating : 1, ARENA_TEAM_3v3);
                player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_ARENA, GetMapId());
                player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_PLAY_ARENA, 1, ARENA_TEAM_5v5);
                player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_PLAY_ARENA, 1, ARENA_TEAM_3v3);
                player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_PERSONAL_RATING, rating, ARENA_TEAM_5v5);
                player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_PERSONAL_RATING, rating, ARENA_TEAM_3v3);
                player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_TEAM_RATING, rating, ARENA_TEAM_3v3);
                player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_TEAM_RATING, rating, ARENA_TEAM_5v5);
            }
            player->ModifyCurrency(CURRENCY_TYPE_CONQUEST_META_ARENA, sWorld->getIntConfig(CONFIG_CURRENCY_CONQUEST_POINTS_ARENA_REWARD));
        }
    }

    minitr = std::min_element(losers.begin(), losers.end(), minelement);
    min = (*minitr)->MatchmakerRating;

    for (auto&& itr : losers)
    {
        Player* player = ObjectAccessor::FindConnectedPlayer(itr->Guid);

        if (res != RoundResult::Normal)
        {
            loserArenaTeam->FinishSolo(itr->Guid, ARENA_TIMELIMIT_POINTS_LOSS, ARENA_TIMELIMIT_POINTS_LOSS, false);
        }
        else if (itr->MatchmakerRating - min > firstthreshold)
        {
            int32 threshold = -16;
            if (itr->MatchmakerRating > loserMatchMakerRating)
            {
                if (itr->MatchmakerRating - loserMatchMakerRating > secondthreshold)
                    threshold = -14;
            }

            int32 ratingChange = ArenaTeam::GetRatingMod(itr->Rating, winnerMatchMakerRating, false);
            ratingChange = std::max(ratingChange, threshold);
            loserArenaTeam->FinishSolo(itr->Guid, ratingChange, ratingChange, false);
        }
        else
        {
            loserArenaTeam->LoseSolo(itr->Guid, winnerMatchMakerRating);
        }

        if (player && player->GetMap() == GetBgMap())
        {
            player->GetAchievementMgr().ResetAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_ARENA, ACHIEVEMENT_CRITERIA_CONDITION_NO_LOSE);
            player->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_PLAY_ARENA, 1, ARENA_TYPE_5v5);
            player->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_PERSONAL_RATING, itr->Rating, ARENA_TYPE_5v5);
        }
    }

    sRatedPvpMgr->RecalcuateRank(PVP_SLOT_ARENA_5v5);
}

void Battleground::EndNow()
{
    RemoveFromBGFreeSlotQueue();
    SetStatus(STATUS_WAIT_LEAVE);
    SetRemainingTime(0);
}

// To be removed
char const* Battleground::GetTrinityString(int32 entry)
{
    // FIXME: now we have different DBC locales and need localized message for each target client
    return sObjectMgr->GetTrinityStringForDBCLocale(entry);
}

// IMPORTANT NOTICE:
// buffs aren't spawned/despawned when players captures anything
// buffs are in their positions when battleground starts
void Battleground::HandleTriggerBuff(ObjectGuid go_guid)
{
    GameObject* obj = GetBgMap()->GetGameObject(go_guid);
    if (!obj || obj->GetGoType() != GAMEOBJECT_TYPE_TRAP || !obj->isSpawned())
        return;

    // Change buff type, when buff is used:
    int32 index = BgObjects.size() - 1;
    while (index >= 0 && BgObjects[index] != go_guid)
        index--;
    if (index < 0)
    {
        TC_LOG_ERROR("bg.battleground", "Battleground::HandleTriggerBuff: cannot find buff gameobject (GUID: %u, entry: %u, type: %u) in internal data for BG (map: %u, instance id: %u)!",
            go_guid.GetCounter(), obj->GetEntry(), obj->GetGoType(), m_MapId, m_InstanceID);
        return;
    }

    // Randomly select new buff
    uint8 buff = urand(0, 2);
    uint32 entry = obj->GetEntry();
    if (m_BuffChange && entry != Buff_Entries[buff])
    {
        // Despawn current buff
        SpawnBGObject(index, RESPAWN_ONE_DAY);
        // Set index for new one
        for (uint8 currBuffTypeIndex = 0; currBuffTypeIndex < 3; ++currBuffTypeIndex)
            if (entry == Buff_Entries[currBuffTypeIndex])
            {
                index -= currBuffTypeIndex;
                index += buff;
            }
    }

    SpawnBGObject(index, BUFF_RESPAWN_TIME);
}

void Battleground::HandleKillPlayer(Player* victim, Player* killer)
{
    // Keep in mind that for arena this will have to be changed a bit

    // Add +1 deaths
    UpdatePlayerScore(victim, SCORE_DEATHS, 1);
    // Add +1 kills to group and +1 killing_blows to killer
    if (killer)
    {
        // Don't reward credit for killing ourselves, like fall damage of hellfire (warlock)
        if (killer == victim)
            return;

        UpdatePlayerScore(killer, SCORE_HONORABLE_KILLS, 1);
        UpdatePlayerScore(killer, SCORE_KILLING_BLOWS, 1);

        for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        {
            Player* creditedPlayer = ObjectAccessor::FindPlayer(itr->first);
            if (!creditedPlayer || creditedPlayer == killer)
                continue;

            if (creditedPlayer->GetBGTeam() == killer->GetBGTeam() && creditedPlayer->IsAtGroupRewardDistance(victim))
                UpdatePlayerScore(creditedPlayer, SCORE_HONORABLE_KILLS, 1);
        }
    }

    if (!IsArena())
    {
        // To be able to remove insignia -- ONLY IN Battlegrounds
        victim->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);
        RewardXPAtKill(killer, victim);
    }
}

// Return the player's team based on battlegroundplayer info
// Used in same faction arena matches mainly
uint32 Battleground::GetPlayerTeam(ObjectGuid guid) const
{
    BattlegroundPlayerMap::const_iterator itr = m_Players.find(guid);
    if (itr != m_Players.end())
        return itr->second.Team;
    return 0;
}

uint32 Battleground::GetOtherTeam(uint32 teamId) const
{
    return teamId ? ((teamId == ALLIANCE) ? HORDE : ALLIANCE) : 0;
}

bool Battleground::IsPlayerInBattleground(ObjectGuid guid) const
{
    BattlegroundPlayerMap::const_iterator itr = m_Players.find(guid);
    if (itr != m_Players.end())
        return true;
    return false;
}

void Battleground::PlayerAddedToBGCheckIfBGIsRunning(Player* player)
{
    if (GetStatus() != STATUS_WAIT_LEAVE)
        return;

    WorldPacket data;
    BattlegroundQueueTypeId bgQueueTypeId = BattlegroundMgr::BGQueueTypeId(GetTypeID(), GetArenaType());

    BlockMovement(player);

    sBattlegroundMgr->BuildPvpLogDataPacket(&data, this);
    player->SendDirectMessage(&data);

    sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, this, player, player->GetBattlegroundQueueIndex(bgQueueTypeId), STATUS_IN_PROGRESS, player->GetBattlegroundQueueJoinTime(GetTypeID()), GetElapsedTime(), GetArenaType());
    player->SendDirectMessage(&data);
}

uint32 Battleground::GetAlivePlayersCountByTeam(uint32 Team) const
{
    int count = 0;
    for (BattlegroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.Team == Team)
        {
            Player* player = ObjectAccessor::FindPlayer(itr->first);
            if (player && player->IsAlive() && !player->HasByteFlag(UNIT_FIELD_BYTES_2, 3, FORM_SPIRITOFREDEMPTION))
                ++count;
        }
    }
    return count;
}

void Battleground::SetHoliday(bool is_holiday)
{
    m_HonorMode = is_holiday ? BG_HOLIDAY : BG_NORMAL;
}

int32 Battleground::GetObjectType(ObjectGuid guid)
{
    for (uint32 i = 0; i < BgObjects.size(); ++i)
        if (BgObjects[i] == guid)
            return i;
    TC_LOG_ERROR("bg.battleground", "Battleground::GetObjectType: player used gameobject (GUID: %u) which is not in internal data for BG (map: %u, instance id: %u), cheating?",
        guid.GetCounter(), m_MapId, m_InstanceID);
    return -1;
}

void Battleground::HandleKillUnit(Creature* /*victim*/, Player* /*killer*/) { }

void Battleground::CheckArenaAfterTimerConditions()
{
    EndBattleground(WINNER_NONE);
}

void Battleground::CheckArenaWinConditions()
{
    if (!GetAlivePlayersCountByTeam(ALLIANCE) && GetPlayersCountByTeam(HORDE))
        EndBattleground(HORDE);
    else if (GetPlayersCountByTeam(ALLIANCE) && !GetAlivePlayersCountByTeam(HORDE))
        EndBattleground(ALLIANCE);
}

void Battleground::UpdateArenaWorldState()
{
    UpdateWorldState(0xe10, GetAlivePlayersCountByTeam(HORDE));
    UpdateWorldState(0xe11, GetAlivePlayersCountByTeam(ALLIANCE));
}

void Battleground::SetBgRaid(uint32 TeamID, Group* bg_raid)
{
    Group*& old_raid = TeamID == ALLIANCE ? m_BgRaids[TEAM_ALLIANCE] : m_BgRaids[TEAM_HORDE];
    if (old_raid)
        old_raid->SetBattlegroundGroup(NULL);
    if (bg_raid)
        bg_raid->SetBattlegroundGroup(this);
    old_raid = bg_raid;
}

WorldSafeLocsEntry const* Battleground::GetClosestGraveYard(Player* player)
{
    return sObjectMgr->GetClosestGraveYard(*player, player->GetBGTeam(), player);
}

void Battleground::StartCriteria(CriteriaStartTypes type, uint32 entry)
{
    for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
        if (Player* player = ObjectAccessor::FindPlayer(itr->first))
            player->StartCriteria(type, entry);
}

void Battleground::SetBracket(PvPDifficultyEntry const* bracketEntry)
{
    m_BracketId = bracketEntry->GetBracketId();
    SetLevelRange(bracketEntry->minLevel, bracketEntry->maxLevel);
}

void Battleground::RewardXPAtKill(Player* killer, Player* victim)
{
    if (sWorld->getBoolConfig(CONFIG_BG_XP_FOR_KILL) && killer && victim)
        killer->RewardPlayerAndGroupAtKill(victim, true);
}

uint32 Battleground::GetTeamScore(uint32 teamId) const
{
    if (teamId == TEAM_ALLIANCE || teamId == TEAM_HORDE)
        return m_TeamScores[teamId];

    TC_LOG_ERROR("bg.battleground", "GetTeamScore with wrong Team %u for BG %u", teamId, GetTypeID());
    return 0;
}

void Battleground::HandleAreaTrigger(Player* player, uint32 trigger, bool /*entered*/)
{
    TC_LOG_DEBUG("bg.battleground", "Unhandled AreaTrigger %u in Battleground %u. Player coords (x: %f, y: %f, z: %f)",
                   trigger, player->GetMapId(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ());
}

bool Battleground::CheckAchievementCriteriaMeet(uint32 criteriaId, Player const* /*source*/, Unit const* /*target*/, uint32 /*miscvalue1*/)
{
    TC_LOG_ERROR("bg.battleground", "Battleground::CheckAchievementCriteriaMeet: No implementation for criteria %u", criteriaId);
    return false;
}

void Battleground::SendFlagsPositions()
{
    std::list<ObjectGuid> players;

    if (m_MapId != 998)
    {
        if (ObjectGuid guid = GetFlagPickerGUID(TEAM_ALLIANCE))
            players.push_back(guid);
        if (ObjectGuid guid = GetFlagPickerGUID(TEAM_HORDE))
            players.push_back(guid);
    }
    else if (BattlegroundTOK* tok = dynamic_cast<BattlegroundTOK*>(this))
    {
        for (auto&& guid : tok->GetOrbPickersGUID())
            if (guid)
                players.push_back(guid);
    }

    ByteBuffer dataBuffer;
    WorldPacket data(SMSG_BATTLEGROUND_PLAYER_POSITIONS);

    data.WriteBits(players.size(), 20);

    for (auto&& itr : players)
    {
        Player* player = ObjectAccessor::FindPlayer(itr);
        if (!player)
            continue;

        ObjectGuid guid = player->GetGUID();

        data.WriteBit(guid[1]);
        data.WriteBit(guid[5]);
        data.WriteBit(guid[0]);
        data.WriteBit(guid[4]);
        data.WriteBit(guid[6]);
        data.WriteBit(guid[3]);
        data.WriteBit(guid[7]);
        data.WriteBit(guid[2]);

        dataBuffer.WriteByteSeq(guid[0]);
        dataBuffer.WriteByteSeq(guid[3]);
        dataBuffer.WriteByteSeq(guid[7]);
        dataBuffer << uint8(player->GetBGTeam() == ALLIANCE ? 1 : 2); // team
        dataBuffer << float(player->GetPositionY());
        dataBuffer.WriteByteSeq(guid[4]);
        dataBuffer.WriteByteSeq(guid[2]);
        dataBuffer.WriteByteSeq(guid[1]);
        dataBuffer.WriteByteSeq(guid[5]);

        dataBuffer << float(player->GetPositionX());
        dataBuffer << uint8(player->GetBGTeam() == ALLIANCE ? 3 : 2); // icon ( team 1 - 3 or 4, team 2 - 2 or 5)
        dataBuffer.WriteByteSeq(guid[6]);
    }

    data.FlushBits();

    if (dataBuffer.size())
        data.append(dataBuffer);

    SendPacketToAll(&data);
}

void Battleground::SendClearFlagsPositions()
{
    WorldPacket data(SMSG_BATTLEGROUND_PLAYER_POSITIONS, 3);
    data.WriteBits(0, 20);
    SendPacketToAll(&data);
}

void Battleground::RemovePlayer(Player* player, ObjectGuid guid, uint32 team)
{
    if (!player)
        return;
    if (sWorld->getBoolConfig(CONFIG_BATTLEGROUND_IGNORE_FACTION))
    {
        player->setFactionForRace(player->GetRace());
        player->InitDisplayIds();
        player->RemoveAurasDueToSpell(200002);
        player->RemoveAurasDueToSpell(200003);
    }
}

uint32 Battleground::BuildArenaPrepOpponentSpecPacket(WorldPacket* data, uint32 team)
{
    uint32 count = 0;
    ByteBuffer buffer;

    data->Initialize(SMSG_ARENA_PREP_OPPONENT_SPECIALIZATIONS);
    data->WriteBits(count, 21);
    for (auto itr = m_Players.begin(); itr != m_Players.end(); itr++)
    {
        Player* player = _GetPlayerForTeam(team, itr, "BuildArenaPrepOpponentSpecPacket");
        if (!player)
            continue;

        ObjectGuid guid = player->GetGUID();

        data->WriteBit(guid[7]);
        data->WriteBit(guid[1]);
        data->WriteBit(guid[2]);
        data->WriteBit(guid[0]);
        data->WriteBit(guid[6]);
        data->WriteBit(guid[5]);
        data->WriteBit(guid[4]);
        data->WriteBit(guid[3]);

        buffer.WriteByteSeq(guid[0]);
        buffer.WriteByteSeq(guid[7]);
        buffer << uint32(player->GetTalentSpecialization(player->GetActiveSpec()));
        buffer.WriteByteSeq(guid[2]);
        buffer.WriteByteSeq(guid[5]);
        buffer.WriteByteSeq(guid[3]);
        buffer.WriteByteSeq(guid[6]);
        buffer.WriteByteSeq(guid[1]);
        buffer.WriteByteSeq(guid[4]);

        count++;
    }

    data->FlushBits();
    data->PutBits(0, count, 21);
    data->append(buffer);

    return count;
}
