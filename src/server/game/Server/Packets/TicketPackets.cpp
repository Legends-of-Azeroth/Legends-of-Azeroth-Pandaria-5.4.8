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

#include "TicketPackets.h"
#include "Log.h"
#include "zlib.h"

void WorldPackets::Ticket::GMTicketUpdateText::Read()
{
    uint8 messageLen = _worldPacket.ReadBits(11);
    Message = _worldPacket.ReadString(messageLen);
}

void WorldPackets::Ticket::GMTicketCreate::Read()
{
    MapId = _worldPacket.read<uint32>();
    Z = _worldPacket.read<float>();
    Y = _worldPacket.read<float>();
    _worldPacket.read_skip<uint8>();    // flags!?
    X = _worldPacket.read<float>();

    uint32 count = _worldPacket.read<uint32>();

    if (count > 0)
    {
        uint8 textCount = _worldPacket.read<uint8>();
        for (uint32 i = 0; i < textCount; i++)
        {
            uint32 time = _worldPacket.read<uint32>();
            Times.push_back(time);
        }

        uint32 decompressedSize = _worldPacket.read<uint32>();

        if (count && decompressedSize && decompressedSize < 0xFFFF)
        {
            uint32 pos = _worldPacket.rpos();
            ByteBuffer tailPacket;
            tailPacket.resize(decompressedSize);

            uLongf realSize = decompressedSize;
            if (uncompress(tailPacket.contents(), &realSize, _worldPacket.contents() + pos, _worldPacket.size() - pos) == Z_OK)
                tailPacket >> ChatLog;
            else
            {
                TC_LOG_ERROR("network", "CMSG_GM_TICKET_CREATE possibly corrupt. Uncompression failed.");
                DecompressOk = false;
            }
        }
    }

    NeedResponse = _worldPacket.ReadBit();
    HaveTicket = _worldPacket.ReadBit();
    uint16 messageLen = _worldPacket.ReadBits(11);
    Message = _worldPacket.ReadString(messageLen);
}
