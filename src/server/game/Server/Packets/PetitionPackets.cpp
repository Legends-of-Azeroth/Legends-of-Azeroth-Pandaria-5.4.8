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

#include "PetitionPackets.h"

void WorldPackets::Petitions::PetitionBuy::Read()
{
    uint8 nameLen;

    NpcGuid[5] = _worldPacket.ReadBit();
    NpcGuid[2] = _worldPacket.ReadBit();
    NpcGuid[3] = _worldPacket.ReadBit();
    nameLen = _worldPacket.ReadBits(7);
    NpcGuid[4] = _worldPacket.ReadBit();
    NpcGuid[1] = _worldPacket.ReadBit();
    NpcGuid[7] = _worldPacket.ReadBit();
    NpcGuid[0] = _worldPacket.ReadBit();
    NpcGuid[6] = _worldPacket.ReadBit();

    Name = _worldPacket.ReadString(nameLen);
    _worldPacket.ReadByteSeq(NpcGuid[1]);
    _worldPacket.ReadByteSeq(NpcGuid[7]);
    _worldPacket.ReadByteSeq(NpcGuid[4]);
    _worldPacket.ReadByteSeq(NpcGuid[6]);
    _worldPacket.ReadByteSeq(NpcGuid[0]);
    _worldPacket.ReadByteSeq(NpcGuid[5]);
    _worldPacket.ReadByteSeq(NpcGuid[2]);
    _worldPacket.ReadByteSeq(NpcGuid[3]);
}

void WorldPackets::Petitions::PetitionShowSignatures::Read()
{
    PetitionGuid[3] = _worldPacket.ReadBit();
    PetitionGuid[7] = _worldPacket.ReadBit();
    PetitionGuid[2] = _worldPacket.ReadBit();
    PetitionGuid[4] = _worldPacket.ReadBit();
    PetitionGuid[5] = _worldPacket.ReadBit();
    PetitionGuid[6] = _worldPacket.ReadBit();
    PetitionGuid[0] = _worldPacket.ReadBit();
    PetitionGuid[1] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(PetitionGuid[2]);
    _worldPacket.ReadByteSeq(PetitionGuid[4]);
    _worldPacket.ReadByteSeq(PetitionGuid[5]);
    _worldPacket.ReadByteSeq(PetitionGuid[7]);
    _worldPacket.ReadByteSeq(PetitionGuid[1]);
    _worldPacket.ReadByteSeq(PetitionGuid[0]);
    _worldPacket.ReadByteSeq(PetitionGuid[3]);
    _worldPacket.ReadByteSeq(PetitionGuid[6]);
}

void WorldPackets::Petitions::QueryPetition::Read()
{
    _worldPacket >> GuildGuid;

    PetitionGuid[2] = _worldPacket.ReadBit();
    PetitionGuid[3] = _worldPacket.ReadBit();
    PetitionGuid[1] = _worldPacket.ReadBit();
    PetitionGuid[0] = _worldPacket.ReadBit();
    PetitionGuid[4] = _worldPacket.ReadBit();
    PetitionGuid[7] = _worldPacket.ReadBit();
    PetitionGuid[6] = _worldPacket.ReadBit();
    PetitionGuid[5] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(PetitionGuid[0]);
    _worldPacket.ReadByteSeq(PetitionGuid[4]);
    _worldPacket.ReadByteSeq(PetitionGuid[7]);
    _worldPacket.ReadByteSeq(PetitionGuid[5]);
    _worldPacket.ReadByteSeq(PetitionGuid[1]);
    _worldPacket.ReadByteSeq(PetitionGuid[6]);
    _worldPacket.ReadByteSeq(PetitionGuid[3]);
    _worldPacket.ReadByteSeq(PetitionGuid[2]);
}

void WorldPackets::Petitions::PetitionRename::Read()
{
    uint8 nameLen;

    nameLen = _worldPacket.ReadBits(7);
    PetitionGuid[7] = _worldPacket.ReadBit();
    PetitionGuid[4] = _worldPacket.ReadBit();
    PetitionGuid[6] = _worldPacket.ReadBit();
    PetitionGuid[2] = _worldPacket.ReadBit();
    PetitionGuid[0] = _worldPacket.ReadBit();
    PetitionGuid[5] = _worldPacket.ReadBit();
    PetitionGuid[3] = _worldPacket.ReadBit();
    PetitionGuid[1] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(PetitionGuid[4]);
    _worldPacket.ReadByteSeq(PetitionGuid[1]);
    _worldPacket.ReadByteSeq(PetitionGuid[7]);
    NewName = _worldPacket.ReadString(nameLen);
    _worldPacket.ReadByteSeq(PetitionGuid[0]);
    _worldPacket.ReadByteSeq(PetitionGuid[3]);
    _worldPacket.ReadByteSeq(PetitionGuid[2]);
    _worldPacket.ReadByteSeq(PetitionGuid[6]);
    _worldPacket.ReadByteSeq(PetitionGuid[5]);
}

