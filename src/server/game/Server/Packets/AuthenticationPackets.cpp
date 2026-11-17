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
#include "AuthenticationPackets.h"

bool WorldPackets::Auth::EarlyProcessClientPacket::ReadNoThrow()
{
    try
    {
        Read();
        return true;
    }
    catch (ByteBufferPositionException const& /*ex*/)
    {
    }

    return false;
}

void WorldPackets::Auth::Ping::Read()
{
    _worldPacket >> Latency;
    _worldPacket >> Serial;
}

WorldPacket const* WorldPackets::Auth::Pong::Write()
{
    _worldPacket << uint32(Serial);
    return &_worldPacket;
}

WorldPacket const* WorldPackets::Auth::AuthChallenge::Write()
{
    _worldPacket<< uint16(0); // header?
    _worldPacket.append(DosChallenge.data(), DosChallenge.size());
    _worldPacket << uint8(DosZeroBits);
    _worldPacket.append(Challenge.data(), Challenge.size());
    return &_worldPacket;
}