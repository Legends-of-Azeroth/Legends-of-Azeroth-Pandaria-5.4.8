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

#include "EquipmentSetPackets.h"
#include "Player.h"

void WorldPackets::EquipmentSet::DeleteEquipmentSet::Read()
{
    SetGuid[4] = _worldPacket.ReadBit();
    SetGuid[2] = _worldPacket.ReadBit();
    SetGuid[6] = _worldPacket.ReadBit();
    SetGuid[0] = _worldPacket.ReadBit();
    SetGuid[5] = _worldPacket.ReadBit();
    SetGuid[1] = _worldPacket.ReadBit();
    SetGuid[7] = _worldPacket.ReadBit();
    SetGuid[3] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(SetGuid[2]);
    _worldPacket.ReadByteSeq(SetGuid[0]);
    _worldPacket.ReadByteSeq(SetGuid[1]);
    _worldPacket.ReadByteSeq(SetGuid[6]);
    _worldPacket.ReadByteSeq(SetGuid[3]);
    _worldPacket.ReadByteSeq(SetGuid[4]);
    _worldPacket.ReadByteSeq(SetGuid[5]);
    _worldPacket.ReadByteSeq(SetGuid[7]);
}

void WorldPackets::EquipmentSet::SaveEquipmentSet::Read()
{
    Index = _worldPacket.read<uint32>();

    ItemGuids.resize(EQUIPMENT_SLOT_END);
    for (uint32 i = 0; i < EQUIPMENT_SLOT_END; ++i)
    {
        ItemGuids[i][5] = _worldPacket.ReadBit();
        ItemGuids[i][0] = _worldPacket.ReadBit();
        ItemGuids[i][1] = _worldPacket.ReadBit();
        ItemGuids[i][4] = _worldPacket.ReadBit();
        ItemGuids[i][6] = _worldPacket.ReadBit();
        ItemGuids[i][3] = _worldPacket.ReadBit();
        ItemGuids[i][7] = _worldPacket.ReadBit();
        ItemGuids[i][2] = _worldPacket.ReadBit();
    }

    SetGuid[7] = _worldPacket.ReadBit();
    SetGuid[1] = _worldPacket.ReadBit();
    SetGuid[5] = _worldPacket.ReadBit();
    SetGuid[2] = _worldPacket.ReadBit();
    SetGuid[3] = _worldPacket.ReadBit();
    SetGuid[0] = _worldPacket.ReadBit();

    uint8 setNameLen = _worldPacket.ReadBits(8);
    SetGuid[6] = _worldPacket.ReadBit();
    uint8 iconNameLen = _worldPacket.ReadBits(9);
    SetGuid[4] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(SetGuid[4]);
    _worldPacket.ReadByteSeq(SetGuid[0]);

    for (uint32 i = 0; i < EQUIPMENT_SLOT_END; ++i)
    {
        _worldPacket.ReadByteSeq(ItemGuids[i][1]);
        _worldPacket.ReadByteSeq(ItemGuids[i][0]);
        _worldPacket.ReadByteSeq(ItemGuids[i][7]);
        _worldPacket.ReadByteSeq(ItemGuids[i][3]);
        _worldPacket.ReadByteSeq(ItemGuids[i][6]);
        _worldPacket.ReadByteSeq(ItemGuids[i][2]);
        _worldPacket.ReadByteSeq(ItemGuids[i][4]);
        _worldPacket.ReadByteSeq(ItemGuids[i][5]);
    }

    IconName = _worldPacket.ReadString(iconNameLen);

    _worldPacket.ReadByteSeq(SetGuid[7]);
    _worldPacket.ReadByteSeq(SetGuid[2]);

    Name = _worldPacket.ReadString(setNameLen);

    _worldPacket.ReadByteSeq(SetGuid[6]);
    _worldPacket.ReadByteSeq(SetGuid[1]);
    _worldPacket.ReadByteSeq(SetGuid[5]);
    _worldPacket.ReadByteSeq(SetGuid[3]);
}

void WorldPackets::EquipmentSet::UseEquipmentSet::Read()
{
    for (uint32 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
    {
        _worldPacket.read_skip<uint8>(); // srcslot
        _worldPacket.read_skip<uint8>(); // srcbag
    }

    ItemGuids.resize(EQUIPMENT_SLOT_END);
    for (uint32 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
    {
        ItemGuids[i][3] = _worldPacket.ReadBit();
        ItemGuids[i][1] = _worldPacket.ReadBit();
        ItemGuids[i][7] = _worldPacket.ReadBit();
        ItemGuids[i][4] = _worldPacket.ReadBit();
        ItemGuids[i][5] = _worldPacket.ReadBit();
        ItemGuids[i][6] = _worldPacket.ReadBit();
        ItemGuids[i][0] = _worldPacket.ReadBit();
        ItemGuids[i][2] = _worldPacket.ReadBit();
    }

    uint8 InvItemCounter = _worldPacket.ReadBits(2);

    for (uint8 i = 0; i < InvItemCounter; i++)
    {
        _worldPacket.ReadBit(); // Container Slot
        _worldPacket.ReadBit(); // Slot
    }

    for (uint32 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
    {
        _worldPacket.ReadByteSeq(ItemGuids[i][4]);
        _worldPacket.ReadByteSeq(ItemGuids[i][7]);
        _worldPacket.ReadByteSeq(ItemGuids[i][0]);
        _worldPacket.ReadByteSeq(ItemGuids[i][3]);
        _worldPacket.ReadByteSeq(ItemGuids[i][2]);
        _worldPacket.ReadByteSeq(ItemGuids[i][5]);
        _worldPacket.ReadByteSeq(ItemGuids[i][1]);
        _worldPacket.ReadByteSeq(ItemGuids[i][6]);
    }

    for (uint8 i = 0; i < InvItemCounter; i++)
    {
        _worldPacket.read_skip<uint8>();
        _worldPacket.read_skip<uint8>();
    }
}
