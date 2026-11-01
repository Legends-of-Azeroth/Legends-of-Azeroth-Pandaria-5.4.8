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

#ifndef VoiceChatPackets_h__
#define VoiceChatPackets_h__

#include "Packet.h"

namespace WorldPackets
{
    namespace VoiceChat
    {
        class VoiceSessionEnable final : public ClientPacket
        {
        public:
            VoiceSessionEnable(WorldPacket&& packet) : ClientPacket(CMSG_VOICE_SESSION_ENABLE, std::move(packet)) { }

            void Read() override;

            bool IsVoiceEnabled = false;
            bool IsMicrophoneEnabled = false;
        };
    }
}

#endif // VoiceChatPackets_h__
