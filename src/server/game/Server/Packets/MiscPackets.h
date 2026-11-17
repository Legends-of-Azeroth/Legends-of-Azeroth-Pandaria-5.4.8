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

#ifndef MiscPackets_h__
#define MiscPackets_h__

#include "Packet.h"
#include <array>
#include "Optional.h"

enum WeatherState : uint32;

namespace WorldPackets
{
    namespace Misc
    {
        class BinderConfirm final : public ServerPacket
        {
        public:
            BinderConfirm() : ServerPacket(SMSG_BINDER_CONFIRM, 8) { }
            BinderConfirm(ObjectGuid unit) : ServerPacket(SMSG_BINDER_CONFIRM, 8), Unit(unit) { }

            WorldPacket const* Write() override;

            ObjectGuid Unit;
        };

        class TriggerCinematic final : public ServerPacket
        {
        public:
            TriggerCinematic() : ServerPacket(SMSG_TRIGGER_CINEMATIC, 4) { }

            WorldPacket const* Write() override;

            uint32 CinematicID = 0;
        };        

        class TriggerMovie final : public ServerPacket
        {
        public:
            TriggerMovie() : ServerPacket(SMSG_TRIGGER_MOVIE, 4) { }

            WorldPacket const* Write() override;

            uint32 MovieID = 0;
        };

        class CompleteCinematic final : public ClientPacket
        {
        public:
            CompleteCinematic(WorldPacket&& packet) : ClientPacket(CMSG_COMPLETE_CINEMATIC, std::move(packet)) { }

            void Read() override { }
        };
        
        class NextCinematicCamera final : public ClientPacket
        {
        public:
            NextCinematicCamera(WorldPacket&& packet) : ClientPacket(CMSG_NEXT_CINEMATIC_CAMERA, std::move(packet)) { }

            void Read() override { }
        };

        class CompleteMovie final : public ClientPacket
        {
        public:
            CompleteMovie(WorldPacket&& packet) : ClientPacket(CMSG_COMPLETE_MOVIE, std::move(packet)) { }

            void Read() override { }
        };

        class OpeningCinematic final : public ClientPacket
        {
        public:
            OpeningCinematic(WorldPacket&& packet) : ClientPacket(CMSG_OPENING_CINEMATIC, std::move(packet)) { }

            void Read() override { }
        };

        class StreamingMovies final : public ServerPacket
        {
        public:
            StreamingMovies() : ServerPacket(SMSG_STREAMING_MOVIES) { }

            WorldPacket const* Write() override;

            std::vector<uint16> MovieIDs;
        };

        class UITime final : public ServerPacket
        {
        public:
            UITime() : ServerPacket(SMSG_WORLD_STATE_UI_TIMER_UPDATE, 4) { }

            WorldPacket const* Write() override;

            uint32 Time = 0;
        };

        class TC_GAME_API Weather final : public ServerPacket
        {
        public:
            Weather();
            Weather(WeatherState weatherID, float intensity = 0.0f, bool abrupt = false);

            WorldPacket const* Write() override;

            bool Abrupt = false;
            float Intensity = 0.0f;
            WeatherState WeatherID = WeatherState(0);
        };        

        class TC_GAME_API PlayMusic final : public ServerPacket
        {
        public:
            PlayMusic() : ServerPacket(SMSG_PLAY_MUSIC, 4) { }
            PlayMusic(uint32 soundKitID) : ServerPacket(SMSG_PLAY_MUSIC, 4), SoundKitID(soundKitID) { }

            WorldPacket const* Write() override;

            uint32 SoundKitID = 0;
        };

        class PlayObjectSound final : public ServerPacket
        {
        public:
            PlayObjectSound() : ServerPacket(SMSG_PLAY_OBJECT_SOUND, 8 + 8 + 4) { }
            PlayObjectSound(ObjectGuid sourceObjectGuid, ObjectGuid targetObjectGuid, uint32 soundKitID) : ServerPacket(SMSG_PLAY_OBJECT_SOUND, 8 + 8 + 4),
                SourceObjectGUID(sourceObjectGuid), TargetObjectGUID(targetObjectGuid), SoundKitID(soundKitID) { }

