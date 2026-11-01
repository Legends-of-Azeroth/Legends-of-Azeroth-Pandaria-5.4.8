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

#include "VoidStoragePackets.h"

void WorldPackets::VoidStorage::VoidStorageUnlock::Read()
{
    NpcGuid[3] = _worldPacket.ReadBit();
    NpcGuid[1] = _worldPacket.ReadBit();
    NpcGuid[5] = _worldPacket.ReadBit();
    NpcGuid[6] = _worldPacket.ReadBit();
    NpcGuid[4] = _worldPacket.ReadBit();
    NpcGuid[0] = _worldPacket.ReadBit();
    NpcGuid[7] = _worldPacket.ReadBit();
    NpcGuid[2] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(NpcGuid[4]);
    _worldPacket.ReadByteSeq(NpcGuid[3]);
    _worldPacket.ReadByteSeq(NpcGuid[6]);
    _worldPacket.ReadByteSeq(NpcGuid[2]);
    _worldPacket.ReadByteSeq(NpcGuid[1]);
    _worldPacket.ReadByteSeq(NpcGuid[5]);
    _worldPacket.ReadByteSeq(NpcGuid[7]);
    _worldPacket.ReadByteSeq(NpcGuid[0]);
}

void WorldPackets::VoidStorage::VoidStorageQuery::Read()
{
    NpcGuid[1] = _worldPacket.ReadBit();
    NpcGuid[5] = _worldPacket.ReadBit();
    NpcGuid[6] = _worldPacket.ReadBit();
    NpcGuid[0] = _worldPacket.ReadBit();
    NpcGuid[7] = _worldPacket.ReadBit();
    NpcGuid[2] = _worldPacket.ReadBit();
    NpcGuid[3] = _worldPacket.ReadBit();
    NpcGuid[4] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(NpcGuid[1]);
    _worldPacket.ReadByteSeq(NpcGuid[6]);
    _worldPacket.ReadByteSeq(NpcGuid[4]);
    _worldPacket.ReadByteSeq(NpcGuid[3]);
    _worldPacket.ReadByteSeq(NpcGuid[7]);
    _worldPacket.ReadByteSeq(NpcGuid[0]);
    _worldPacket.ReadByteSeq(NpcGuid[2]);
    _worldPacket.ReadByteSeq(NpcGuid[5]);
}