void WorldPackets::Petitions::SignPetition::Read()
{
    _worldPacket.read_skip<uint8>();

    PetitionGuid[4] = _worldPacket.ReadBit();
    PetitionGuid[2] = _worldPacket.ReadBit();
    PetitionGuid[0] = _worldPacket.ReadBit();
    PetitionGuid[1] = _worldPacket.ReadBit();
    PetitionGuid[5] = _worldPacket.ReadBit();
    PetitionGuid[3] = _worldPacket.ReadBit();
    PetitionGuid[6] = _worldPacket.ReadBit();
    PetitionGuid[7] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(PetitionGuid[6]);
    _worldPacket.ReadByteSeq(PetitionGuid[1]);
    _worldPacket.ReadByteSeq(PetitionGuid[7]);
    _worldPacket.ReadByteSeq(PetitionGuid[2]);
    _worldPacket.ReadByteSeq(PetitionGuid[5]);
    _worldPacket.ReadByteSeq(PetitionGuid[3]);
    _worldPacket.ReadByteSeq(PetitionGuid[0]);
    _worldPacket.ReadByteSeq(PetitionGuid[4]);
}

void WorldPackets::Petitions::PetitionDecline::Read()
{
    PetitionGuid[5] = _worldPacket.ReadBit();
    PetitionGuid[6] = _worldPacket.ReadBit();
    PetitionGuid[4] = _worldPacket.ReadBit();
    PetitionGuid[3] = _worldPacket.ReadBit();
    PetitionGuid[1] = _worldPacket.ReadBit();
    PetitionGuid[7] = _worldPacket.ReadBit();
    PetitionGuid[0] = _worldPacket.ReadBit();
    PetitionGuid[2] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(PetitionGuid[6]);
    _worldPacket.ReadByteSeq(PetitionGuid[2]);
    _worldPacket.ReadByteSeq(PetitionGuid[1]);
    _worldPacket.ReadByteSeq(PetitionGuid[5]);
    _worldPacket.ReadByteSeq(PetitionGuid[0]);
    _worldPacket.ReadByteSeq(PetitionGuid[7]);
    _worldPacket.ReadByteSeq(PetitionGuid[4]);
    _worldPacket.ReadByteSeq(PetitionGuid[3]);
}

void WorldPackets::Petitions::OfferPetition::Read()
{
    _worldPacket.read_skip<uint32>();

    PlayerGuid[4] = _worldPacket.ReadBit();
    PlayerGuid[1] = _worldPacket.ReadBit();
    PetitionGuid[2] = _worldPacket.ReadBit();
    PlayerGuid[6] = _worldPacket.ReadBit();
    PetitionGuid[1] = _worldPacket.ReadBit();
    PlayerGuid[2] = _worldPacket.ReadBit();
    PetitionGuid[4] = _worldPacket.ReadBit();
    PlayerGuid[3] = _worldPacket.ReadBit();
    PlayerGuid[7] = _worldPacket.ReadBit();
    PetitionGuid[0] = _worldPacket.ReadBit();
    PetitionGuid[6] = _worldPacket.ReadBit();
    PlayerGuid[5] = _worldPacket.ReadBit();
    PlayerGuid[0] = _worldPacket.ReadBit();
    PetitionGuid[3] = _worldPacket.ReadBit();
    PetitionGuid[5] = _worldPacket.ReadBit();
    PetitionGuid[7] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(PlayerGuid[7]);
    _worldPacket.ReadByteSeq(PetitionGuid[1]);
    _worldPacket.ReadByteSeq(PetitionGuid[4]);
    _worldPacket.ReadByteSeq(PetitionGuid[2]);
    _worldPacket.ReadByteSeq(PlayerGuid[6]);
    _worldPacket.ReadByteSeq(PetitionGuid[3]);
    _worldPacket.ReadByteSeq(PetitionGuid[0]);
    _worldPacket.ReadByteSeq(PetitionGuid[5]);
    _worldPacket.ReadByteSeq(PlayerGuid[0]);
    _worldPacket.ReadByteSeq(PlayerGuid[2]);
    _worldPacket.ReadByteSeq(PlayerGuid[5]);
    _worldPacket.ReadByteSeq(PlayerGuid[3]);
    _worldPacket.ReadByteSeq(PlayerGuid[4]);
    _worldPacket.ReadByteSeq(PetitionGuid[7]);
    _worldPacket.ReadByteSeq(PlayerGuid[1]);
    _worldPacket.ReadByteSeq(PetitionGuid[6]);
}

void WorldPackets::Petitions::TurnInPetition::Read()
{
    PetitionGuid[1] = _worldPacket.ReadBit();
    PetitionGuid[2] = _worldPacket.ReadBit();
    PetitionGuid[3] = _worldPacket.ReadBit();
    PetitionGuid[0] = _worldPacket.ReadBit();
    PetitionGuid[5] = _worldPacket.ReadBit();
    PetitionGuid[7] = _worldPacket.ReadBit();
    PetitionGuid[4] = _worldPacket.ReadBit();
    PetitionGuid[6] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(PetitionGuid[2]);
    _worldPacket.ReadByteSeq(PetitionGuid[1]);
    _worldPacket.ReadByteSeq(PetitionGuid[4]);
    _worldPacket.ReadByteSeq(PetitionGuid[6]);
    _worldPacket.ReadByteSeq(PetitionGuid[0]);
    _worldPacket.ReadByteSeq(PetitionGuid[7]);
    _worldPacket.ReadByteSeq(PetitionGuid[5]);
    _worldPacket.ReadByteSeq(PetitionGuid[3]);
}

void WorldPackets::Petitions::PetitionShowList::Read()
{
    Guid[1] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[0]);
}
