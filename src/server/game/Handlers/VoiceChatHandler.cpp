/*
* This file is part of the Pandaria 5.4.8 Project. See THANKS file for Copyright information
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

#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Log.h"

void WorldSession::HandleVoiceSessionEnableOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_VOICE_SESSION_ENABLE");
    bool isVoiceEnabled, isMicrophoneEnabled;
    isVoiceEnabled = recvData.ReadBit();
    isMicrophoneEnabled = recvData.ReadBit();
}

void WorldSession::HandleChannelVoiceOnOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_CHANNEL_VOICE_ON");
    // Enable Voice button in channel context menu
    recvData.ReadString(recvData.ReadBits(8));
    //channel->EnableVoice(recvData.GetOpcode() == CMSG_CHANNEL_VOICE_ON);
}

void WorldSession::HandleSetActiveVoiceChannel(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_SET_ACTIVE_VOICE_CHANNEL");
    recvData.read_skip<uint32>();
    recvData.read_skip<char*>();
}
