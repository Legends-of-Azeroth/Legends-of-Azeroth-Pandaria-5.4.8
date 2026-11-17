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

#include "TaxiPackets.h"

void WorldPackets::Taxi::ActivateTaxi::Read()
{
    Nodes.resize(2);
    _worldPacket >> Nodes[1] >> Nodes[0];

    Guid[4] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[7]);
}

void WorldPackets::Taxi::ActivateTaxiExpress::Read()
{
    Guid[6] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    uint32 node_count = _worldPacket.ReadBits(22);
    Guid[2] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[1]);

    for (uint32 i = 0; i < node_count; i++)
    {
        uint32 node;
        _worldPacket >> node;
        Nodes.push_back(node);
    }

    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[4]);
}

void WorldPackets::Taxi::EnableTaxiNode::Read()
{
    Guid[3] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[2]);
}

void WorldPackets::Taxi::TaxiNodeStatusQuery::Read()
{
    Guid[7] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[3]);
}

void WorldPackets::Taxi::TaxiQueryAvailableNodes::Read()
{
    Guid[7] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[1]);
}
