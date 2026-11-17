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

#include "Vignette.h"
#include "ObjectMgr.h"

namespace Vignette
{
    Entity::Entity(VignetteEntry const* vignetteEntry, uint32 const mapId) : m_Map(mapId), m_VignetteEntry(vignetteEntry)
    {
        ASSERT(vignetteEntry);

        m_Guid             = ObjectGuid::Empty;
        m_NeedClientUpdate = false;
    }

    Entity::~Entity() { }

    void Entity::Create(Map* map, Type type, G3D::Vector3 position, ObjectGuid sourceGuid)
    {
        m_Guid       = ObjectGuid(HighGuid::Vignette, m_VignetteEntry->Id, map->GenerateLowGuid<HighGuid::Vignette>());
        m_Type       = type;
        m_Position   = position;
        m_SourceGuid = sourceGuid;
    }

    void Entity::UpdatePosition(G3D::Vector3 newPosition)
    {
        if ((int32)m_Position.x == (int32)newPosition.x && (int32)m_Position.y == (int32)newPosition.y)
            return;

        m_Position         = newPosition;
        m_NeedClientUpdate = true;
    }
}
