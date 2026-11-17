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

#ifndef MailPackets_h__
#define MailPackets_h__

#include "Packet.h"

#include <string>
#include <vector>

namespace WorldPackets
{
    namespace Mail
    {
        class MailGetList final : public ClientPacket
        {
        public:
            MailGetList(WorldPacket&& packet) : ClientPacket(CMSG_MAIL_GET_LIST, std::move(packet)) { }

            void Read() override;

            ObjectGuid Mailbox;
        };

        class MailDelete final : public ClientPacket
        {
        public:
            MailDelete(WorldPacket&& packet) : ClientPacket(CMSG_MAIL_DELETE, std::move(packet)) { }

            void Read() override;

            uint32 MailId = 0;
            uint32 MailTemplateId = 0;
        };

        class MailTakeItem final : public ClientPacket
        {
        public:
            MailTakeItem(WorldPacket&& packet) : ClientPacket(CMSG_MAIL_TAKE_ITEM, std::move(packet)) { }

            void Read() override;

            uint32 MailId = 0;
            uint32 ItemId = 0;
            ObjectGuid Mailbox;
        };

        class MailTakeMoney final : public ClientPacket
        {
        public:
            MailTakeMoney(WorldPacket&& packet) : ClientPacket(CMSG_MAIL_TAKE_MONEY, std::move(packet)) { }

            void Read() override;

            uint32 MailId = 0;
            uint64 Money = 0;
            ObjectGuid Mailbox;
        };

        class MailMarkAsRead final : public ClientPacket
        {
        public:
            MailMarkAsRead(WorldPacket&& packet) : ClientPacket(CMSG_MAIL_MARK_AS_READ, std::move(packet)) { }

            void Read() override;

            uint32 MailId = 0;
            ObjectGuid Mailbox;
        };

        class MailReturnToSender final : public ClientPacket
        {
        public:
            MailReturnToSender(WorldPacket&& packet) : ClientPacket(CMSG_MAIL_RETURN_TO_SENDER, std::move(packet)) { }

            void Read() override;

            uint32 MailId = 0;
            ObjectGuid Mailbox;
        };

        class MailQueryNextMailTime final : public ClientPacket
        {
        public:
            MailQueryNextMailTime(WorldPacket&& packet) : ClientPacket(CMSG_QUERY_NEXT_MAIL_TIME, std::move(packet)) { }

            void Read() override;
        };

        class SendMail final : public ClientPacket
        {
        public:
            SendMail(WorldPacket&& packet) : ClientPacket(CMSG_SEND_MAIL, std::move(packet)) { }

            void Read() override;

            uint32 Unk1 = 0;
            uint32 Unk2 = 0;
            uint64 COD = 0;
            uint64 Money = 0;
            ObjectGuid Mailbox;
            uint8 ItemCount = 0;
            std::vector<ObjectGuid> ItemGuids;
            std::string Body;
            std::string Subject;
            std::string ReceiverName;
        };

        class MailCreateTextItem final : public ClientPacket
        {
        public:
            MailCreateTextItem(WorldPacket&& packet) : ClientPacket(CMSG_MAIL_CREATE_TEXT_ITEM, std::move(packet)) { }

            void Read() override;

            uint32 MailId = 0;
            ObjectGuid Mailbox;
        };
    }
}

#endif // MailPackets_h__
