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

#include "QuestPackets.h"

WorldPacket const* WorldPackets::Quest::QuestGiverStatusMultiple::Write()
{
    _worldPacket.WriteBits(QuestGiver.size(), 21);

    for (QuestGiverInfo const& questGiver : QuestGiver)
    {
        ObjectGuid guid = questGiver.Guid;

        _worldPacket.WriteBit(guid[4]);
        _worldPacket.WriteBit(guid[0]);
        _worldPacket.WriteBit(guid[3]);
        _worldPacket.WriteBit(guid[6]);
        _worldPacket.WriteBit(guid[5]);
        _worldPacket.WriteBit(guid[7]);
        _worldPacket.WriteBit(guid[1]);
        _worldPacket.WriteBit(guid[2]);
    }

    for (QuestGiverInfo const& questGiver : QuestGiver)
    {
        ObjectGuid guid = questGiver.Guid;

        _worldPacket.WriteByteSeq(guid[6]);
        _worldPacket.WriteByteSeq(guid[2]);
        _worldPacket.WriteByteSeq(guid[7]);
        _worldPacket.WriteByteSeq(guid[5]);
        _worldPacket.WriteByteSeq(guid[4]);
        _worldPacket << uint32(questGiver.Status);
        _worldPacket.WriteByteSeq(guid[1]);
        _worldPacket.WriteByteSeq(guid[3]);
        _worldPacket.WriteByteSeq(guid[0]);
    }

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Quest::QuestUpdateComplete::Write()
{
    _worldPacket << uint32(QuestID);

    return &_worldPacket;
}

void WorldPackets::Quest::QuestGiverStatusQuery::Read()
{
    Guid[4] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[3]);
}

void WorldPackets::Quest::QuestGiverHello::Read()
{
    Guid[5] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[2]);
}

void WorldPackets::Quest::QuestGiverAcceptQuest::Read()
{
    QuestId = _worldPacket.read<uint32>();
    Guid[6] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    StartCheat = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[7]);
}

void WorldPackets::Quest::QuestGiverQueryQuest::Read()
{
    _worldPacket >> QuestId;
    Guid[2] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    _worldPacket.ReadBit(); // unused bit

    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[6]);
}

void WorldPackets::Quest::QueryQuestInfo::Read()
{
    _worldPacket >> QuestId;
    Guid[0] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[0]);
}

void WorldPackets::Quest::QuestGiverChooseReward::Read()
{
    _worldPacket >> Reward;
    _worldPacket >> QuestId;
    Guid[2] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[4]);
}

void WorldPackets::Quest::QuestGiverRequestReward::Read()
{
    _worldPacket >> QuestId;
    Guid[6] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[4]);
}

void WorldPackets::Quest::QuestLogRemoveQuest::Read()
{
    _worldPacket >> Slot;
}

void WorldPackets::Quest::QuestConfirmAccept::Read()
{
    _worldPacket >> QuestId;
}

void WorldPackets::Quest::QuestGiverCompleteQuest::Read()
{
    _worldPacket >> QuestId;
    PlayerGuid[4] = _worldPacket.ReadBit();
    PlayerGuid[2] = _worldPacket.ReadBit();
    PlayerGuid[1] = _worldPacket.ReadBit();
    PlayerGuid[5] = _worldPacket.ReadBit();
    PlayerGuid[6] = _worldPacket.ReadBit();
    PlayerGuid[7] = _worldPacket.ReadBit();
    PlayerGuid[3] = _worldPacket.ReadBit();
    AutoCompleteMode = _worldPacket.ReadBit();
    PlayerGuid[0] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(PlayerGuid[0]);
    _worldPacket.ReadByteSeq(PlayerGuid[2]);
    _worldPacket.ReadByteSeq(PlayerGuid[1]);
    _worldPacket.ReadByteSeq(PlayerGuid[4]);
    _worldPacket.ReadByteSeq(PlayerGuid[3]);
    _worldPacket.ReadByteSeq(PlayerGuid[6]);
    _worldPacket.ReadByteSeq(PlayerGuid[7]);
    _worldPacket.ReadByteSeq(PlayerGuid[5]);
}

void WorldPackets::Quest::PushQuestToParty::Read()
{
    QuestId = _worldPacket.read<uint32>();
}

void WorldPackets::Quest::QuestPushResult::Read()
{
    QuestId = _worldPacket.read<uint32>();
    Msg = _worldPacket.read<uint8>();
    Guid[5] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[3]);
}