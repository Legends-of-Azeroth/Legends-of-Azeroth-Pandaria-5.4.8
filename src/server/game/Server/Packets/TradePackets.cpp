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

#include "TradePackets.h"

void WorldPackets::Trade::BeginTrade::Read()
{
}

void WorldPackets::Trade::AcceptTrade::Read()
{
}

void WorldPackets::Trade::CancelTrade::Read()
{
}

void WorldPackets::Trade::UnacceptTrade::Read()
{
}

void WorldPackets::Trade::IgnoreTrade::Read()
{
}

void WorldPackets::Trade::ClearTradeItem::Read()
{
    _worldPacket >> TradeSlot;
}

void WorldPackets::Trade::SetTradeItem::Read()
{
    _worldPacket >> TradeSlot;
    _worldPacket >> Slot;
    _worldPacket >> Bag;
}

void WorldPackets::Trade::SetTradeGold::Read()
{
    _worldPacket >> Gold;
}

void WorldPackets::Trade::InitiateTrade::Read()
{
    Guid[5] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[1]);
}
