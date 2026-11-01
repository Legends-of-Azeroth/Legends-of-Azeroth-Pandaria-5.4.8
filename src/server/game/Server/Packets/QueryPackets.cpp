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

#include "QueryPackets.h"

void WorldPackets::Query::QueryGameObject::Read()
{
    _worldPacket >> GameObjectID;

    Guid[5] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[0]);
}

WorldPacket const* WorldPackets::Query::QueryGameObjectResponse::Write()
{
    _worldPacket.WriteBit(Allow);
    _worldPacket << GameObjectID;
    _worldPacket.FlushBits();

    if (Allow)
    {
        uint32 dataSize = Stats.GetDataSize();

        _worldPacket << uint32(dataSize);
        if (dataSize)
        {
            _worldPacket << Stats.Type;
            _worldPacket << Stats.DisplayID;
            for (int8 i = 0; i < 4; i++)
                _worldPacket << Stats.Name[i];

            _worldPacket << Stats.IconName;
            _worldPacket << Stats.CastBarCaption;
            _worldPacket << Stats.UnkString;

            for (uint32 i = 0; i < MAX_GAMEOBJECT_DATA; i++)
                _worldPacket << Stats.Data[i];

            _worldPacket << Stats.Size;

            _worldPacket << uint8(Stats.QuestItems.size());
            for (uint32 questItem : Stats.QuestItems)
                _worldPacket << questItem;

            _worldPacket << Stats.UnkInt32;
        }
    }

    return &_worldPacket;
}

void WorldPackets::Query::QueryPlayerName::Read()
{
    uint8 bit14, bit1C;

    Guid[4] = _worldPacket.ReadBit();
    bit14 = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    bit1C = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[4]);

    if (bit14)
        _worldPacket.read<uint32>();

    if (bit1C)
        _worldPacket.read<uint32>();
}

void WorldPackets::Query::QueryRealmName::Read()
{
    VirtualRealmAddress = _worldPacket.read<uint32>();
}

void WorldPackets::Query::QueryCreature::Read()
{
    CreatureID = _worldPacket.read<uint32>();
}

void WorldPackets::Query::QueryNPCText::Read()
{
    TextID = _worldPacket.read<uint32>();

    ObjectGuid guid;
    guid[4] = _worldPacket.ReadBit();
    guid[5] = _worldPacket.ReadBit();
    guid[1] = _worldPacket.ReadBit();
    guid[7] = _worldPacket.ReadBit();
    guid[0] = _worldPacket.ReadBit();
    guid[2] = _worldPacket.ReadBit();
    guid[6] = _worldPacket.ReadBit();
    guid[3] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(guid[4]);
    _worldPacket.ReadByteSeq(guid[0]);
    _worldPacket.ReadByteSeq(guid[2]);
    _worldPacket.ReadByteSeq(guid[5]);
    _worldPacket.ReadByteSeq(guid[1]);
    _worldPacket.ReadByteSeq(guid[7]);
    _worldPacket.ReadByteSeq(guid[3]);
    _worldPacket.ReadByteSeq(guid[6]);
}

void WorldPackets::Query::QueryPageText::Read()
{
    PageTextID = _worldPacket.read<uint32>();

    ObjectGuid guid;
    guid[2] = _worldPacket.ReadBit();
    guid[1] = _worldPacket.ReadBit();
    guid[3] = _worldPacket.ReadBit();
    guid[7] = _worldPacket.ReadBit();
    guid[6] = _worldPacket.ReadBit();
    guid[4] = _worldPacket.ReadBit();
    guid[0] = _worldPacket.ReadBit();
    guid[5] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(guid[0]);
    _worldPacket.ReadByteSeq(guid[6]);
    _worldPacket.ReadByteSeq(guid[3]);
    _worldPacket.ReadByteSeq(guid[5]);
    _worldPacket.ReadByteSeq(guid[1]);
    _worldPacket.ReadByteSeq(guid[7]);
    _worldPacket.ReadByteSeq(guid[4]);
    _worldPacket.ReadByteSeq(guid[2]);
}

void WorldPackets::Query::QueryCorpseTransport::Read()
{
    ObjectGuid corpseGuid;
    corpseGuid[7] = _worldPacket.ReadBit();
    corpseGuid[6] = _worldPacket.ReadBit();
    corpseGuid[3] = _worldPacket.ReadBit();
    corpseGuid[0] = _worldPacket.ReadBit();
    corpseGuid[4] = _worldPacket.ReadBit();
    corpseGuid[1] = _worldPacket.ReadBit();
    corpseGuid[5] = _worldPacket.ReadBit();
    corpseGuid[2] = _worldPacket.ReadBit();
    _worldPacket.ReadByteSeq(corpseGuid[1]);
    _worldPacket.ReadByteSeq(corpseGuid[6]);
    _worldPacket.ReadByteSeq(corpseGuid[0]);
    _worldPacket.ReadByteSeq(corpseGuid[5]);
    _worldPacket.ReadByteSeq(corpseGuid[3]);
    _worldPacket.ReadByteSeq(corpseGuid[2]);
    _worldPacket.ReadByteSeq(corpseGuid[4]);
    _worldPacket.ReadByteSeq(corpseGuid[7]);
}

void WorldPackets::Query::QueryQuestCompletionNPCs::Read()
{
    for (int i = 0; i < 50; ++i)
        QuestCompletionNPCs.push_back(_worldPacket.read<uint32>());

    Count = _worldPacket.read<uint32>();
}

void WorldPackets::Query::QuestPOIQuery::Read()
{
    MissingQuestCount = _worldPacket.ReadBits(22);

    if (MissingQuestCount > MAX_QUEST_LOG_SIZE)
    {
        _worldPacket.rfinish();
        return;
    }

    for (uint32 i = 0; i < MissingQuestCount; ++i)
        MissingQuestPOIs.push_back(_worldPacket.read<uint32>());
}