            WorldPacket const* Write() override;

            ObjectGuid SourceObjectGUID;
            ObjectGuid TargetObjectGUID;
            uint32 SoundKitID = 0;
        };

        class TC_GAME_API PlaySound final : public ServerPacket
        {
        public:
            PlaySound() : ServerPacket(SMSG_PLAY_SOUND, 4 + 8) { }
            PlaySound(ObjectGuid sourceObjectGuid, uint32 soundKitID) : ServerPacket(SMSG_PLAY_SOUND, 4 + 8), SourceObjectGUID(sourceObjectGuid), SoundKitID(soundKitID) { }

            WorldPacket const* Write() override;

            ObjectGuid SourceObjectGUID;
            uint32 SoundKitID = 0;
        };

        class StartMirrorTimer final : public ServerPacket
        {
        public:
            StartMirrorTimer() : ServerPacket(SMSG_START_MIRROR_TIMER, 21) { }
            StartMirrorTimer(uint32 timer, uint32 value, uint32 maxValue, int32 scale, bool paused, uint32 spellID) :
                ServerPacket(SMSG_START_MIRROR_TIMER, 21), Timer(timer), Value(value), MaxValue(maxValue), Scale(scale), Paused(paused), SpellID(spellID) { }

            WorldPacket const* Write() override;

            uint32 Timer = 0;
            uint32 Value = 0;
            uint32 MaxValue = 0;
            int32 Scale = 0;
            bool Paused = false;
            uint32 SpellID = 0;
        };

        class StopMirrorTimer final : public ServerPacket
        {
        public:
            StopMirrorTimer() : ServerPacket(SMSG_STOP_MIRROR_TIMER, 4) { }
            StopMirrorTimer(uint32 timer) : ServerPacket(SMSG_STOP_MIRROR_TIMER, 4), Timer(timer) { }

            WorldPacket const* Write() override;

            uint32 Timer = 0;
        };

        class BindPointUpdate final : public ServerPacket
        {
        public:
            BindPointUpdate() : ServerPacket(SMSG_BIND_POINT_UPDATE, 20) { }

            WorldPacket const* Write() override;

            uint32 BindMapID = 0;
            Position BindPosition;
            uint32 BindAreaID = 0;
        };

        class Dismount final : public ServerPacket
        {
        public:
            Dismount() : ServerPacket(SMSG_DISMOUNT, 8) { }

            WorldPacket const* Write() override;

            ObjectGuid Guid;
        };

        class CorpseReclaimDelay : public ServerPacket
        {
        public:
            CorpseReclaimDelay() : ServerPacket(SMSG_CORPSE_RECLAIM_DELAY, 4) { }

            WorldPacket const* Write() override;

            uint32 Remaining = 0;
        };

        class FarSight final : public ClientPacket
        {
        public:
            FarSight(WorldPacket&& packet) : ClientPacket(CMSG_FAR_SIGHT, std::move(packet)) { }

            void Read() override;

            bool Enable = false;
        };

        class SetPvP final : public ClientPacket
        {
        public:
            SetPvP(WorldPacket&& packet) : ClientPacket(CMSG_SET_PVP, std::move(packet)) { }

            void Read() override;

            bool EnablePVP = false;
        };

        class TogglePvP final : public ClientPacket
        {
        public:
            TogglePvP(WorldPacket&& packet) : ClientPacket(CMSG_TOGGLE_PVP, std::move(packet)) { }

            void Read() override;

            bool HasPvPStatus() const { return GetSize() == 1; }

            Optional<bool> Enable;
        };

        class OverrideLight final : public ServerPacket
        {
        public:
            OverrideLight() : ServerPacket(SMSG_OVERRIDE_LIGHT, 4 + 4 + 4) { }

