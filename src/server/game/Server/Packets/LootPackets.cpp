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

#include "LootPackets.h"

void WorldPackets::Loot::LootUnit::Read()
{
    Unit[4] = _worldPacket.ReadBit();
    Unit[5] = _worldPacket.ReadBit();
    Unit[2] = _worldPacket.ReadBit();
    Unit[7] = _worldPacket.ReadBit();
    Unit[0] = _worldPacket.ReadBit();
    Unit[1] = _worldPacket.ReadBit();
    Unit[3] = _worldPacket.ReadBit();
    Unit[6] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Unit[3]);
    _worldPacket.ReadByteSeq(Unit[5]);
    _worldPacket.ReadByteSeq(Unit[0]);
    _worldPacket.ReadByteSeq(Unit[6]);
    _worldPacket.ReadByteSeq(Unit[4]);
    _worldPacket.ReadByteSeq(Unit[1]);
    _worldPacket.ReadByteSeq(Unit[7]);
    _worldPacket.ReadByteSeq(Unit[2]);
}

void WorldPackets::Loot::LootRelease::Read()
{
    Unit[7] = _worldPacket.ReadBit();
    Unit[4] = _worldPacket.ReadBit();
    Unit[2] = _worldPacket.ReadBit();
    Unit[3] = _worldPacket.ReadBit();
    Unit[0] = _worldPacket.ReadBit();
    Unit[5] = _worldPacket.ReadBit();
    Unit[6] = _worldPacket.ReadBit();
    Unit[1] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Unit[0]);
    _worldPacket.ReadByteSeq(Unit[6]);
    _worldPacket.ReadByteSeq(Unit[4]);
    _worldPacket.ReadByteSeq(Unit[2]);
    _worldPacket.ReadByteSeq(Unit[5]);
    _worldPacket.ReadByteSeq(Unit[3]);
    _worldPacket.ReadByteSeq(Unit[7]);
    _worldPacket.ReadByteSeq(Unit[1]);
}

void WorldPackets::Loot::LootRoll::Read()
{
    _worldPacket >> LootListID;
    _worldPacket >> RollType;

    LootObj[7] = _worldPacket.ReadBit();
    LootObj[1] = _worldPacket.ReadBit();
    LootObj[2] = _worldPacket.ReadBit();
    LootObj[0] = _worldPacket.ReadBit();
    LootObj[6] = _worldPacket.ReadBit();
    LootObj[3] = _worldPacket.ReadBit();
    LootObj[4] = _worldPacket.ReadBit();
    LootObj[5] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(LootObj[0]);
    _worldPacket.ReadByteSeq(LootObj[2]);
    _worldPacket.ReadByteSeq(LootObj[7]);
    _worldPacket.ReadByteSeq(LootObj[3]);
    _worldPacket.ReadByteSeq(LootObj[1]);
    _worldPacket.ReadByteSeq(LootObj[5]);
    _worldPacket.ReadByteSeq(LootObj[4]);
    _worldPacket.ReadByteSeq(LootObj[6]);
}

void WorldPackets::Loot::LootItem::Read()
{
    int32 lootCount = _worldPacket.ReadBits(23);
    guids.resize(lootCount);

    for (int i = 0; i < lootCount; i++)
    {
        guids[i][2] = _worldPacket.ReadBit();
        guids[i][7] = _worldPacket.ReadBit();
        guids[i][0] = _worldPacket.ReadBit();
        guids[i][6] = _worldPacket.ReadBit();
        guids[i][5] = _worldPacket.ReadBit();
        guids[i][3] = _worldPacket.ReadBit();
        guids[i][1] = _worldPacket.ReadBit();
        guids[i][4] = _worldPacket.ReadBit();
    }

    lootSlots.resize(lootCount);
    for (int i = 0; i < lootCount; i++)
    {
        _worldPacket.ReadByteSeq(guids[i][0]);
        _worldPacket.ReadByteSeq(guids[i][4]);
        _worldPacket.ReadByteSeq(guids[i][1]);
        _worldPacket.ReadByteSeq(guids[i][7]);
        _worldPacket.ReadByteSeq(guids[i][6]);
        _worldPacket.ReadByteSeq(guids[i][5]);
        _worldPacket.ReadByteSeq(guids[i][3]);
        _worldPacket.ReadByteSeq(guids[i][2]);
        _worldPacket >> lootSlots[i];
    }
}

void WorldPackets::Loot::MasterLootItem::Read()
{
    Target[0] = _worldPacket.ReadBit();
    Target[2] = _worldPacket.ReadBit();
    Target[4] = _worldPacket.ReadBit();
    Target[1] = _worldPacket.ReadBit();
    Target[7] = _worldPacket.ReadBit();
    Target[6] = _worldPacket.ReadBit();
    uint32 size = _worldPacket.ReadBits(23);

    lootViews.resize(size);
    for (uint32 i = 0; i < size; ++i)
    {
        lootViews[i][6] = _worldPacket.ReadBit();
        lootViews[i][1] = _worldPacket.ReadBit();
        lootViews[i][7] = _worldPacket.ReadBit();
        lootViews[i][0] = _worldPacket.ReadBit();
        lootViews[i][3] = _worldPacket.ReadBit();
        lootViews[i][4] = _worldPacket.ReadBit();
        lootViews[i][5] = _worldPacket.ReadBit();
        lootViews[i][2] = _worldPacket.ReadBit();
    }

    Target[3] = _worldPacket.ReadBit();
    Target[5] = _worldPacket.ReadBit();

    slots.resize(size);
    for (uint32 i = 0; i < size; ++i)
    {
        _worldPacket >> slots[i];
        _worldPacket.ReadByteSeq(lootViews[i][3]);
        _worldPacket.ReadByteSeq(lootViews[i][0]);
        _worldPacket.ReadByteSeq(lootViews[i][6]);
        _worldPacket.ReadByteSeq(lootViews[i][7]);
        _worldPacket.ReadByteSeq(lootViews[i][1]);
        _worldPacket.ReadByteSeq(lootViews[i][4]);
        _worldPacket.ReadByteSeq(lootViews[i][2]);
        _worldPacket.ReadByteSeq(lootViews[i][5]);
    }

    _worldPacket.ReadByteSeq(Target[4]);
    _worldPacket.ReadByteSeq(Target[5]);
    _worldPacket.ReadByteSeq(Target[2]);
    _worldPacket.ReadByteSeq(Target[3]);
    _worldPacket.ReadByteSeq(Target[6]);
    _worldPacket.ReadByteSeq(Target[0]);
    _worldPacket.ReadByteSeq(Target[7]);
    _worldPacket.ReadByteSeq(Target[1]);
}

void WorldPackets::Loot::SetCurrencyFlags::Read()
{
    _worldPacket >> Flags;
    _worldPacket >> CurrencyId;
}
