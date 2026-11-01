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

#include "MailPackets.h"

void WorldPackets::Mail::MailGetList::Read()
{
    Mailbox[6] = _worldPacket.ReadBit();
    Mailbox[3] = _worldPacket.ReadBit();
    Mailbox[7] = _worldPacket.ReadBit();
    Mailbox[5] = _worldPacket.ReadBit();
    Mailbox[4] = _worldPacket.ReadBit();
    Mailbox[1] = _worldPacket.ReadBit();
    Mailbox[2] = _worldPacket.ReadBit();
    Mailbox[0] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Mailbox[7]);
    _worldPacket.ReadByteSeq(Mailbox[1]);
    _worldPacket.ReadByteSeq(Mailbox[6]);
    _worldPacket.ReadByteSeq(Mailbox[5]);
    _worldPacket.ReadByteSeq(Mailbox[4]);
    _worldPacket.ReadByteSeq(Mailbox[2]);
    _worldPacket.ReadByteSeq(Mailbox[3]);
    _worldPacket.ReadByteSeq(Mailbox[0]);
}

void WorldPackets::Mail::MailDelete::Read()
{
    _worldPacket >> MailId;
    _worldPacket >> MailTemplateId;
}

void WorldPackets::Mail::MailTakeItem::Read()
{
    _worldPacket >> MailId;
    _worldPacket >> ItemId;

    Mailbox[6] = _worldPacket.ReadBit();
    Mailbox[5] = _worldPacket.ReadBit();
    Mailbox[2] = _worldPacket.ReadBit();
    Mailbox[3] = _worldPacket.ReadBit();
    Mailbox[0] = _worldPacket.ReadBit();
    Mailbox[1] = _worldPacket.ReadBit();
    Mailbox[4] = _worldPacket.ReadBit();
    Mailbox[7] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Mailbox[0]);
    _worldPacket.ReadByteSeq(Mailbox[1]);
    _worldPacket.ReadByteSeq(Mailbox[4]);
    _worldPacket.ReadByteSeq(Mailbox[2]);
    _worldPacket.ReadByteSeq(Mailbox[5]);
    _worldPacket.ReadByteSeq(Mailbox[6]);
    _worldPacket.ReadByteSeq(Mailbox[3]);
    _worldPacket.ReadByteSeq(Mailbox[7]);
}

void WorldPackets::Mail::MailTakeMoney::Read()
{
    _worldPacket >> MailId;
    _worldPacket >> Money;

    Mailbox[7] = _worldPacket.ReadBit();
    Mailbox[6] = _worldPacket.ReadBit();
    Mailbox[3] = _worldPacket.ReadBit();
    Mailbox[2] = _worldPacket.ReadBit();
    Mailbox[4] = _worldPacket.ReadBit();
    Mailbox[5] = _worldPacket.ReadBit();
    Mailbox[0] = _worldPacket.ReadBit();
    Mailbox[1] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Mailbox[7]);
    _worldPacket.ReadByteSeq(Mailbox[1]);
    _worldPacket.ReadByteSeq(Mailbox[4]);
    _worldPacket.ReadByteSeq(Mailbox[0]);
    _worldPacket.ReadByteSeq(Mailbox[3]);
    _worldPacket.ReadByteSeq(Mailbox[2]);
    _worldPacket.ReadByteSeq(Mailbox[6]);
    _worldPacket.ReadByteSeq(Mailbox[5]);
}

void WorldPackets::Mail::MailMarkAsRead::Read()
{
    _worldPacket >> MailId;

    Mailbox[0] = _worldPacket.ReadBit();
    Mailbox[2] = _worldPacket.ReadBit();
    Mailbox[3] = _worldPacket.ReadBit();
    _worldPacket.ReadBit();
    Mailbox[4] = _worldPacket.ReadBit();
    Mailbox[6] = _worldPacket.ReadBit();
    Mailbox[7] = _worldPacket.ReadBit();
    Mailbox[1] = _worldPacket.ReadBit();
    Mailbox[5] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Mailbox[1]);
    _worldPacket.ReadByteSeq(Mailbox[7]);
    _worldPacket.ReadByteSeq(Mailbox[2]);
    _worldPacket.ReadByteSeq(Mailbox[5]);
    _worldPacket.ReadByteSeq(Mailbox[6]);
    _worldPacket.ReadByteSeq(Mailbox[3]);
    _worldPacket.ReadByteSeq(Mailbox[4]);
    _worldPacket.ReadByteSeq(Mailbox[0]);
}

