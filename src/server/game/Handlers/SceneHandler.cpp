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

#include "Common.h"
#include "WorldSession.h"
#include "Player.h"

void WorldSession::HandleSceneTriggerEvent(WorldPackets::Scene::SceneTriggerEvent& packet)
{
    std::string event = packet.Event;
    uint32 sceneInstanceID = packet.SceneInstanceID;
    GetPlayer()->GetSceneMgr().OnSceneTrigger(sceneInstanceID, event);
}

void WorldSession::HandleScenePlaybackComplete(WorldPackets::Scene::ScenePlaybackComplete& packet)
{
    uint32 sceneInstanceID = packet.SceneInstanceID;
    GetPlayer()->GetSceneMgr().OnSceneComplete(sceneInstanceID);
}

void WorldSession::HandleScenePlaybackCanceled(WorldPackets::Scene::ScenePlaybackCanceled& packet)
{
    uint32 sceneInstanceID = packet.SceneInstanceID;
    GetPlayer()->GetSceneMgr().OnSceneCancel(sceneInstanceID);
}
