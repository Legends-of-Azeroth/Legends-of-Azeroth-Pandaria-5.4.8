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

#ifndef ZONE_SCRIPT_H_
#define ZONE_SCRIPT_H_

#include "Common.h"
#include "Creature.h"

class GameObject;

class ZoneScript
{
    public:
        ZoneScript() { }
        virtual ~ZoneScript() { }

        // Interface for the direct inheritance
        virtual void GameObjectRemoved(GameObject* go) { OnGameObjectRemove(go); }

        virtual uint32 GetCreatureEntry(uint32 /*guidlow*/, CreatureData const* data) { return data->id; }
        virtual uint32 GetGameObjectEntry(uint32 /*guidlow*/, uint32 entry) { return entry; }

        virtual void OnCreatureCreate(Creature* ) { }
        virtual void OnCreatureRemove(Creature* ) { }

        virtual void OnGameObjectCreate(GameObject* ) { }
        virtual void OnGameObjectRemove(GameObject* ) { }

        virtual void OnUnitDeath(Unit*) { }

        //All-purpose data storage 64 bit
        virtual ObjectGuid GetGuidData(uint32 /*DataId*/) const { return ObjectGuid::Empty; }
        virtual void SetGuidData(uint32 /*DataId*/, ObjectGuid /*Value*/) { }

        virtual uint64 GetData64(uint32 /*DataId*/) const { return 0; }
        virtual void SetData64(uint32 /*DataId*/, uint64 /*Value*/) { }

        //All-purpose data storage 32 bit
        virtual uint32 GetData(uint32 /*DataId*/) const { return 0; }
        virtual void SetData(uint32 /*DataId*/, uint32 /*Value*/) { }

        virtual void ProcessEvent(WorldObject* /*obj*/, uint32 /*eventId*/) { }
};

#endif
