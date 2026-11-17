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

#ifndef BATTLE_PET_MGR_H
#define BATTLE_PET_MGR_H

#include "BattlePet.h"
#include "Common.h"
#include "TemporarySummon.h"

typedef std::set<BattlePet*> BattlePetSet;

enum
{
    BATTLE_PET_MAX_JOURNAL_SPECIES = 3,
    BATTLE_PET_MAX_JOURNAL_PETS    = 1000,
    BATTLE_PET_MAX_LOADOUT_SLOTS   = 3,
    BATTLE_PET_CAGE_ITEM_ID        = 82800,
};

enum BattlePetLoadoutSlots
{
    BATTLE_PET_LOADOUT_SLOT_1                   = 0,
    BATTLE_PET_LOADOUT_SLOT_2                   = 1,
    BATTLE_PET_LOADOUT_SLOT_3                   = 2,
    BATTLE_PET_LOADOUT_SLOT_NONE
};

// custom flags used in 'account_battle_pet_slots' db table
// custom flags used in 'account_battle_pet_slots' table
enum BattlePetLoadoutFlags
{
    BATTLE_PET_LOADOUT_SLOT_FLAG_NONE           = 0x00,
    BATTLE_PET_LOADOUT_SLOT_FLAG_SLOT_1         = 0x01,
    BATTLE_PET_LOADOUT_SLOT_FLAG_SLOT_2         = 0x02,
    BATTLE_PET_LOADOUT_SLOT_FLAG_SLOT_3         = 0x04,
    BATTLE_PET_LOADOUT_TRAP                     = 0x08,
    BATTLE_PET_LOADOUT_STRONG_TRAP              = 0x10,
    BATTLE_PET_LOADOUT_PRISTINE_TRAP            = 0x20,
    BATTLE_PET_LOADOUT_GM_TRAP                  = 0x40
};

// flags used in 'account_battle_pet' db table
enum BattlePetJournalFlags
{
    BATTLE_PET_JOURNAL_FLAG_NONE                = 0x00,
    BATTLE_PET_JOURNAL_FLAG_FAVORITES           = 0x01,
    BATTLE_PET_JOURNAL_FLAG_COLLECTED           = 0x02, // name dumped from client, use unknown
    BATTLE_PET_JOURNAL_FLAG_NOT_COLLECTED       = 0x04,
    BATTLE_PET_JOURNAL_FLAG_UNKNOWN_1           = 0x08,
    BATTLE_PET_JOURNAL_FLAG_ABILITY_1           = 0x10, // ability flags are set if the second ability for that slot is selected
    BATTLE_PET_JOURNAL_FLAG_ABILITY_2           = 0x20, // ...
    BATTLE_PET_JOURNAL_FLAG_ABILITY_3           = 0x40  // ...
};

enum BattlePetSpells
{
    SPELL_BATTLE_PET_TRAINING_PASSIVE           = 119467,
    SPELL_TRACK_PETS                            = 122026,
    SPELL_REVIVE_BATTLE_PETS                    = 125439
};

class BattlePetMgr
{
public:
    BattlePetMgr(Player* owner) : m_owner(owner) { }

    ~BattlePetMgr();

    Player* GetOwner() const { return m_owner; }
    BattlePet* GetBattlePet(uint64 id) const;
    uint8 GetBattlePetCount(uint16 speciesId) const;
    uint8 GetLoadoutSlotForBattlePet(uint64 id);

    uint64 GetCurrentSummonId() const { return m_summonedBattlePetId; }
    TempSummon* GetCurrentSummon() const;
    void SetCurrentSummonId(ObjectGuid summonId) { m_summonedBattlePetId = summonId; }
    void SetCurrentSummon(ObjectGuid guid) { m_summonGuid = guid; if (!guid) m_summonedBattlePetId = ObjectGuid::Empty; }

    void UnSummonCurrentBattlePet(bool temporary);
    void ResummonLastBattlePet();

    void LoadFromDb(PreparedQueryResult result);
    void LoadSlotsFromDb(PreparedQueryResult result);
    void SaveToDb(CharacterDatabaseTransaction trans);
    void SaveSlotsToDb(CharacterDatabaseTransaction trans);

    void UnlockLoadoutSlot(uint8 slot);
    bool HasLoadoutSlot(uint8 slot) const;
    ObjectGuid GetLoadoutSlot(uint8 slot) const;
    void SetLoadoutSlot(uint8 slot, ObjectGuid id, bool save = false);

    bool HasLoadoutFlag(uint8 flag) const { return (m_loadoutFlags & flag) != 0; };
    uint8 GetLoadoutFlags() const { return m_loadoutFlags; };
    void SetLoadoutFlag(uint8 flag);

    void Create(uint32 speciesId, uint8 level = 1, uint8 breed = 0, uint8 quality = 0, bool ignoreChecks = false);
    void Delete(BattlePet* battlePet);
    void CageBattlePet(ObjectGuid guid);

    bool CanStoreBattlePet(uint16 species) const;
    uint32 GetTrapAbility() const;

    void SendBattlePetDeleted(ObjectGuid id);
    void SendBattlePetJournalLock();
    void SendBattlePetJournal();
    void SendBattlePetSlotUpdate(uint8 slot, bool notification, ObjectGuid id = ObjectGuid::Empty);
    void SendBattlePetUpdate(BattlePet* battlePet, bool notification);

    BattlePetSet BattlePets;

private:
    Player* m_owner;

    ObjectGuid m_summonGuid;
    ObjectGuid m_summonedBattlePetId;
    ObjectGuid m_lastSummonedBattlePetId;

    bool m_loadoutSave = false;
    uint8 m_loadoutFlags = 0;
    ObjectGuid m_loadout[BATTLE_PET_MAX_LOADOUT_SLOTS];

    BattlePetMgr(const BattlePetMgr&) = delete;
    BattlePetMgr& operator=(const BattlePetMgr&) = delete;
};

#endif
