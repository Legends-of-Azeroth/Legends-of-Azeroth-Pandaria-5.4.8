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

#include "ClientConfigPackets.h"

void WorldPackets::ClientConfig::RequestAccountData::Read()
{
    DataType = _worldPacket.ReadBits(3);
}

void WorldPackets::ClientConfig::UserClientUpdateAccountData::Read()
{
    _worldPacket >> Size >> Time >> CompressedSize;

    if (Size == 0)
    {
        DataType = _worldPacket.ReadBits(3);
        _worldPacket.FlushBits();
        return;
    }

    if (Size > 0xFFFF)
    {
        _worldPacket.rfinish();
        return;
    }

    CompressedData = std::string(reinterpret_cast<char const*>(_worldPacket.contents() + _worldPacket.rpos()), CompressedSize);
    _worldPacket.read_skip(CompressedSize);
    DataType = _worldPacket.ReadBits(3);
    _worldPacket.FlushBits();
}
