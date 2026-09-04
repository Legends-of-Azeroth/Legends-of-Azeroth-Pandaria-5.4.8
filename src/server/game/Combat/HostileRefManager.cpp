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

#include "HostileRefManager.h"
#include "ThreatManager.h"
#include "Unit.h"

HostileRefManager::~HostileRefManager()
{
}

//=================================================
// send threat to all my hateres for the victim
// The victim is hated than by them as well
// use for buffs and healing threat functionality

void HostileRefManager::threatAssist(Unit* victim, float baseThreat, SpellInfo const* threatSpell)
{
    iOwner->GetThreatManager().ForwardThreatForAssistingMe(victim, baseThreat, threatSpell);
}

//=================================================

void HostileRefManager::addTempThreat(float /*threat*/, bool /*apply*/)
{
    iOwner->GetThreatManager().UpdateMyTempModifiers();
}

//=================================================

void HostileRefManager::addThreatPercent(int32 percent)
{
    for (auto const& pair : iOwner->GetThreatManager().GetThreatenedByMeList())
        pair.second->ModifyThreatByPercent(percent);
}

//=================================================
// The online / offline status is given to the method. The calculation has to be done before

void HostileRefManager::setOnlineOfflineState(bool isOnline)
{
    if (isOnline)
        iOwner->GetThreatManager().EvaluateSuppressed(true);
}

//=================================================
// The online / offline status is calculated and set

void HostileRefManager::updateThreatTables()
{
    iOwner->GetThreatManager().EvaluateSuppressed(true);
}

//=================================================
// The references are not needed anymore
// tell the source to remove them from the list and free the mem

void HostileRefManager::deleteReferences()
{
    auto threats = iOwner->GetThreatManager().GetThreatenedByMeList();
    for (auto const& pair : threats)
        pair.second->ClearThreat();
}

//=================================================
// delete one reference, defined by faction

void HostileRefManager::deleteReferencesForFaction(uint32 faction)
{
    auto threats = iOwner->GetThreatManager().GetThreatenedByMeList();
    for (auto const& pair : threats)
        if (pair.second->GetOwner()->GetFactionTemplateEntry() &&
            pair.second->GetOwner()->GetFactionTemplateEntry()->faction == faction)
            pair.second->ClearThreat();
}

//=================================================
// delete one reference, defined by Unit

void HostileRefManager::deleteReference(Unit* creature)
{
    if (creature)
        creature->GetThreatManager().ClearThreat(iOwner);
}

//=================================================
// set state for one reference, defined by Unit

void HostileRefManager::setOnlineOfflineState(Unit* /*creature*/, bool isOnline)
{
    if (isOnline)
        iOwner->GetThreatManager().EvaluateSuppressed(true);
}

//=================================================

void HostileRefManager::UpdateVisibility()
{
    auto threats = iOwner->GetThreatManager().GetThreatenedByMeList();
    for (auto const& pair : threats)
        if (!pair.second->GetOwner()->CanSeeOrDetect(iOwner))
            pair.second->ClearThreat();
}