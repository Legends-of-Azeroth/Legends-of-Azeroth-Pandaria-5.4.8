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

#ifndef VIGNETTE_H
#define VIGNETTE_H

#include "Common.h"
#include "Object.h"
#include "DB2Stores.h"
#include "GameObject.h"

namespace Vignette
{
    // This enum is internal only (not sended or related to client-side)
    // It's represent type of vignette to know how we must handle them
    enum class Type : uint8
    {
        SourceCreature,           // Used for vignette linked to creature, player show the vignette if the creature is in the view-range
        SourceGameObject,         // Used for vignette linked to gameobject, player show the vignette if the gameobject is in the view-range
        SourceRare,               // Used for vignette linked to rare creature, player show the vignette if the creature is in the view-range and if the player doesn't have the tracking quest
        SourceTreasure,           // Used for vignette linked to treasure gameobject, player show the vignette if the treasure is in the view-range and if the player doesn't have the tracking quest
        SourceScript              // Used for script, that kind of vignette are never remove automaticaly
    };

    /************************************************************************/
    /*                         HELPERS                                      */
    /************************************************************************/

    /**
    * Return VignetteEntry (db2 entry) from WorldObject if available
    * Work only for Creature and Gameobject
    * @param target : WorldObject for which we wanna get the vignette entry
    */
    inline VignetteEntry const* GetVignetteEntryFromWorldObject(WorldObject const* target) // Unused function 'GetVignetteEntryFromWorldObject'
    {
        uint32 vignetteId = 0;

        if (target->GetTypeId() == TYPEID_UNIT)
            vignetteId = target->ToCreature()->GetCreatureTemplate()->VignetteID;
        if (target->GetTypeId() == TYPEID_GAMEOBJECT)
            vignetteId = target->ToGameObject()->GetGOInfo()->GetVignetteId();

        if (vignetteId == 0)
            return nullptr;

        return sVignetteStore.LookupEntry(vignetteId);
    }

    /**
    * Return TrackingQuest (db entry) from WorldObject if available
    * Work only for Creature and Gameobject
    * @param target : WorldObject for which we wanna get the tracking quest
    */
    inline uint32 GetTrackingQuestIdFromWorldObject(WorldObject const* target) // Unused function 'GetTrackingQuestIdFromWorldObject'
    {
        uint32 trackingQuest = 0;

        if (target->GetTypeId() == TYPEID_UNIT)
            trackingQuest = target->ToCreature()->GetCreatureTemplate()->TrackingQuestID;
        if (target->GetTypeId() == TYPEID_GAMEOBJECT)
            trackingQuest = target->ToGameObject()->GetGOInfo()->GetTrackingQuestId();

        return trackingQuest;
    }

    /**
    * Return default vignette type (@see enum Type) from WorldObject
    * Work only for Creature and Gameobject
    * @param target : WorldObject for which we wanna get the default vignette type
    */
    inline Type GetDefaultVignetteTypeFromWorldObject(WorldObject const* target) // Unused function 'GetDefaultVignetteTypeFromWorldObject'
    {
        switch (target->GetTypeId())
        {
            case TYPEID_UNIT:
                return Type::SourceCreature;
            case TYPEID_GAMEOBJECT:
                return Type::SourceGameObject;
            // Can't happen
            default:
                return Type::SourceScript;
        }
    }

    /**
    * Return tracking vignette type (@see enum Type) from WorldObject
    * Work only for Creature and Gameobject
    * @param target : WorldObject for which we wanna get the tracking vignette type
    */
    inline Type GetTrackingVignetteTypeFromWorldObject(WorldObject const* target) // Unused function  GetTrackingVignetteTypeFromWorldObject
    {
        switch (target->GetTypeId())
        {
            case TYPEID_UNIT:
                return Type::SourceRare;
            case TYPEID_GAMEOBJECT:
                return Type::SourceTreasure;
            // Can't happen
            default:
                return Type::SourceScript;
        }
    }

    // Entity is the class which represent the vignette
    class Entity
    {
        friend class Manager;

        public:
            /**
            * Update the position of the vignette
            * @param newPosition : New position of the vignette
            */
            void UpdatePosition(G3D::Vector3 newPosition);

            /**
            * Reset the need of client update
            */
            void ResetNeedClientUpdate() { m_NeedClientUpdate = false; }

            /**
            * Get the guid of the vignette
            * @return guid : New position of the vignette
            */
            ObjectGuid GetGuid() const { return m_Guid; }

            /**
            * Get the source guid of the vignette, can be 0
            * @return source guid
            */
            ObjectGuid GeSourceGuid() const { return m_SourceGuid; }

            /**
            * Get the vignette type
            * @return Type @see Type
            */
            Type GetVignetteType() const { return m_Type; }

            /**
            * Check if the vignette need to be update client-side
            * @return result as bool
            */
            bool NeedClientUpdate() const { return m_NeedClientUpdate; }

            /**
            * Get the current position of the vignette
            * @return Vector3
            */
            G3D::Vector3 const& GetPosition() const { return m_Position; }

            /**
            * Get the Vignette Entry (Vignette.db2)
            * @return VignetteEntry
            */
            VignetteEntry const* GetVignetteEntry() const { return m_VignetteEntry; }

        private:
            /**
            * Vignette Entity constructor
            * @param vignetteEntry : VignetteEntry, refer to Vignette.db2
            * @param mapId : ID of the map where the vignette are, refer to Map.dbc
            */
            Entity(VignetteEntry const* vignetteEntry, uint32 const mapId);

            /*
            * Vignette Entity destructor
            */
            ~Entity();

            /**
            * Create the vignette
            * @param type : Type of the vignette, @see enum Type
            * @param position: World position of the vignette
            * @param sourceGuid: Guid of the source (can be gameobject or creature)
            */
            void Create(Map* map, Type type, G3D::Vector3 position, ObjectGuid sourceGuid);

            ObjectGuid           m_Guid;                    // Guid of the current vignette (GUID_TYPE_VIGNETTE)
            ObjectGuid           m_SourceGuid;              // Guid of the source if any (can be gameobject or creature)
            uint32 const         m_Map;                     // Map Id of the current vignette (we only send vignette which are on the same map the player is)
            VignetteEntry const* m_VignetteEntry;           // Vignette data (db2)
            G3D::Vector3         m_Position;                // World coordinates of the current vignette, client compute 2D minimap coord itself
            Type                 m_Type;                    // Type of the vignette, @see enum Type

        protected:
            bool                m_NeedClientUpdate;         // If true, the vignette need to be updated client-side
    };
}
#endif
