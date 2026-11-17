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

#include "DuelPackets.h"

void WorldPackets::Duel::CanDuel::Read()
{
    TargetGUID[1] = _worldPacket.ReadBit();
    TargetGUID[5] = _worldPacket.ReadBit();
    TargetGUID[4] = _worldPacket.ReadBit();
    TargetGUID[6] = _worldPacket.ReadBit();
    TargetGUID[3] = _worldPacket.ReadBit();
    TargetGUID[2] = _worldPacket.ReadBit();
    TargetGUID[7] = _worldPacket.ReadBit();
    TargetGUID[0] = _worldPacket.ReadBit();
    _worldPacket.ReadByteSeq(TargetGUID[4]);
    _worldPacket.ReadByteSeq(TargetGUID[2]);
    _worldPacket.ReadByteSeq(TargetGUID[5]);
    _worldPacket.ReadByteSeq(TargetGUID[7]);
    _worldPacket.ReadByteSeq(TargetGUID[1]);
    _worldPacket.ReadByteSeq(TargetGUID[3]);
    _worldPacket.ReadByteSeq(TargetGUID[6]);
    _worldPacket.ReadByteSeq(TargetGUID[0]);
}

void WorldPackets::Duel::DuelResponse::Read()
{
    ArbiterGUID[7] = _worldPacket.ReadBit();
    ArbiterGUID[1] = _worldPacket.ReadBit();
    ArbiterGUID[3] = _worldPacket.ReadBit();
    ArbiterGUID[4] = _worldPacket.ReadBit();
    ArbiterGUID[0] = _worldPacket.ReadBit();
    ArbiterGUID[2] = _worldPacket.ReadBit();
    ArbiterGUID[6] = _worldPacket.ReadBit();
    Accepted = _worldPacket.ReadBit();
    ArbiterGUID[5] = _worldPacket.ReadBit();
    _worldPacket.ReadByteSeq(ArbiterGUID[6]);
    _worldPacket.ReadByteSeq(ArbiterGUID[4]);
    _worldPacket.ReadByteSeq(ArbiterGUID[5]);
    _worldPacket.ReadByteSeq(ArbiterGUID[0]);
    _worldPacket.ReadByteSeq(ArbiterGUID[1]);
    _worldPacket.ReadByteSeq(ArbiterGUID[2]);
    _worldPacket.ReadByteSeq(ArbiterGUID[7]);
    _worldPacket.ReadByteSeq(ArbiterGUID[3]);
}
