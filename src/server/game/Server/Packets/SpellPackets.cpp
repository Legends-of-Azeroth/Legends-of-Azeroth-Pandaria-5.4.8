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

#include "SpellPackets.h"
#include "SharedDefines.h"
#include "Spell.h"
#include "SpellInfo.h"

namespace WorldPackets::Spells
{

WorldPacket const* ClearTarget::Write()
{
    _worldPacket.WriteBit(Guid[6]);
    _worldPacket.WriteBit(Guid[2]);
    _worldPacket.WriteBit(Guid[0]);
    _worldPacket.WriteBit(Guid[4]);
    _worldPacket.WriteBit(Guid[7]);
    _worldPacket.WriteBit(Guid[1]);
    _worldPacket.WriteBit(Guid[3]);
    _worldPacket.WriteBit(Guid[5]);

    _worldPacket.WriteByteSeq(Guid[4]);
    _worldPacket.WriteByteSeq(Guid[0]);
    _worldPacket.WriteByteSeq(Guid[3]);
    _worldPacket.WriteByteSeq(Guid[5]);
    _worldPacket.WriteByteSeq(Guid[2]);
    _worldPacket.WriteByteSeq(Guid[7]);
    _worldPacket.WriteByteSeq(Guid[6]);
    _worldPacket.WriteByteSeq(Guid[1]);

    return &_worldPacket;
}

}