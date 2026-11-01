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

#include "CombatPackets.h"

void WorldPackets::Combat::AttackSwing::Read()
{
    TargetGUID[6] = _worldPacket.ReadBit();
    TargetGUID[5] = _worldPacket.ReadBit();
    TargetGUID[7] = _worldPacket.ReadBit();
    TargetGUID[0] = _worldPacket.ReadBit();
    TargetGUID[3] = _worldPacket.ReadBit();
    TargetGUID[1] = _worldPacket.ReadBit();
    TargetGUID[4] = _worldPacket.ReadBit();
    TargetGUID[2] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(TargetGUID[6]);
    _worldPacket.ReadByteSeq(TargetGUID[7]);
    _worldPacket.ReadByteSeq(TargetGUID[1]);
    _worldPacket.ReadByteSeq(TargetGUID[3]);
    _worldPacket.ReadByteSeq(TargetGUID[2]);
    _worldPacket.ReadByteSeq(TargetGUID[0]);
    _worldPacket.ReadByteSeq(TargetGUID[4]);
    _worldPacket.ReadByteSeq(TargetGUID[5]);
}

void WorldPackets::Combat::SetSheathed::Read()
{
    _worldPacket >> Sheathed;
    HasData = _worldPacket.ReadBit();
}

void WorldPackets::Combat::CancelAura::Read()
{
    _worldPacket >> SpellId;

    _worldPacket.ReadBit(); // fake bit
    TargetGUID[6] = _worldPacket.ReadBit();
    TargetGUID[5] = _worldPacket.ReadBit();
    TargetGUID[1] = _worldPacket.ReadBit();
    TargetGUID[0] = _worldPacket.ReadBit();
    TargetGUID[4] = _worldPacket.ReadBit();
    TargetGUID[3] = _worldPacket.ReadBit();
    TargetGUID[2] = _worldPacket.ReadBit();
    TargetGUID[7] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(TargetGUID[3]);
    _worldPacket.ReadByteSeq(TargetGUID[2]);
    _worldPacket.ReadByteSeq(TargetGUID[1]);
    _worldPacket.ReadByteSeq(TargetGUID[0]);
    _worldPacket.ReadByteSeq(TargetGUID[4]);
    _worldPacket.ReadByteSeq(TargetGUID[7]);
    _worldPacket.ReadByteSeq(TargetGUID[5]);
    _worldPacket.ReadByteSeq(TargetGUID[6]);
}

void WorldPackets::Combat::CancelCast::Read()
{
    bool hasCounter = !_worldPacket.ReadBit();
    bool hasSpellId = !_worldPacket.ReadBit();

    if (hasSpellId)
        _worldPacket >> SpellId;

    if (hasCounter)
        _worldPacket >> Counter;
}

void WorldPackets::Combat::CancelChannelling::Read()
{
    _worldPacket >> SpellId;
}

void WorldPackets::Combat::CancelTempEnchantment::Read()
{
    _worldPacket >> Slot;
}

void WorldPackets::Combat::CancelModSpeedNoControlAuras::Read()
{
    TargetGUID[3] = _worldPacket.ReadBit();
    TargetGUID[2] = _worldPacket.ReadBit();
    TargetGUID[5] = _worldPacket.ReadBit();
    TargetGUID[0] = _worldPacket.ReadBit();
    TargetGUID[7] = _worldPacket.ReadBit();
    TargetGUID[1] = _worldPacket.ReadBit();
    TargetGUID[6] = _worldPacket.ReadBit();
    TargetGUID[4] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(TargetGUID[6]);
    _worldPacket.ReadByteSeq(TargetGUID[7]);
    _worldPacket.ReadByteSeq(TargetGUID[3]);
    _worldPacket.ReadByteSeq(TargetGUID[5]);
    _worldPacket.ReadByteSeq(TargetGUID[0]);
    _worldPacket.ReadByteSeq(TargetGUID[4]);
    _worldPacket.ReadByteSeq(TargetGUID[1]);
    _worldPacket.ReadByteSeq(TargetGUID[2]);
}

void WorldPackets::Combat::ResurrectResponse::Read()
{
    _worldPacket >> Status;

    ResurrectingPlayer[3] = _worldPacket.ReadBit();
    ResurrectingPlayer[0] = _worldPacket.ReadBit();
    ResurrectingPlayer[6] = _worldPacket.ReadBit();
    ResurrectingPlayer[4] = _worldPacket.ReadBit();
    ResurrectingPlayer[5] = _worldPacket.ReadBit();
    ResurrectingPlayer[2] = _worldPacket.ReadBit();
    ResurrectingPlayer[1] = _worldPacket.ReadBit();
    ResurrectingPlayer[7] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(ResurrectingPlayer[7]);
    _worldPacket.ReadByteSeq(ResurrectingPlayer[0]);
    _worldPacket.ReadByteSeq(ResurrectingPlayer[1]);
    _worldPacket.ReadByteSeq(ResurrectingPlayer[3]);
    _worldPacket.ReadByteSeq(ResurrectingPlayer[4]);
    _worldPacket.ReadByteSeq(ResurrectingPlayer[6]);
    _worldPacket.ReadByteSeq(ResurrectingPlayer[2]);
    _worldPacket.ReadByteSeq(ResurrectingPlayer[5]);
}

void WorldPackets::Combat::ReclaimCorpse::Read()
{
    TargetGUID[1] = _worldPacket.ReadBit();
    TargetGUID[5] = _worldPacket.ReadBit();
    TargetGUID[7] = _worldPacket.ReadBit();
    TargetGUID[2] = _worldPacket.ReadBit();
    TargetGUID[6] = _worldPacket.ReadBit();
    TargetGUID[3] = _worldPacket.ReadBit();
    TargetGUID[0] = _worldPacket.ReadBit();
    TargetGUID[4] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(TargetGUID[2]);
    _worldPacket.ReadByteSeq(TargetGUID[5]);
    _worldPacket.ReadByteSeq(TargetGUID[4]);
    _worldPacket.ReadByteSeq(TargetGUID[6]);
    _worldPacket.ReadByteSeq(TargetGUID[1]);
    _worldPacket.ReadByteSeq(TargetGUID[0]);
    _worldPacket.ReadByteSeq(TargetGUID[7]);
    _worldPacket.ReadByteSeq(TargetGUID[3]);
}

void WorldPackets::Combat::RepopRequest::Read()
{
    _worldPacket >> Unk;
}

void WorldPackets::Combat::SetAdvancedCombatLogging::Read()
{
    _worldPacket.read_skip<bool>();
}
