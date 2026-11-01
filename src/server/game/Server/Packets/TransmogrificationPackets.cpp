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

#include "TransmogrificationPackets.h"
#include "Player.h"

void WorldPackets::Transmogrification::TransmogrifyItems::Read()
{
    NpcGuid[5] = _worldPacket.ReadBit();
    NpcGuid[6] = _worldPacket.ReadBit();
    NpcGuid[1] = _worldPacket.ReadBit();
    NpcGuid[2] = _worldPacket.ReadBit();
    NpcGuid[3] = _worldPacket.ReadBit();
    NpcGuid[4] = _worldPacket.ReadBit();
    Count = _worldPacket.ReadBits(21);

    ItemGuids.resize(Count, ObjectGuid::Empty);
    NewEntries.resize(Count, 0);
    Slots.resize(Count, 0);
    std::vector<bool> HasItemBonus(Count, false);
    std::vector<bool> HasModifications(Count, false);

    for (uint8 i = 0; i < Count; ++i)
    {
        HasItemBonus[i] = _worldPacket.ReadBit();
        HasModifications[i] = _worldPacket.ReadBit();
    }

    if (Count >= EQUIPMENT_SLOT_END)
    {
        _worldPacket.rfinish();
        return;
    }

    NpcGuid[0] = _worldPacket.ReadBit();
    NpcGuid[7] = _worldPacket.ReadBit();

    for (uint8 i = 0; i < Count; ++i)
    {
        if (HasModifications[i])
        {
            ItemGuids[i][5] = _worldPacket.ReadBit();
            ItemGuids[i][6] = _worldPacket.ReadBit();
            ItemGuids[i][1] = _worldPacket.ReadBit();
            ItemGuids[i][3] = _worldPacket.ReadBit();
            ItemGuids[i][0] = _worldPacket.ReadBit();
            ItemGuids[i][4] = _worldPacket.ReadBit();
            ItemGuids[i][7] = _worldPacket.ReadBit();
            ItemGuids[i][2] = _worldPacket.ReadBit();
        }

        if (HasItemBonus[i])
        {
            ItemGuids[i][4] = _worldPacket.ReadBit();
            ItemGuids[i][1] = _worldPacket.ReadBit();
            ItemGuids[i][0] = _worldPacket.ReadBit();
            ItemGuids[i][6] = _worldPacket.ReadBit();
            ItemGuids[i][5] = _worldPacket.ReadBit();
            ItemGuids[i][2] = _worldPacket.ReadBit();
            ItemGuids[i][7] = _worldPacket.ReadBit();
            ItemGuids[i][3] = _worldPacket.ReadBit();
        }
    }

    for (uint8 i = 0; i < Count; ++i)
    {
        _worldPacket >> Slots[i];
        _worldPacket >> NewEntries[i];
    }

    _worldPacket.ReadByteSeq(NpcGuid[5]);
    _worldPacket.ReadByteSeq(NpcGuid[0]);
    _worldPacket.ReadByteSeq(NpcGuid[1]);
    _worldPacket.ReadByteSeq(NpcGuid[2]);
    _worldPacket.ReadByteSeq(NpcGuid[3]);
    _worldPacket.ReadByteSeq(NpcGuid[4]);
    _worldPacket.ReadByteSeq(NpcGuid[6]);
    _worldPacket.ReadByteSeq(NpcGuid[7]);

    for (uint8 i = 0; i < Count; ++i)
    {
        if (HasModifications[i])
        {
            _worldPacket.ReadByteSeq(ItemGuids[i][2]);
            _worldPacket.ReadByteSeq(ItemGuids[i][5]);
            _worldPacket.ReadByteSeq(ItemGuids[i][4]);
            _worldPacket.ReadByteSeq(ItemGuids[i][3]);
            _worldPacket.ReadByteSeq(ItemGuids[i][6]);
            _worldPacket.ReadByteSeq(ItemGuids[i][0]);
            _worldPacket.ReadByteSeq(ItemGuids[i][7]);
            _worldPacket.ReadByteSeq(ItemGuids[i][1]);
        }

        if (HasItemBonus[i])
        {
            _worldPacket.ReadByteSeq(ItemGuids[i][7]);
            _worldPacket.ReadByteSeq(ItemGuids[i][1]);
            _worldPacket.ReadByteSeq(ItemGuids[i][6]);
            _worldPacket.ReadByteSeq(ItemGuids[i][5]);
            _worldPacket.ReadByteSeq(ItemGuids[i][4]);
            _worldPacket.ReadByteSeq(ItemGuids[i][3]);
            _worldPacket.ReadByteSeq(ItemGuids[i][0]);
            _worldPacket.ReadByteSeq(ItemGuids[i][2]);
        }
    }
}
