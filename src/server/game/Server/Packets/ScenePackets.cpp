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

#include "ScenePackets.h"

void WorldPackets::Scene::SceneTriggerEvent::Read()
{
    _worldPacket.ReadBit();
    uint32 len = _worldPacket.ReadBits(6);
    Event = _worldPacket.ReadString(len);
    SceneInstanceID = _worldPacket.read<uint32>();
}

void WorldPackets::Scene::ScenePlaybackComplete::Read()
{
    SceneInstanceID = 0;
    if (!_worldPacket.ReadBit())
        _worldPacket >> SceneInstanceID;
}

void WorldPackets::Scene::ScenePlaybackCanceled::Read()
{
    SceneInstanceID = 0;
    if (!_worldPacket.ReadBit())
        _worldPacket >> SceneInstanceID;
}

void WorldPackets::Scene::QueryScenarioPOI::Read()
{
    uint32 count = _worldPacket.ReadBits(22);
    CriteriaTreeIds.resize(count);
    for (uint32 i = 0; i < count; ++i)
        CriteriaTreeIds[i] = _worldPacket.read<uint32>();
}
