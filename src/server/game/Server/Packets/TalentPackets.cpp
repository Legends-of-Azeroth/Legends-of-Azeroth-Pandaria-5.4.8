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

#include "TalentPackets.h"

void WorldPackets::Talent::SetPrimaryTalentTree::Read()
{
    _worldPacket >> TabIndex;
}

void WorldPackets::Talent::LearnTalent::Read()
{
    uint32 const count = _worldPacket.ReadBits(23);
    Talents.resize(count);
    for (uint32 i = 0; i < count; ++i)
        _worldPacket >> Talents[i];
}

void WorldPackets::Talent::ConfirmRespecWipe::Read()
{
    _worldPacket >> RespecType;
    RespecMaster[7] = _worldPacket.ReadBit();
    RespecMaster[2] = _worldPacket.ReadBit();
    RespecMaster[6] = _worldPacket.ReadBit();
    RespecMaster[1] = _worldPacket.ReadBit();
    RespecMaster[4] = _worldPacket.ReadBit();
    RespecMaster[0] = _worldPacket.ReadBit();
    RespecMaster[3] = _worldPacket.ReadBit();
    RespecMaster[5] = _worldPacket.ReadBit();
    _worldPacket.ReadByteSeq(RespecMaster[2]);
    _worldPacket.ReadByteSeq(RespecMaster[4]);
    _worldPacket.ReadByteSeq(RespecMaster[1]);
    _worldPacket.ReadByteSeq(RespecMaster[3]);
    _worldPacket.ReadByteSeq(RespecMaster[0]);
    _worldPacket.ReadByteSeq(RespecMaster[5]);
    _worldPacket.ReadByteSeq(RespecMaster[7]);
    _worldPacket.ReadByteSeq(RespecMaster[6]);
}

void WorldPackets::Talent::UnlearnSpecialization::Read()
{
    _worldPacket >> SpecIndex;
}

void WorldPackets::Talent::UnlearnSkill::Read()
{
    _worldPacket >> SkillId;
}
