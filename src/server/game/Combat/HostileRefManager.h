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

#ifndef SF_HOSTILEREFMANAGER
#define SF_HOSTILEREFMANAGER

#include "Common.h"

class Unit;
class SpellInfo;

/* Legacy compatibility adapter for the TC 3.3.5 style threat system migration.
   It does not retain a list of references anymore; every operation delegates
   to the new ThreatManager/ThreatReference implementation. */
class HostileRefManager
{
    private:
        Unit* iOwner;
    public:
        explicit HostileRefManager(Unit* owner) { iOwner = owner; }
        ~HostileRefManager();

        Unit* GetOwner() { return iOwner; }

        // send threat to all my hateres for the victim
        // The victim is hated than by them as well
        // use for buffs and healing threat functionality
        void threatAssist(Unit* victim, float baseThreat, SpellInfo const* threatSpell = NULL);

        void addTempThreat(float threat, bool apply);

        void addThreatPercent(int32 percent);

        // The references are not needed anymore
        // tell the source to remove them from the list and free the mem
        void deleteReferences();

        // Remove specific faction references
        void deleteReferencesForFaction(uint32 faction);

        void updateThreatTables();

        void setOnlineOfflineState(bool isOnline);

        // set state for one reference, defined by Unit
        void setOnlineOfflineState(Unit* creature, bool isOnline);

        // delete one reference, defined by Unit
        void deleteReference(Unit* creature);

        void UpdateVisibility();
};
//=================================================
#endif