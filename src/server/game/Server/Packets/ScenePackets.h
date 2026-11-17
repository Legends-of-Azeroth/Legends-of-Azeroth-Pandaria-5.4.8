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

#ifndef ScenePackets_h__
#define ScenePackets_h__

#include "Packet.h"
#include <string>

namespace WorldPackets
{
    namespace Scene
    {
        class SceneTriggerEvent final : public ClientPacket
        {
        public:
            SceneTriggerEvent(WorldPacket&& packet) : ClientPacket(CMSG_SCENE_TRIGGER_EVENT, std::move(packet)) { }

            void Read() override;

            std::string Event;
            uint32 SceneInstanceID = 0;
        };

        class ScenePlaybackComplete final : public ClientPacket
        {
        public:
            ScenePlaybackComplete(WorldPacket&& packet) : ClientPacket(CMSG_SCENE_PLAYBACK_COMPLETE, std::move(packet)) { }

            void Read() override;

            uint32 SceneInstanceID = 0;
        };

        class ScenePlaybackCanceled final : public ClientPacket
        {
        public:
            ScenePlaybackCanceled(WorldPacket&& packet) : ClientPacket(CMSG_SCENE_PLAYBACK_CANCELED, std::move(packet)) { }

            void Read() override;

            uint32 SceneInstanceID = 0;
        };

        class QueryScenarioPOI final : public ClientPacket
        {
        public:
            QueryScenarioPOI(WorldPacket&& packet) : ClientPacket(CMSG_QUERY_SCENARIO_POI, std::move(packet)) { }

            void Read() override;

            std::vector<uint32> CriteriaTreeIds;
        };
    }
}

#endif // ScenePackets_h__
