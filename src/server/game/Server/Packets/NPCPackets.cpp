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

#include "NPCPackets.h"

void WorldPackets::NPC::BinderActivate::Read()
{
    Unit[0] = _worldPacket.ReadBit();
    Unit[5] = _worldPacket.ReadBit();
    Unit[4] = _worldPacket.ReadBit();
    Unit[7] = _worldPacket.ReadBit();
    Unit[6] = _worldPacket.ReadBit();
    Unit[2] = _worldPacket.ReadBit();
    Unit[1] = _worldPacket.ReadBit();
    Unit[3] = _worldPacket.ReadBit();
    _worldPacket.ReadByteSeq(Unit[0]);
    _worldPacket.ReadByteSeq(Unit[4]);
    _worldPacket.ReadByteSeq(Unit[2]);
    _worldPacket.ReadByteSeq(Unit[3]);
    _worldPacket.ReadByteSeq(Unit[7]);
    _worldPacket.ReadByteSeq(Unit[1]);
    _worldPacket.ReadByteSeq(Unit[5]);
    _worldPacket.ReadByteSeq(Unit[6]);
}

void WorldPackets::NPC::GossipHello::Read()
{
    Unit[2] = _worldPacket.ReadBit();
    Unit[4] = _worldPacket.ReadBit();
    Unit[0] = _worldPacket.ReadBit();
    Unit[3] = _worldPacket.ReadBit();
    Unit[6] = _worldPacket.ReadBit();
    Unit[7] = _worldPacket.ReadBit();
    Unit[5] = _worldPacket.ReadBit();
    Unit[1] = _worldPacket.ReadBit();
    _worldPacket.ReadByteSeq(Unit[4]);
    _worldPacket.ReadByteSeq(Unit[7]);
    _worldPacket.ReadByteSeq(Unit[1]);
    _worldPacket.ReadByteSeq(Unit[0]);
    _worldPacket.ReadByteSeq(Unit[5]);
    _worldPacket.ReadByteSeq(Unit[3]);
    _worldPacket.ReadByteSeq(Unit[6]);
    _worldPacket.ReadByteSeq(Unit[2]);
}

void WorldPackets::NPC::ListInventory::Read()
{
    Unit[6] = _worldPacket.ReadBit();
    Unit[7] = _worldPacket.ReadBit();
    Unit[3] = _worldPacket.ReadBit();
    Unit[1] = _worldPacket.ReadBit();
    Unit[2] = _worldPacket.ReadBit();
    Unit[0] = _worldPacket.ReadBit();
    Unit[4] = _worldPacket.ReadBit();
    Unit[5] = _worldPacket.ReadBit();
    _worldPacket.ReadByteSeq(Unit[0]);
    _worldPacket.ReadByteSeq(Unit[7]);
    _worldPacket.ReadByteSeq(Unit[1]);
    _worldPacket.ReadByteSeq(Unit[6]);
    _worldPacket.ReadByteSeq(Unit[4]);
    _worldPacket.ReadByteSeq(Unit[3]);
    _worldPacket.ReadByteSeq(Unit[5]);
    _worldPacket.ReadByteSeq(Unit[2]);
}

void WorldPackets::NPC::TrainerList::Read()
{
    Unit[0] = _worldPacket.ReadBit();
    Unit[2] = _worldPacket.ReadBit();
    Unit[7] = _worldPacket.ReadBit();
    Unit[6] = _worldPacket.ReadBit();
    Unit[1] = _worldPacket.ReadBit();
    Unit[4] = _worldPacket.ReadBit();
    Unit[5] = _worldPacket.ReadBit();
    Unit[3] = _worldPacket.ReadBit();
    _worldPacket.ReadByteSeq(Unit[3]);
    _worldPacket.ReadByteSeq(Unit[6]);
    _worldPacket.ReadByteSeq(Unit[7]);
    _worldPacket.ReadByteSeq(Unit[5]);
    _worldPacket.ReadByteSeq(Unit[1]);
    _worldPacket.ReadByteSeq(Unit[0]);
    _worldPacket.ReadByteSeq(Unit[2]);
    _worldPacket.ReadByteSeq(Unit[4]);
}