void WorldPackets::Mail::MailReturnToSender::Read()
{
    _worldPacket >> MailId;

    Mailbox[2] = _worldPacket.ReadBit();
    Mailbox[0] = _worldPacket.ReadBit();
    Mailbox[4] = _worldPacket.ReadBit();
    Mailbox[6] = _worldPacket.ReadBit();
    Mailbox[3] = _worldPacket.ReadBit();
    Mailbox[1] = _worldPacket.ReadBit();
    Mailbox[7] = _worldPacket.ReadBit();
    Mailbox[5] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Mailbox[5]);
    _worldPacket.ReadByteSeq(Mailbox[6]);
    _worldPacket.ReadByteSeq(Mailbox[2]);
    _worldPacket.ReadByteSeq(Mailbox[0]);
    _worldPacket.ReadByteSeq(Mailbox[3]);
    _worldPacket.ReadByteSeq(Mailbox[1]);
    _worldPacket.ReadByteSeq(Mailbox[4]);
    _worldPacket.ReadByteSeq(Mailbox[7]);
}

void WorldPackets::Mail::MailQueryNextMailTime::Read()
{
}

void WorldPackets::Mail::SendMail::Read()
{
    _worldPacket >> Unk1;
    _worldPacket >> Unk2;
    _worldPacket >> COD;
    _worldPacket >> Money;

    Mailbox[0] = _worldPacket.ReadBit();
    Mailbox[6] = _worldPacket.ReadBit();
    Mailbox[4] = _worldPacket.ReadBit();
    Mailbox[1] = _worldPacket.ReadBit();
    uint32 bodyLength = _worldPacket.ReadBits(11);
    Mailbox[3] = _worldPacket.ReadBit();
    uint32 receiverLength = _worldPacket.ReadBits(9);
    Mailbox[7] = _worldPacket.ReadBit();
    Mailbox[5] = _worldPacket.ReadBit();
    ItemCount = _worldPacket.ReadBits(5);

    ItemGuids.resize(ItemCount);
    for (uint8 i = 0; i < ItemCount; ++i)
    {
        ItemGuids[i][1] = _worldPacket.ReadBit();
        ItemGuids[i][7] = _worldPacket.ReadBit();
        ItemGuids[i][2] = _worldPacket.ReadBit();
        ItemGuids[i][5] = _worldPacket.ReadBit();
        ItemGuids[i][0] = _worldPacket.ReadBit();
        ItemGuids[i][6] = _worldPacket.ReadBit();
        ItemGuids[i][3] = _worldPacket.ReadBit();
        ItemGuids[i][4] = _worldPacket.ReadBit();
    }

    uint32 subjectLength = _worldPacket.ReadBits(9);
    Mailbox[2] = _worldPacket.ReadBit();

    for (uint8 i = 0; i < ItemCount; ++i)
    {
        _worldPacket.read_skip<uint8>();
        _worldPacket.ReadByteSeq(ItemGuids[i][3]);
        _worldPacket.ReadByteSeq(ItemGuids[i][0]);
        _worldPacket.ReadByteSeq(ItemGuids[i][2]);
        _worldPacket.ReadByteSeq(ItemGuids[i][1]);
        _worldPacket.ReadByteSeq(ItemGuids[i][6]);
        _worldPacket.ReadByteSeq(ItemGuids[i][5]);
        _worldPacket.ReadByteSeq(ItemGuids[i][7]);
        _worldPacket.ReadByteSeq(ItemGuids[i][4]);
    }

    _worldPacket.ReadByteSeq(Mailbox[1]);
    Body = _worldPacket.ReadString(bodyLength);
    _worldPacket.ReadByteSeq(Mailbox[0]);
    Subject = _worldPacket.ReadString(subjectLength);
    _worldPacket.ReadByteSeq(Mailbox[2]);
    _worldPacket.ReadByteSeq(Mailbox[6]);
    _worldPacket.ReadByteSeq(Mailbox[5]);
    _worldPacket.ReadByteSeq(Mailbox[7]);
    _worldPacket.ReadByteSeq(Mailbox[3]);
    _worldPacket.ReadByteSeq(Mailbox[4]);
    ReceiverName = _worldPacket.ReadString(receiverLength);
}

void WorldPackets::Mail::MailCreateTextItem::Read()
{
    _worldPacket >> MailId;

    Mailbox[4] = _worldPacket.ReadBit();
    Mailbox[1] = _worldPacket.ReadBit();
    Mailbox[6] = _worldPacket.ReadBit();
    Mailbox[2] = _worldPacket.ReadBit();
    Mailbox[5] = _worldPacket.ReadBit();
    Mailbox[3] = _worldPacket.ReadBit();
    Mailbox[0] = _worldPacket.ReadBit();
    Mailbox[7] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Mailbox[6]);
    _worldPacket.ReadByteSeq(Mailbox[5]);
    _worldPacket.ReadByteSeq(Mailbox[4]);
    _worldPacket.ReadByteSeq(Mailbox[3]);
    _worldPacket.ReadByteSeq(Mailbox[0]);
    _worldPacket.ReadByteSeq(Mailbox[7]);
    _worldPacket.ReadByteSeq(Mailbox[2]);
    _worldPacket.ReadByteSeq(Mailbox[1]);
}