            WorldPacket const* Write() override;

            int32 AreaLightID = 0;
            int32 TransitionMilliseconds = 0;
            int32 OverrideLightID = 0;
        };

        class DisplayGameError final : public ServerPacket
        {
        public:
            DisplayGameError(GameError error) : ServerPacket(SMSG_DISPLAY_GAME_ERROR, 4 + 1), Error(error) { }
            DisplayGameError(GameError error, int32 arg) : ServerPacket(SMSG_DISPLAY_GAME_ERROR, 4 + 1 + 4), Error(error), Arg(arg) { }
            DisplayGameError(GameError error, int32 arg1, int32 arg2) : ServerPacket(SMSG_DISPLAY_GAME_ERROR, 4 + 1 + 4 + 4), Error(error), Arg(arg1), Arg2(arg2) { }

            WorldPacket const* Write() override;

            GameError Error;
            Optional<int32> Arg;
            Optional<int32> Arg2;
        };

        class TutorialSetFlag final : public ClientPacket
        {
        public:
            TutorialSetFlag(WorldPacket&& packet) : ClientPacket(CMSG_TUTORIAL_FLAG, std::move(packet)) { }

            void Read() override;

            uint32 TutorialBit = 0;
        };

        class TutorialClear final : public ClientPacket
        {
        public:
            TutorialClear(WorldPacket&& packet) : ClientPacket(CMSG_TUTORIAL_CLEAR, std::move(packet)) { }

            void Read() override { }
        };

        class TutorialReset final : public ClientPacket
        {
        public:
            TutorialReset(WorldPacket&& packet) : ClientPacket(CMSG_TUTORIAL_RESET, std::move(packet)) { }

            void Read() override { }
        };

        class QueryCountdownTimer final : public ClientPacket
        {
        public:
            QueryCountdownTimer(WorldPacket&& packet) : ClientPacket(CMSG_QUERY_COUNTDOWN_TIMER, std::move(packet)) { }

            void Read() override;
        };

        class RequestHotfix final : public ClientPacket
        {
        public:
            RequestHotfix(WorldPacket&& packet) : ClientPacket(CMSG_HOTFIX_REQUEST, std::move(packet)) { }

            void Read() override;

            uint32 Type = 0;
            std::vector<uint32> Entries;
        };

        class LogoutRequest final : public ClientPacket
        {
        public:
            LogoutRequest(WorldPacket&& packet) : ClientPacket(CMSG_LOGOUT_REQUEST, std::move(packet)) { }

            void Read() override { }
        };

        class LogoutCancel final : public ClientPacket
        {
        public:
            LogoutCancel(WorldPacket&& packet) : ClientPacket(CMSG_LOGOUT_CANCEL, std::move(packet)) { }

            void Read() override { }
        };

        class RequestCemeteryList final : public ClientPacket
        {
        public:
            RequestCemeteryList(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_CEMETERY_LIST, std::move(packet)) { }

            void Read() override { }
        };

        class SetSelection final : public ClientPacket
        {
        public:
            SetSelection(WorldPacket&& packet) : ClientPacket(CMSG_SET_SELECTION, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
        };

        class StandStateChange final : public ClientPacket
        {
        public:
            StandStateChange(WorldPacket&& packet) : ClientPacket(CMSG_STANDSTATECHANGE, std::move(packet)) { }

            void Read() override;

            uint32 AnimState = 0;
        };

        class SetActionButton final : public ClientPacket
        {
        public:
            SetActionButton(WorldPacket&& packet) : ClientPacket(CMSG_SET_ACTION_BUTTON, std::move(packet)) { }

            void Read() override;

            uint8 SlotId = 0;
            ObjectGuid ButtonStream;
        };

        class SetActionBarToggles final : public ClientPacket
        {
        public:
            SetActionBarToggles(WorldPacket&& packet) : ClientPacket(CMSG_SET_ACTIONBAR_TOGGLES, std::move(packet)) { }