void WorldPackets::NPC::TrainerBuySpell::Read()
{
    _worldPacket >> SpellID >> TrainerID;
    TrainerGUID[1] = _worldPacket.ReadBit();
    TrainerGUID[4] = _worldPacket.ReadBit();
    TrainerGUID[0] = _worldPacket.ReadBit();
    TrainerGUID[6] = _worldPacket.ReadBit();
    TrainerGUID[3] = _worldPacket.ReadBit();
    TrainerGUID[2] = _worldPacket.ReadBit();
    TrainerGUID[5] = _worldPacket.ReadBit();
    TrainerGUID[7] = _worldPacket.ReadBit();
    _worldPacket.ReadByteSeq(TrainerGUID[3]);
    _worldPacket.ReadByteSeq(TrainerGUID[1]);
    _worldPacket.ReadByteSeq(TrainerGUID[4]);
    _worldPacket.ReadByteSeq(TrainerGUID[7]);
    _worldPacket.ReadByteSeq(TrainerGUID[0]);
    _worldPacket.ReadByteSeq(TrainerGUID[5]);
    _worldPacket.ReadByteSeq(TrainerGUID[6]);
    _worldPacket.ReadByteSeq(TrainerGUID[2]);
}

void WorldPackets::NPC::TabardVendorActivate::Read()
{
    Unit[2] = _worldPacket.ReadBit();
    Unit[1] = _worldPacket.ReadBit();
    Unit[4] = _worldPacket.ReadBit();
    Unit[6] = _worldPacket.ReadBit();
    Unit[3] = _worldPacket.ReadBit();
    Unit[5] = _worldPacket.ReadBit();
    Unit[0] = _worldPacket.ReadBit();
    Unit[7] = _worldPacket.ReadBit();
    _worldPacket.ReadByteSeq(Unit[7]);
    _worldPacket.ReadByteSeq(Unit[6]);
    _worldPacket.ReadByteSeq(Unit[2]);
    _worldPacket.ReadByteSeq(Unit[5]);
    _worldPacket.ReadByteSeq(Unit[0]);
    _worldPacket.ReadByteSeq(Unit[1]);
    _worldPacket.ReadByteSeq(Unit[4]);
    _worldPacket.ReadByteSeq(Unit[3]);
}

void WorldPackets::NPC::SpiritHealerActivate::Read()
{
    Unit[2] = _worldPacket.ReadBit();
    Unit[7] = _worldPacket.ReadBit();
    Unit[6] = _worldPacket.ReadBit();
    Unit[0] = _worldPacket.ReadBit();
    Unit[5] = _worldPacket.ReadBit();
    Unit[4] = _worldPacket.ReadBit();
    Unit[1] = _worldPacket.ReadBit();
    Unit[3] = _worldPacket.ReadBit();
    _worldPacket.ReadByteSeq(Unit[1]);
    _worldPacket.ReadByteSeq(Unit[5]);
    _worldPacket.ReadByteSeq(Unit[6]);
    _worldPacket.ReadByteSeq(Unit[3]);
    _worldPacket.ReadByteSeq(Unit[2]);
    _worldPacket.ReadByteSeq(Unit[0]);
    _worldPacket.ReadByteSeq(Unit[7]);
    _worldPacket.ReadByteSeq(Unit[4]);
}

void WorldPackets::NPC::ListPets::Read()
{
    Unit[0] = _worldPacket.ReadBit();
    Unit[5] = _worldPacket.ReadBit();
    Unit[1] = _worldPacket.ReadBit();
    Unit[3] = _worldPacket.ReadBit();
    Unit[6] = _worldPacket.ReadBit();
    Unit[7] = _worldPacket.ReadBit();
    Unit[2] = _worldPacket.ReadBit();
    Unit[4] = _worldPacket.ReadBit();
    _worldPacket.ReadByteSeq(Unit[0]);
    _worldPacket.ReadByteSeq(Unit[5]);
    _worldPacket.ReadByteSeq(Unit[7]);
    _worldPacket.ReadByteSeq(Unit[1]);
    _worldPacket.ReadByteSeq(Unit[2]);
    _worldPacket.ReadByteSeq(Unit[3]);
    _worldPacket.ReadByteSeq(Unit[4]);
    _worldPacket.ReadByteSeq(Unit[6]);
}

void WorldPackets::NPC::SetPetSlot::Read()
{
    _worldPacket >> OldPetId;
    _worldPacket >> NewSlot;
    Unit[5] = _worldPacket.ReadBit();
    Unit[7] = _worldPacket.ReadBit();
    Unit[3] = _worldPacket.ReadBit();
    Unit[2] = _worldPacket.ReadBit();
    Unit[6] = _worldPacket.ReadBit();
    Unit[1] = _worldPacket.ReadBit();
    Unit[0] = _worldPacket.ReadBit();
    Unit[4] = _worldPacket.ReadBit();
    _worldPacket.ReadByteSeq(Unit[0]);
    _worldPacket.ReadByteSeq(Unit[3]);
    _worldPacket.ReadByteSeq(Unit[2]);
    _worldPacket.ReadByteSeq(Unit[6]);
    _worldPacket.ReadByteSeq(Unit[5]);
    _worldPacket.ReadByteSeq(Unit[7]);
    _worldPacket.ReadByteSeq(Unit[4]);
    _worldPacket.ReadByteSeq(Unit[1]);
}
