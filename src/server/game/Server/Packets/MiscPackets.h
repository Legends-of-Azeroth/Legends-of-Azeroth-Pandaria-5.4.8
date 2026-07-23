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

    }
}

#endif // MiscPackets_h__