            void Read() override;

            uint8 ActionBar = 0;
        };

        class PlayedTimeClient final : public ClientPacket
        {
        public:
            PlayedTimeClient(WorldPacket&& packet) : ClientPacket(CMSG_PLAYED_TIME, std::move(packet)) { }

            void Read() override;

            uint8 Unk1 = 0;
        };

        class SetTitle final : public ClientPacket
        {
        public:
            SetTitle(WorldPacket&& packet) : ClientPacket(CMSG_SET_TITLE, std::move(packet)) { }

            void Read() override;

            int32 Title = 0;
        };

        class TimeSyncResponse final : public ClientPacket
        {
        public:
            TimeSyncResponse(WorldPacket&& packet) : ClientPacket(CMSG_TIME_SYNC_RESP, std::move(packet)) { }

            void Read() override;

            uint32 Counter = 0;
            uint32 ClientTicks = 0;
        };

        class SetTaxiBenchmarkMode final : public ClientPacket
        {
        public:
            SetTaxiBenchmarkMode(WorldPacket&& packet) : ClientPacket(CMSG_SET_TAXI_BENCHMARK_MODE, std::move(packet)) { }

            void Read() override;

            uint8 Mode = 0;
        };

        class GuildAchievementProgressQuery final : public ClientPacket
        {
        public:
            GuildAchievementProgressQuery(WorldPacket&& packet) : ClientPacket(CMSG_GUILD_ACHIEVEMENT_PROGRESS_QUERY, std::move(packet)) { }

            void Read() override;

            uint32 AchievementId = 0;
        };

        class UITimeRequest final : public ClientPacket
        {
        public:
            UITimeRequest(WorldPacket&& packet) : ClientPacket(CMSG_WORLD_STATE_UI_TIMER_UPDATE, std::move(packet)) { }

            void Read() override { }
        };

        class ReadyForAccountDataTimes final : public ClientPacket
        {
        public:
            ReadyForAccountDataTimes(WorldPacket&& packet) : ClientPacket(CMSG_READY_FOR_ACCOUNT_DATA_TIMES, std::move(packet)) { }

            void Read() override { }
        };

        class ViolenceLevel final : public ClientPacket
        {
        public:
            ViolenceLevel(WorldPacket&& packet) : ClientPacket(CMSG_VIOLENCE_LEVEL, std::move(packet)) { }

            void Read() override;

            uint8 Level = 0;
        };

        class ObjectUpdateFailed final : public ClientPacket
        {
        public:
            ObjectUpdateFailed(WorldPacket&& packet) : ClientPacket(CMSG_OBJECT_UPDATE_FAILED, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
        };

        class SelectFaction final : public ClientPacket
        {
        public:
            SelectFaction(WorldPacket&& packet) : ClientPacket(CMSG_SELECT_FACTION, std::move(packet)) { }

            void Read() override;

            uint32 Choice = 0;
        };

        class DiscardedTimeSyncAcks final : public ClientPacket
        {
        public:
            DiscardedTimeSyncAcks(WorldPacket&& packet) : ClientPacket(CMSG_DISCARDED_TIME_SYNC_ACKS, std::move(packet)) { }

            void Read() override;
        };

        class LogStreamingError final : public ClientPacket
        {
        public:
            LogStreamingError(WorldPacket&& packet) : ClientPacket(CMSG_LOG_STREAMING_ERROR, std::move(packet)) { }

            void Read() override;

            std::string MissingData;
        };

        class ShowTradeSkill final : public ClientPacket
        {
        public:
            ShowTradeSkill(WorldPacket&& packet) : ClientPacket(CMSG_SHOW_TRADE_SKILL, std::move(packet)) { }

            void Read() override;

            uint32 SpellId = 0;
            uint32 SkillId = 0;
            ObjectGuid Guid;
        };

    }
}

#endif // MiscPackets_h__
