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

WorldPacket const* WorldPackets::Misc::BinderConfirm::Write()
{
    _worldPacket.WriteBit(Unit[4]);
    _worldPacket.WriteBit(Unit[6]);
    _worldPacket.WriteBit(Unit[2]);
    _worldPacket.WriteBit(Unit[1]);
    _worldPacket.WriteBit(Unit[5]);
    _worldPacket.WriteBit(Unit[3]);
    _worldPacket.WriteBit(Unit[0]);
    _worldPacket.WriteBit(Unit[7]);

    _worldPacket.WriteByteSeq(Unit[6]);
    _worldPacket.WriteByteSeq(Unit[2]);
    _worldPacket.WriteByteSeq(Unit[5]);
    _worldPacket.WriteByteSeq(Unit[0]);
    _worldPacket.WriteByteSeq(Unit[4]);
    _worldPacket.WriteByteSeq(Unit[7]);
    _worldPacket.WriteByteSeq(Unit[1]);
    _worldPacket.WriteByteSeq(Unit[3]);
    return &_worldPacket;
}

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

WorldPacket const* WorldPackets::Misc::UITime::Write()
{
    _worldPacket << uint32(Time);

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

WorldPacket const* WorldPackets::Misc::PlayMusic::Write()
{
    _worldPacket << SoundKitID;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::PlayObjectSound::Write()
{
    _worldPacket.WriteBit(SourceObjectGUID[5]);
    _worldPacket.WriteBit(TargetObjectGUID[7]);
    _worldPacket.WriteBit(TargetObjectGUID[0]);
    _worldPacket.WriteBit(TargetObjectGUID[3]);
    _worldPacket.WriteBit(SourceObjectGUID[1]);
    _worldPacket.WriteBit(TargetObjectGUID[4]);
    _worldPacket.WriteBit(SourceObjectGUID[7]);
    _worldPacket.WriteBit(SourceObjectGUID[2]);
    _worldPacket.WriteBit(SourceObjectGUID[4]);
    _worldPacket.WriteBit(SourceObjectGUID[3]);
    _worldPacket.WriteBit(TargetObjectGUID[5]);
    _worldPacket.WriteBit(TargetObjectGUID[1]);
    _worldPacket.WriteBit(TargetObjectGUID[6]);
    _worldPacket.WriteBit(TargetObjectGUID[2]);
    _worldPacket.WriteBit(SourceObjectGUID[6]);
    _worldPacket.WriteBit(SourceObjectGUID[0]);

    _worldPacket.WriteByteSeq(TargetObjectGUID[6]);
    _worldPacket.WriteByteSeq(TargetObjectGUID[2]);
    _worldPacket.WriteByteSeq(SourceObjectGUID[2]);
    _worldPacket.WriteByteSeq(SourceObjectGUID[5]);
    _worldPacket.WriteByteSeq(TargetObjectGUID[7]);
    _worldPacket.WriteByteSeq(TargetObjectGUID[5]);
    _worldPacket.WriteByteSeq(TargetObjectGUID[3]);
    _worldPacket.WriteByteSeq(TargetObjectGUID[1]);
    _worldPacket.WriteByteSeq(SourceObjectGUID[3]);
    _worldPacket.WriteByteSeq(SourceObjectGUID[1]);
    _worldPacket << SoundKitID;
    _worldPacket.WriteByteSeq(TargetObjectGUID[4]);
    _worldPacket.WriteByteSeq(SourceObjectGUID[4]);
    _worldPacket.WriteByteSeq(SourceObjectGUID[7]);
    _worldPacket.WriteByteSeq(SourceObjectGUID[0]);
    _worldPacket.WriteByteSeq(SourceObjectGUID[6]);
    _worldPacket.WriteByteSeq(TargetObjectGUID[0]);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::PlaySound::Write()
{
    _worldPacket.WriteBit(SourceObjectGUID[2]);
    _worldPacket.WriteBit(SourceObjectGUID[3]);
    _worldPacket.WriteBit(SourceObjectGUID[7]);
    _worldPacket.WriteBit(SourceObjectGUID[6]);
    _worldPacket.WriteBit(SourceObjectGUID[0]);
    _worldPacket.WriteBit(SourceObjectGUID[5]);
    _worldPacket.WriteBit(SourceObjectGUID[4]);
    _worldPacket.WriteBit(SourceObjectGUID[1]);
    _worldPacket << SoundKitID;
    _worldPacket.WriteByteSeq(SourceObjectGUID[3]);
    _worldPacket.WriteByteSeq(SourceObjectGUID[2]);
    _worldPacket.WriteByteSeq(SourceObjectGUID[4]);
    _worldPacket.WriteByteSeq(SourceObjectGUID[7]);
    _worldPacket.WriteByteSeq(SourceObjectGUID[5]);
    _worldPacket.WriteByteSeq(SourceObjectGUID[0]);
    _worldPacket.WriteByteSeq(SourceObjectGUID[6]);
    _worldPacket.WriteByteSeq(SourceObjectGUID[1]);

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

WorldPacket const* WorldPackets::Misc::BindPointUpdate::Write()
{
    _worldPacket << BindPosition.GetPositionX();
    _worldPacket << BindPosition.GetPositionY();
    _worldPacket << BindPosition.GetPositionZ();
    _worldPacket << uint32(BindAreaID);    
    _worldPacket << uint32(BindMapID);

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

void WorldPackets::Misc::SetPvP::Read()
{
    EnablePVP = _worldPacket.ReadBit();
}

void WorldPackets::Misc::TogglePvP::Read()
{
    if (HasPvPStatus())
        Enable = _worldPacket.read<uint8>() != 0;
}

WorldPacket const* WorldPackets::Misc::OverrideLight::Write()
{
    _worldPacket << int32(AreaLightID);
    _worldPacket << int32(OverrideLightID);
    _worldPacket << int32(TransitionMilliseconds);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Misc::DisplayGameError::Write()
{
    _worldPacket << uint32(Error);
    _worldPacket.WriteBit(Arg.has_value());
    _worldPacket.WriteBit(Arg2.has_value());
    _worldPacket.FlushBits();

    if (Arg)
        _worldPacket << int32(*Arg);

    if (Arg2)
        _worldPacket << int32(*Arg2);

    return &_worldPacket;
}