void WorldPackets::VoidStorage::VoidStorageTransfer::Read()
{
    NpcGuid[7] = _worldPacket.ReadBit();
    NpcGuid[4] = _worldPacket.ReadBit();

    CountDeposit = _worldPacket.ReadBits(24);

    ItemGuids.resize(CountDeposit);
    for (uint32 i = 0; i < CountDeposit; ++i)
    {
        ItemGuids[i][0] = _worldPacket.ReadBit();
        ItemGuids[i][3] = _worldPacket.ReadBit();
        ItemGuids[i][6] = _worldPacket.ReadBit();
        ItemGuids[i][5] = _worldPacket.ReadBit();
        ItemGuids[i][4] = _worldPacket.ReadBit();
        ItemGuids[i][2] = _worldPacket.ReadBit();
        ItemGuids[i][1] = _worldPacket.ReadBit();
        ItemGuids[i][7] = _worldPacket.ReadBit();
    }

    CountWithdraw = _worldPacket.ReadBits(24);

    ItemIds.resize(CountWithdraw);
    for (uint32 i = 0; i < CountWithdraw; ++i)
    {
        ItemIds[i][4] = _worldPacket.ReadBit();
        ItemIds[i][0] = _worldPacket.ReadBit();
        ItemIds[i][5] = _worldPacket.ReadBit();
        ItemIds[i][7] = _worldPacket.ReadBit();
        ItemIds[i][6] = _worldPacket.ReadBit();
        ItemIds[i][1] = _worldPacket.ReadBit();
        ItemIds[i][2] = _worldPacket.ReadBit();
        ItemIds[i][3] = _worldPacket.ReadBit();
    }

    NpcGuid[6] = _worldPacket.ReadBit();
    NpcGuid[0] = _worldPacket.ReadBit();
    NpcGuid[3] = _worldPacket.ReadBit();
    NpcGuid[1] = _worldPacket.ReadBit();
    NpcGuid[2] = _worldPacket.ReadBit();
    NpcGuid[5] = _worldPacket.ReadBit();

    for (uint32 i = 0; i < CountDeposit; ++i)
    {
        _worldPacket.ReadByteSeq(ItemGuids[i][5]);
        _worldPacket.ReadByteSeq(ItemGuids[i][6]);
        _worldPacket.ReadByteSeq(ItemGuids[i][3]);
        _worldPacket.ReadByteSeq(ItemGuids[i][4]);
        _worldPacket.ReadByteSeq(ItemGuids[i][1]);
        _worldPacket.ReadByteSeq(ItemGuids[i][7]);
        _worldPacket.ReadByteSeq(ItemGuids[i][2]);
        _worldPacket.ReadByteSeq(ItemGuids[i][0]);
    }

    _worldPacket.ReadByteSeq(NpcGuid[5]);

    for (uint32 i = 0; i < CountWithdraw; ++i)
    {
        _worldPacket.ReadByteSeq(ItemIds[i][0]);
        _worldPacket.ReadByteSeq(ItemIds[i][4]);
        _worldPacket.ReadByteSeq(ItemIds[i][1]);
        _worldPacket.ReadByteSeq(ItemIds[i][2]);
        _worldPacket.ReadByteSeq(ItemIds[i][6]);
        _worldPacket.ReadByteSeq(ItemIds[i][3]);
        _worldPacket.ReadByteSeq(ItemIds[i][7]);
        _worldPacket.ReadByteSeq(ItemIds[i][5]);
    }

    _worldPacket.ReadByteSeq(NpcGuid[1]);
    _worldPacket.ReadByteSeq(NpcGuid[7]);
    _worldPacket.ReadByteSeq(NpcGuid[4]);
    _worldPacket.ReadByteSeq(NpcGuid[3]);
    _worldPacket.ReadByteSeq(NpcGuid[2]);
    _worldPacket.ReadByteSeq(NpcGuid[0]);
    _worldPacket.ReadByteSeq(NpcGuid[6]);
}

void WorldPackets::VoidStorage::VoidSwapItem::Read()
{
    _worldPacket >> NewSlot;

    NpcGuid[6] = _worldPacket.ReadBit();
    ItemId[4] = _worldPacket.ReadBit();
    ItemId[7] = _worldPacket.ReadBit();
    ItemId[3] = _worldPacket.ReadBit();
    ItemId[2] = _worldPacket.ReadBit();
    NpcGuid[4] = _worldPacket.ReadBit();
    NpcGuid[2] = _worldPacket.ReadBit();
    ItemId[0] = _worldPacket.ReadBit();
    ItemId[1] = _worldPacket.ReadBit();
    NpcGuid[7] = _worldPacket.ReadBit();
    NpcGuid[1] = _worldPacket.ReadBit();
    ItemId[6] = _worldPacket.ReadBit();
    NpcGuid[3] = _worldPacket.ReadBit();
    NpcGuid[5] = _worldPacket.ReadBit();
    ItemId[5] = _worldPacket.ReadBit();
    NpcGuid[0] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(NpcGuid[3]);
    _worldPacket.ReadByteSeq(NpcGuid[5]);
    _worldPacket.ReadByteSeq(ItemId[6]);
    _worldPacket.ReadByteSeq(NpcGuid[4]);
    _worldPacket.ReadByteSeq(ItemId[4]);
    _worldPacket.ReadByteSeq(NpcGuid[0]);
    _worldPacket.ReadByteSeq(ItemId[5]);
    _worldPacket.ReadByteSeq(ItemId[7]);
    _worldPacket.ReadByteSeq(NpcGuid[7]);
    _worldPacket.ReadByteSeq(NpcGuid[2]);
    _worldPacket.ReadByteSeq(NpcGuid[1]);
    _worldPacket.ReadByteSeq(ItemId[1]);
    _worldPacket.ReadByteSeq(ItemId[3]);
    _worldPacket.ReadByteSeq(NpcGuid[6]);
    _worldPacket.ReadByteSeq(ItemId[0]);
    _worldPacket.ReadByteSeq(ItemId[2]);
}
