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

#ifndef ClientConfigPackets_h__
#define ClientConfigPackets_h__

#include "Packet.h"
#include <string>

namespace WorldPackets
{
    namespace ClientConfig
    {
        class RequestAccountData final : public ClientPacket
        {
        public:
            explicit RequestAccountData(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_ACCOUNT_DATA, std::move(packet)) { }

            void Read() override;

            int32 DataType = 0; ///< @see enum AccountDataType
        };

        class UserClientUpdateAccountData final : public ClientPacket
        {
        public:
            explicit UserClientUpdateAccountData(WorldPacket&& packet) : ClientPacket(CMSG_UPDATE_ACCOUNT_DATA, std::move(packet)) { }

            void Read() override;

            uint32 Size = 0;
            uint32 Time = 0;
            uint32 CompressedSize = 0;
            uint32 DataType = 0; ///< @see enum AccountDataType
            std::string CompressedData;
        };
    }
}

#endif // ClientConfigPackets_h__
