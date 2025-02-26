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

#ifndef BATTLE_PET_AURA_H
#define BATTLE_PET_AURA_H

#include "Common.h"

class BattlePet;
class PetBattle;

//              stateId  value
typedef std::map<uint32, int32> AuraStateStore;

class BattlePetAura
{
public:
    BattlePetAura(PetBattle* petBattle, uint32 id, uint32 ability, uint32 /*trigger*/, BattlePet* caster, BattlePet* target, int32 duration)
        : m_id(id), m_ability(ability), m_duration(duration), m_maxDuration(duration), m_caster(caster), m_target(target),
        m_petBattle(petBattle) { }

    uint32 GetId() { return m_id; }
    uint32 GetAbility() { return m_ability; }
    int32 GetDuration() { return m_duration; }
    int32 GetTurn() { return m_turn; }
    bool HasExpired() { return m_expired; }

    void OnApply();
    void OnExpire();
    void Process();
    void Expire();

private:
    uint32 m_id;
    uint32 m_ability;
    uint32 m_trigger = 0;                           // ability effect that trigger aura
    int32 m_turn = 1;
    int32 m_duration;                               // duration remaining
    int32 m_maxDuration;

    AuraStateStore m_auraStates;                    // contains states modified by the aura

    BattlePet* m_caster;
    BattlePet* m_target;
    PetBattle* m_petBattle;                   // parent pet battle
    bool m_expired = false;
};

#endif // !BATTLE_PET_AURA_H
