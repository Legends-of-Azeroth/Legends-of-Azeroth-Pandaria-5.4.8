/*
* This file is part of the Pandaria 5.4.8 Project. See THANKS file for Copyright information
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

#ifndef TRINITYCORE_WORLDPACKET_H
#define TRINITYCORE_WORLDPACKET_H

#include "Common.h"
#include "Opcodes.h"
#include "ByteBuffer.h"
#include "Duration.h"
#include <chrono>
#include "MessageBuffer.h"

class WorldPacket : public ByteBuffer
{
    public:
        // just container for later use
        WorldPacket() : ByteBuffer(0, Reserve{}), m_opcode(UNKNOWN_OPCODE)
        {
        }

        WorldPacket(uint32 opcode) : ByteBuffer(0, Reserve{}), m_opcode(opcode) { }

        WorldPacket(uint32 opcode, size_t res, Reserve) : ByteBuffer(res, Reserve{}), m_opcode(opcode) { }

        WorldPacket(uint32 opcode, size_t res, Resize) : ByteBuffer(res, Resize{}), m_opcode(opcode) { }

        WorldPacket(uint32 opcode, size_t res) : WorldPacket(opcode, res, Reserve{}) { }

        WorldPacket(WorldPacket&& packet) noexcept : ByteBuffer(std::move(packet)), m_opcode(packet.m_opcode), m_receivedTime(packet.m_receivedTime)
        {
        }

        WorldPacket(WorldPacket const& right) = default;

        WorldPacket& operator=(WorldPacket const& right)
        {
            if (this != &right)
            {
                m_opcode = right.m_opcode;
                ByteBuffer::operator =(right);
            }

            return *this;
        }

        WorldPacket& operator=(WorldPacket&& right) noexcept
        {
            if (this != &right)
            {
                m_opcode = right.m_opcode;
                ByteBuffer::operator=(std::move(right));
            }

            return *this;
        }

        WorldPacket(uint32 opcode, MessageBuffer&& buffer) : ByteBuffer(std::move(buffer)), m_opcode(opcode) { }

        void Initialize(uint32 opcode, size_t newres = 200)
        {
            clear();
            _storage.reserve(newres);
            m_opcode = opcode;
        }

        uint32 GetOpcode() const { return m_opcode; }
        void SetOpcode(uint32 opcode) { m_opcode = opcode; }

        TimePoint GetReceivedTime() const { return m_receivedTime; }
        void SetReceiveTime(TimePoint receivedTime) { m_receivedTime = receivedTime; }

    protected:
        uint32 m_opcode;
        TimePoint m_receivedTime; // only set for a specific set of opcodes, for performance reasons.
};

#endif
