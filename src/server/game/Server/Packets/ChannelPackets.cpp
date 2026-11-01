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

#include "ChannelPackets.h"

void WorldPackets::Channel::JoinChannel::Read()
{
    ChannelId = _worldPacket.read<uint32>();
    _worldPacket.ReadBit(); // unk bit
    uint32 channelLength = _worldPacket.ReadBits(7);
    uint32 passLength = _worldPacket.ReadBits(7);
    _worldPacket.ReadBit(); // unk bit
    Password = _worldPacket.ReadString(passLength);
    ChannelName = _worldPacket.ReadString(channelLength);
}

void WorldPackets::Channel::LeaveChannel::Read()
{
    _worldPacket >> Unk;                                      // channel id?
    uint32 length = _worldPacket.ReadBits(7);
    ChannelName = _worldPacket.ReadString(length);
}

void WorldPackets::Channel::ChannelAnnouncements::Read()
{
    uint32 length = _worldPacket.ReadBits(7);
    ChannelName = _worldPacket.ReadString(length);
}

void WorldPackets::Channel::ChannelBan::Read()
{
    uint32 nameLength = _worldPacket.ReadBits(8) << 1;
    nameLength += _worldPacket.ReadBit();
    uint32 channelLength = _worldPacket.ReadBits(7);
    TargetName = _worldPacket.ReadString(nameLength);
    ChannelName = _worldPacket.ReadString(channelLength);
}

void WorldPackets::Channel::ChannelDisplayList::Read()
{
    uint32 length = _worldPacket.ReadBits(7);
    ChannelName = _worldPacket.ReadString(length);
}

void WorldPackets::Channel::ChannelInvite::Read()
{
    uint32 nameLength = _worldPacket.ReadBits(8) << 1;
    nameLength += _worldPacket.ReadBit();
    uint32 channelLength = _worldPacket.ReadBits(7);
    ChannelName = _worldPacket.ReadString(channelLength);
    TargetName = _worldPacket.ReadString(nameLength);
}

void WorldPackets::Channel::ChannelKick::Read()
{
    uint32 channelLength = _worldPacket.ReadBits(7);
    uint32 nameLength = _worldPacket.ReadBits(8) << 1;
    nameLength += _worldPacket.ReadBit();
    ChannelName = _worldPacket.ReadString(channelLength);
    TargetName = _worldPacket.ReadString(nameLength);
}

void WorldPackets::Channel::ChannelList::Read()
{
    uint32 length = _worldPacket.ReadBits(7);
    ChannelName = _worldPacket.ReadString(length);
}

void WorldPackets::Channel::ChannelModerator::Read()
{
    uint32 channelLength = _worldPacket.ReadBits(7);
    uint32 nameLength = _worldPacket.ReadBits(8) << 1;
    nameLength += _worldPacket.ReadBit();
    ChannelName = _worldPacket.ReadString(channelLength);
    TargetName = _worldPacket.ReadString(nameLength);
}

void WorldPackets::Channel::ChannelMute::Read()
{
    uint32 channelLength = _worldPacket.ReadBits(7);
    uint32 nameLength = _worldPacket.ReadBits(8) << 1;
    nameLength += _worldPacket.ReadBit();
    TargetName = _worldPacket.ReadString(nameLength);
    ChannelName = _worldPacket.ReadString(channelLength);
}

void WorldPackets::Channel::ChannelOwner::Read()
{
    uint32 length = _worldPacket.ReadBits(7);
    ChannelName = _worldPacket.ReadString(length);
}

void WorldPackets::Channel::ChannelPassword::Read()
{
    uint32 passLength = _worldPacket.ReadBits(7);
    uint32 nameLength = _worldPacket.ReadBits(7);
    Password = _worldPacket.ReadString(passLength);
    ChannelName = _worldPacket.ReadString(nameLength);
}

void WorldPackets::Channel::ChannelSetOwner::Read()
{
    uint32 nameLength = _worldPacket.ReadBits(8) << 1;
    nameLength += _worldPacket.ReadBit();
    uint32 channelLength = _worldPacket.ReadBits(7);
    ChannelName = _worldPacket.ReadString(channelLength);
    TargetName = _worldPacket.ReadString(nameLength);
}

void WorldPackets::Channel::ChannelUnban::Read()
{
    uint32 nameLength = _worldPacket.ReadBits(8) << 1;
    nameLength += _worldPacket.ReadBit();
    uint32 channelLength = _worldPacket.ReadBits(7);
    TargetName = _worldPacket.ReadString(nameLength);
    ChannelName = _worldPacket.ReadString(channelLength);
}

void WorldPackets::Channel::ChannelUnmoderator::Read()
{
    uint32 channelLength = _worldPacket.ReadBits(7);
    uint32 nameLength = _worldPacket.ReadBits(8) << 1;
    nameLength += _worldPacket.ReadBit();
    TargetName = _worldPacket.ReadString(nameLength);
    ChannelName = _worldPacket.ReadString(channelLength);
}

void WorldPackets::Channel::ChannelUnmute::Read()
{
    uint32 nameLength = _worldPacket.ReadBits(8) << 1;
    nameLength += _worldPacket.ReadBit();
    uint32 channelLength = _worldPacket.ReadBits(7);
    TargetName = _worldPacket.ReadString(nameLength);
    ChannelName = _worldPacket.ReadString(channelLength);
}

void WorldPackets::Channel::ChannelVoiceOn::Read()
{
    _worldPacket.ReadString(_worldPacket.ReadBits(8));
}
