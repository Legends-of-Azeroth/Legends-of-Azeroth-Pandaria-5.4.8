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

#include "BankPackets.h"

void WorldPackets::Bank::AutoBankItem::Read()
{
    _worldPacket >> Slot >> Bag;
}

void WorldPackets::Bank::AutoStoreBankItem::Read()
{
    _worldPacket >> Slot >> Bag;
}

void WorldPackets::Bank::BuyBankSlot::Read()
{
    Guid[7] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[4]);
}

void WorldPackets::Bank::BankerActivate::Read()
{
    Banker[4] = _worldPacket.ReadBit();
    Banker[5] = _worldPacket.ReadBit();
    Banker[0] = _worldPacket.ReadBit();
    Banker[6] = _worldPacket.ReadBit();
    Banker[1] = _worldPacket.ReadBit();
    Banker[2] = _worldPacket.ReadBit();
    Banker[7] = _worldPacket.ReadBit();
    Banker[3] = _worldPacket.ReadBit();
    _worldPacket.ReadByteSeq(Banker[1]);
    _worldPacket.ReadByteSeq(Banker[7]);
    _worldPacket.ReadByteSeq(Banker[2]);
    _worldPacket.ReadByteSeq(Banker[5]);
    _worldPacket.ReadByteSeq(Banker[6]);
    _worldPacket.ReadByteSeq(Banker[3]);
    _worldPacket.ReadByteSeq(Banker[0]);
    _worldPacket.ReadByteSeq(Banker[4]);
}
