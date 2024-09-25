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

#include "MiscPackets.h"

WorldPacket const* WorldPackets::Misc::TriggerCinematic::Write()
{
    _worldPacket << uint32(CinematicID);

    return &_worldPacket;
}
WorldPacket const* WorldPackets::Misc::TriggerMovie::Write()
{
    _worldPacket << uint32(MovieID);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::StreamingMovies::Write()
{
    _worldPacket.WriteBits(MovieIDs.size(), 25);
    _worldPacket.FlushBits();

    for (uint16 moveId : MovieIDs)
        _worldPacket << int16(moveId);

    return &_worldPacket;
}

WorldPackets::Misc::Weather::Weather() : ServerPacket(SMSG_WEATHER, 4 + 4 + 1) { }

WorldPackets::Misc::Weather::Weather(WeatherState weatherID, float intensity /*= 0.0f*/, bool abrupt /*= false*/)
    : ServerPacket(SMSG_WEATHER, 4 + 4 + 1), Abrupt(abrupt), Intensity(intensity), WeatherID(weatherID) { }

WorldPacket const* WorldPackets::Misc::Weather::Write()
{
    _worldPacket << uint32(WeatherID);
    _worldPacket << float(Intensity);
    _worldPacket << uint8(Abrupt);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::StartMirrorTimer::Write()
{
    _worldPacket << MaxValue;
    _worldPacket << uint32(SpellID); // spell id
    _worldPacket << uint32(Timer);
    _worldPacket << Scale;
    _worldPacket << Value;
    _worldPacket << uint8(Paused);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::StopMirrorTimer::Write()
{
    _worldPacket << uint32(Timer);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::Dismount::Write()
{
    _worldPacket.WriteBit(Guid[6]);
    _worldPacket.WriteBit(Guid[3]);
    _worldPacket.WriteBit(Guid[0]);
    _worldPacket.WriteBit(Guid[7]);
    _worldPacket.WriteBit(Guid[1]);
    _worldPacket.WriteBit(Guid[2]);
    _worldPacket.WriteBit(Guid[5]);
    _worldPacket.WriteBit(Guid[4]);

    _worldPacket.WriteByteSeq(Guid[3]);
    _worldPacket.WriteByteSeq(Guid[6]);
    _worldPacket.WriteByteSeq(Guid[7]);
    _worldPacket.WriteByteSeq(Guid[5]);
    _worldPacket.WriteByteSeq(Guid[1]);
    _worldPacket.WriteByteSeq(Guid[4]);
    _worldPacket.WriteByteSeq(Guid[2]);
    _worldPacket.WriteByteSeq(Guid[0]);

    return &_worldPacket;
}


WorldPacket const* WorldPackets::Misc::CorpseReclaimDelay::Write()
{
    _worldPacket.WriteBit(Remaining == 0);

    if (Remaining)
        _worldPacket << uint32(Remaining);

    return &_worldPacket;
}

void WorldPackets::Misc::FarSight::Read()
{
    Enable = _worldPacket.ReadBit();
}