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

#include "BattlePetMgr.h"
#include "ByteBuffer.h"
#include "Common.h"
#include "DatabaseEnv.h"
#include "DB2Enums.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "Player.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "AchievementMgr.h"

BattlePetMgr::~BattlePetMgr()
{
    for (auto&& it : BattlePets)
        delete it;
}

void BattlePetMgr::LoadFromDb(PreparedQueryResult result)
{
    if (!result)
        return;

    std::set<uint32> species;

    do
    {
        Field* fields = result->Fetch();

        uint64 id            = fields[0].GetUInt64();
        uint16 speciesId     = fields[1].GetUInt16();
        std::string nickname = fields[2].GetString();
        uint32 timestamp     = fields[3].GetUInt32();
        uint8 level          = fields[4].GetUInt8();
        uint16 xp            = fields[5].GetUInt16();
        uint16 health        = fields[6].GetUInt16();
        uint16 maxHealth     = fields[7].GetUInt16();
        uint16 power         = fields[8].GetUInt16();
        uint16 speed         = fields[9].GetUInt16();
        uint8 quality        = fields[10].GetUInt8();
        uint8 breedId        = fields[11].GetUInt8();
        uint16 flags         = fields[12].GetUInt16();

        // check if battle pet species exists
        auto battlePetSpeciesEntry = sBattlePetSpeciesStore.LookupEntry(speciesId);
        if (!battlePetSpeciesEntry)
        {
            TC_LOG_ERROR("sql.sql", "Species %u defined in `account_battle_pet` for Battle Pet " UI64FMTD "  does not exist, skipped!",
                speciesId, id);
            continue;
        }

        if (sBattlePetBreedSet.find(breedId) == sBattlePetBreedSet.end() && breedId != 0)
        {
            TC_LOG_ERROR("sql.sql", "Breed %u defined in `account_battle_pet` for Battle Pet %lu does not exist, skipped.", breedId, (uint64)id);
            continue;
        }

        // highest quality client supports, currently players can not obtain legendary pets on retail
        if (quality > ITEM_QUALITY_LEGENDARY)
        {
            TC_LOG_ERROR("sql.sql", "Quality %u defined in `account_battle_pet` for Battle Pet %lu is invalid, skipped.", quality, (uint64)id);
            continue;
        }

        // client supports up to level 255 (uint8)
        if (level > BATTLE_PET_MAX_LEVEL)
        {
            TC_LOG_ERROR("sql.sql", "Level %u defined in `account_battle_pet` for Battle Pet %lu is invalid, skipped.", quality, (uint64)id);
            continue;
        }

        if (battlePetSpeciesEntry->SpeciesId == 296 || battlePetSpeciesEntry->SpeciesId == 298)
            m_owner->Schedule(Milliseconds(1), [=]()
        {
            if (!m_owner->HasSpell(154165))
                m_owner->LearnSpell(154165, false);
        });

        // Same rules as in Player::AddSpell or it will be fucked
        if (m_owner->GetSession()->GetSecurity() < SEC_MODERATOR)
        {
            if (battlePetSpeciesEntry->Flags & BATTLE_PET_FLAG_HORDE_ONLY && m_owner->GetTeam() == ALLIANCE)
                continue;

            if (battlePetSpeciesEntry->Flags & BATTLE_PET_FLAG_ALLIANCE_ONLY && m_owner->GetTeam() == HORDE)
                continue;
        }

        BattlePet* battlePet = new BattlePet(id, speciesId, battlePetSpeciesEntry->FamilyId,
            nickname, timestamp, level, xp, health, quality, breedId, flags, m_owner);

        BattlePets.insert(battlePet);
        species.insert(battlePet->GetSpecies());

        GetOwner()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_COLLECT_BATTLE_PET, battlePet->GetNpc());
        GetOwner()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_COLLECT_BATTLE_PET_SPECIES, battlePet->GetSpecies());

        if (uint32 summonSpell = db2::GetBattlePetSummonSpell(speciesId))
            GetOwner()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_LEARN_SPELL, summonSpell);
    }
    while (result->NextRow());

    GetOwner()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_OWN_BATTLE_PET_COUNT, species.size());
}

void BattlePetMgr::SaveToDb(CharacterDatabaseTransaction trans)
{
    SaveSlotsToDb(trans);

    if (BattlePets.empty())
        return;

    BattlePetSet::iterator itr = BattlePets.begin();
    while (itr != BattlePets.end())
    {
        BattlePet* battlePet = *itr++;

        if (!battlePet)
            return;

        switch (battlePet->GetDbState())
        {
            case BATTLE_PET_DB_STATE_NONE:
                break;
            case BATTLE_PET_DB_STATE_DELETE:
            {
                CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ACCOUNT_BATTLE_PET);
                stmt->setUInt64(0, battlePet->GetId());
                trans->Append(stmt);

                BattlePets.erase(battlePet);
                delete battlePet;

                break;
            }
            case BATTLE_PET_DB_STATE_SAVE:
            {
                CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ACCOUNT_BATTLE_PET);
                stmt->setUInt64(0, battlePet->GetId());
                trans->Append(stmt);

                CharacterDatabasePreparedStatement* lstmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_ACCOUNT_BATTLE_PET);
                lstmt->setUInt64(0, battlePet->GetId());
                lstmt->setUInt32(1, m_owner->GetSession()->GetAccountId());
                lstmt->setUInt16(2, battlePet->GetSpecies());
                lstmt->setString(3, battlePet->GetNickname());
                lstmt->setUInt32(4, battlePet->GetTimestamp());
                lstmt->setUInt8 (5, battlePet->GetLevel());
                lstmt->setUInt16(6, battlePet->GetXp());
                lstmt->setUInt16(7, battlePet->GetCurrentHealth());
                lstmt->setUInt16(8, battlePet->GetMaxHealth());
                lstmt->setUInt16(9, battlePet->GetPower());
                lstmt->setUInt16(10, battlePet->GetSpeed());
                lstmt->setUInt8 (11, battlePet->GetQuality());
                lstmt->setUInt8 (12, battlePet->GetBreed());
                lstmt->setUInt16(13, battlePet->GetFlags());
                trans->Append(lstmt);

                battlePet->SetDbState(BATTLE_PET_DB_STATE_NONE);
                break;
            }
            default:
                break;
        }
    }
}

void BattlePetMgr::LoadSlotsFromDb(PreparedQueryResult result)
{
    if (!result)
        return;

    Field* fields = result->Fetch();

    ObjectGuid slot1(HighGuid::BattlePet, fields[0].GetUInt32());
    ObjectGuid slot2(HighGuid::BattlePet, fields[1].GetUInt32());
    ObjectGuid slot3(HighGuid::BattlePet, fields[2].GetUInt32());
    m_loadoutFlags = fields[3].GetUInt8();

    // update flag and spell state for new alt characters
    if (m_loadoutFlags != BATTLE_PET_LOADOUT_SLOT_FLAG_NONE && !m_owner->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_BATTLE_PET_ENABLED))
        m_owner->SetFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_BATTLE_PET_ENABLED);

    if (m_owner->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_BATTLE_PET_ENABLED))
    {
        m_owner->LearnSpell(SPELL_BATTLE_PET_TRAINING_PASSIVE, false);
        m_owner->LearnSpell(SPELL_TRACK_PETS, false);
        m_owner->LearnSpell(SPELL_REVIVE_BATTLE_PETS, false);
    }

    std::set<uint8> slotErrors;

    if ((!HasLoadoutSlot(BATTLE_PET_LOADOUT_SLOT_1) || !GetBattlePet(slot1)) && slot1 != 0)
        slotErrors.insert(BATTLE_PET_LOADOUT_SLOT_1);
    if ((!HasLoadoutSlot(BATTLE_PET_LOADOUT_SLOT_2) || !GetBattlePet(slot2)) && slot2 != 0)
        slotErrors.insert(BATTLE_PET_LOADOUT_SLOT_2);
    if ((!HasLoadoutSlot(BATTLE_PET_LOADOUT_SLOT_3) || !GetBattlePet(slot3)) && slot3 != 0)
        slotErrors.insert(BATTLE_PET_LOADOUT_SLOT_3);

    for (std::set<uint8>::const_iterator citr = slotErrors.begin(); citr != slotErrors.end(); citr++)
    {
        TC_LOG_ERROR("sql.sql", "Battle Pet slot %u in `account_battle_pet_slots` for account %u is invalid!",
            *citr, m_owner->GetSession()->GetAccountId());
    }

    bool hasError = slotErrors.size() > 0;
    if (hasError)
        m_loadoutSave = true;

    SetLoadoutSlot(BATTLE_PET_LOADOUT_SLOT_1, hasError ? ObjectGuid::Empty : slot1);
    SetLoadoutSlot(BATTLE_PET_LOADOUT_SLOT_2, hasError ? ObjectGuid::Empty : slot2);
    SetLoadoutSlot(BATTLE_PET_LOADOUT_SLOT_3, hasError ? ObjectGuid::Empty : slot3);
}

void BattlePetMgr::SaveSlotsToDb(CharacterDatabaseTransaction trans)
{
    if (!m_loadoutSave)
        return;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ACCOUNT_BATTLE_PET_SLOTS);
    stmt->setUInt32(0, m_owner->GetSession()->GetAccountId());
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_ACCOUNT_BATTLE_PET_SLOTS);
    stmt->setUInt32(0, m_owner->GetSession()->GetAccountId());
    stmt->setUInt64(1, GetLoadoutSlot(BATTLE_PET_LOADOUT_SLOT_1));
    stmt->setUInt64(2, GetLoadoutSlot(BATTLE_PET_LOADOUT_SLOT_2));
    stmt->setUInt64(3, GetLoadoutSlot(BATTLE_PET_LOADOUT_SLOT_3));
    stmt->setUInt8 (4, GetLoadoutFlags());
    trans->Append(stmt);

    m_loadoutSave = false;
}

BattlePet* BattlePetMgr::GetBattlePet(uint64 id) const
{
    for (BattlePetSet::iterator itr = BattlePets.begin(); itr != BattlePets.end(); itr++)
        if ((*itr)->GetId() == id)
            return *itr;

    return NULL;
}

uint8 BattlePetMgr::GetBattlePetCount(uint16 speciesId) const
{
    uint8 counter = 0;

    for (auto&& itr : BattlePets)
        if (itr->GetDbState() != BATTLE_PET_DB_STATE_DELETE)
            if (itr->GetSpecies() == speciesId)
                counter++;

    return counter;
}

void BattlePetMgr::UnSummonCurrentBattlePet(bool temporary)
{
    if (!m_summonGuid)
    {
        if (m_summonedBattlePetId)
        {
            TC_LOG_ERROR("shitlog", "BattlePetMgr::UnSummonCurrentBattlePet !m_summonGuid && m_summonedBattlePetId");
            m_summonedBattlePetId.Clear();
        }
        return;
    }

    m_lastSummonedBattlePetId = temporary ? m_summonedBattlePetId : ObjectGuid::Empty;
    m_summonedBattlePetId.Clear();

    if (TempSummon* summon = GetCurrentSummon())
        summon->UnSummon();

    m_summonGuid.Clear();
}

void BattlePetMgr::ResummonLastBattlePet()
{
    if (!m_lastSummonedBattlePetId)
        return;

    ObjectGuid battlePetId = m_lastSummonedBattlePetId;
    m_lastSummonedBattlePetId.Clear();

    BattlePet* battlePet = GetBattlePet(battlePetId);
    if (!battlePet)
    {

        TC_LOG_ERROR("shitlog", "BattlePetMgr::ResummonLastBattlePet !summon, player: %s (%u), battle pet: " UI64FMTD "",
            m_owner->GetName().c_str(), m_owner->GetGUID().GetCounter(), battlePetId.GetRawValue());
        return;
    }

    uint32 spell = db2::GetBattlePetSummonSpell(battlePet->GetSpecies());
    m_owner->CastSpell(m_owner, spell, true);

    TempSummon* summon = GetCurrentSummon();
    if (!summon)
    {
        TC_LOG_ERROR("shitlog", "BattlePetMgr::ResummonLastBattlePet !summon, player: %s (%u), battle pet: " UI64FMTD ", spell: %u",
            m_owner->GetName().c_str(), m_owner->GetGUID().GetCounter(), battlePetId.GetRawValue(), spell);
    }

    m_summonedBattlePetId = battlePetId;
}

uint8 BattlePetMgr::GetLoadoutSlotForBattlePet(uint64 id)
{
    for (uint8 i = 0; i < BATTLE_PET_MAX_LOADOUT_SLOTS; i++)
        if (GetLoadoutSlot(i) == id)
            return i;

    return BATTLE_PET_LOADOUT_SLOT_NONE;
}

TempSummon* BattlePetMgr::GetCurrentSummon() const
{
    if (!m_summonGuid)
        return nullptr;
    Creature* creature = ObjectAccessor::GetCreature(*m_owner, m_summonGuid);
    if (!creature)
        return nullptr;
    return creature->ToTempSummon();
}

void BattlePetMgr::UnlockLoadoutSlot(uint8 slot)
{
    if (HasLoadoutSlot(slot))
        return;

    if (slot >= BATTLE_PET_MAX_LOADOUT_SLOTS)
        return;

    switch (slot)
    {
        case BATTLE_PET_LOADOUT_SLOT_1:
            SetLoadoutFlag(BATTLE_PET_LOADOUT_SLOT_FLAG_SLOT_1);
            break;
        case BATTLE_PET_LOADOUT_SLOT_2:
            SetLoadoutFlag(BATTLE_PET_LOADOUT_SLOT_FLAG_SLOT_2);
            SetLoadoutFlag(BATTLE_PET_LOADOUT_TRAP);
            break;
        case BATTLE_PET_LOADOUT_SLOT_3:
            SetLoadoutFlag(BATTLE_PET_LOADOUT_SLOT_FLAG_SLOT_3);
            break;
    }

    SetLoadoutSlot(slot, ObjectGuid::Empty);

    // alert client of new Battle Pet loadout slot
    SendBattlePetSlotUpdate(slot, true);
}

void BattlePetMgr::SetLoadoutSlot(uint8 slot, ObjectGuid id, bool save)
{
    if (!HasLoadoutSlot(slot))
        return;

    m_loadout[slot] = id;

    if (save)
        m_loadoutSave = true;
}

ObjectGuid BattlePetMgr::GetLoadoutSlot(uint8 slot) const
{
    if (!HasLoadoutSlot(slot))
        return ObjectGuid::Empty;

    return m_loadout[slot];
}

bool BattlePetMgr::HasLoadoutSlot(uint8 slot) const
{
    if (!m_loadoutFlags || slot >= BATTLE_PET_MAX_LOADOUT_SLOTS)
        return false;

    switch (slot)
    {
        case BATTLE_PET_LOADOUT_SLOT_1:
            return HasLoadoutFlag(BATTLE_PET_LOADOUT_SLOT_FLAG_SLOT_1);
        case BATTLE_PET_LOADOUT_SLOT_2:
            return HasLoadoutFlag(BATTLE_PET_LOADOUT_SLOT_FLAG_SLOT_2);
        case BATTLE_PET_LOADOUT_SLOT_3:
            return HasLoadoutFlag(BATTLE_PET_LOADOUT_SLOT_FLAG_SLOT_3);
    }

    return false;
}

void BattlePetMgr::SetLoadoutFlag(uint8 flag)
{
    if (HasLoadoutFlag(flag))
        return;

    m_loadoutFlags |= flag;
    m_loadoutSave = true;
}

void BattlePetMgr::Create(uint32 speciesId, uint8 level, uint8 breed, uint8 quality, bool ignoreChecks)
{
    // make sure player can store more battle pets
    if (!CanStoreBattlePet(speciesId) && !ignoreChecks)
        return;

    auto battlePetEntry = sBattlePetSpeciesStore.LookupEntry(speciesId);
    ASSERT(battlePetEntry);

    // create and add battle pet to player
    uint64 id = sObjectMgr->GenerateBattlePetId();

    if (!breed)
        breed = sObjectMgr->BattlePetGetRandomBreed(speciesId);

    if (!quality)
        quality = sObjectMgr->BattlePetGetRandomQuality(speciesId);

    BattlePet* battlePet = new BattlePet(id, speciesId, battlePetEntry->FamilyId, level, quality, breed, m_owner);

    BattlePets.insert(battlePet);

    // notify client of the new battle pet
    SendBattlePetUpdate(battlePet, true);

    std::set<uint32> species;
    for (auto&& it : BattlePets)
        species.insert(it->GetSpecies());

    GetOwner()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_COLLECT_BATTLE_PET, battlePet->GetNpc());

    GetOwner()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_OWN_BATTLE_PET_COUNT, species.size());

    if (uint32 summonSpell = db2::GetBattlePetSummonSpell(speciesId))
        GetOwner()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_LEARN_SPELL, summonSpell);
}

void BattlePetMgr::Delete(BattlePet* battlePet)
{
    if (!battlePet)
        return;

    if (battlePet->GetDbState() == BATTLE_PET_DB_STATE_DELETE)
        return;

    uint32 species = battlePet->GetSpecies();

    // this shouldn't happen since the client doesn't allow releasing of slotted Battle Pets
    uint8 srcSlot = GetLoadoutSlotForBattlePet(battlePet->GetId());
    if (srcSlot != BATTLE_PET_LOADOUT_SLOT_NONE)
    {
        SetLoadoutSlot(srcSlot, ObjectGuid::Empty, true);
        SendBattlePetSlotUpdate(srcSlot, false);
    }

    battlePet->SetDbState(BATTLE_PET_DB_STATE_DELETE);

    // alert client of deleted pet
    SendBattlePetDeleted(battlePet->GetId());

    if (!GetBattlePetCount(species))
        GetOwner()->RemoveSpell(db2::GetBattlePetSummonSpell(species));
}

void BattlePetMgr::CageBattlePet(ObjectGuid guid)
{
    BattlePet* battlePet = GetBattlePet(guid);
    if (!battlePet)
    {
        TC_LOG_DEBUG("network", "WorldSession::CageBattlePet - Player %u tryed to cage battle pet companion " UI64FMTD " which it doesn't own!",
            m_owner->GetGUID().GetCounter(), (uint64)guid);
        return;
    }

    ItemPosCountVec dest;
    InventoryResult err = m_owner->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, BATTLE_PET_CAGE_ITEM_ID, 1);
    if (err != EQUIP_ERR_OK)
    {
        m_owner->SendEquipError(err, nullptr, nullptr, BATTLE_PET_CAGE_ITEM_ID);
        return;
    }

    Item* item = m_owner->StoreNewItem(dest, BATTLE_PET_CAGE_ITEM_ID, true);
    if (!item)
        return;

    item->SetDynamicModifier(ITEM_MODIFIER_INDEX_BATTLE_PET_SPECIES, battlePet->GetSpecies());
    item->SetDynamicModifier(ITEM_MODIFIER_INDEX_BATTLE_PET_BREED,   battlePet->GetBreed() | (battlePet->GetQuality() << 24));
    item->SetDynamicModifier(ITEM_MODIFIER_INDEX_BATTLE_PET_LEVEL,   battlePet->GetLevel());

    m_owner->SendNewItem(item, 1, true, false);

    Delete(battlePet);
}

bool BattlePetMgr::CanStoreBattlePet(uint16 species) const
{
    // check if battle pet species exists
    auto battlePetEntry = sBattlePetSpeciesStore.LookupEntry(species);
    if (!battlePetEntry)
        return false;

    // player is only allowed a maximum of 500 battle pets
    if (BattlePets.size() > BATTLE_PET_MAX_JOURNAL_PETS)
        return false;

    // player is only allowed 3 of each battle pet unless it has BATTLE_PET_FLAG_UNIQUE flag
    uint8 speciesCount = GetBattlePetCount(species);
    if (HasBattlePetSpeciesFlag(species, BATTLE_PET_FLAG_UNIQUE) && speciesCount >= 1)
        return false;

    if (speciesCount >= BATTLE_PET_MAX_JOURNAL_SPECIES)
        return false;

    return true;
}

#define BATTLE_PET_ABILITY_TRAP          427
#define BATTLE_PET_ABILITY_STRONG_TRAP   77
#define BATTLE_PET_ABILITY_PRISTINE_TRAP 135
#define BATTLE_PET_ABILITY_GM_TRAP       549

uint32 BattlePetMgr::GetTrapAbility() const
{
    if (HasLoadoutFlag(BATTLE_PET_LOADOUT_TRAP))
        return BATTLE_PET_ABILITY_TRAP;
    if (HasLoadoutFlag(BATTLE_PET_LOADOUT_STRONG_TRAP))
        return BATTLE_PET_ABILITY_STRONG_TRAP;
    if (HasLoadoutFlag(BATTLE_PET_LOADOUT_PRISTINE_TRAP))
        return BATTLE_PET_ABILITY_PRISTINE_TRAP;
    if (HasLoadoutFlag(BATTLE_PET_LOADOUT_GM_TRAP))
        return BATTLE_PET_ABILITY_GM_TRAP;

    return 0;
}

void BattlePetMgr::SendBattlePetDeleted(ObjectGuid id)
{
    WorldPacket data(SMSG_BATTLE_PET_DELETED, 1 + 8);
    data.WriteBit(id[0]);
    data.WriteBit(id[4]);
    data.WriteBit(id[7]);
    data.WriteBit(id[6]);
    data.WriteBit(id[1]);
    data.WriteBit(id[5]);
    data.WriteBit(id[2]);
    data.WriteBit(id[3]);

    data.WriteByteSeq(id[6]);
    data.WriteByteSeq(id[1]);
    data.WriteByteSeq(id[7]);
    data.WriteByteSeq(id[0]);
    data.WriteByteSeq(id[4]);
    data.WriteByteSeq(id[3]);
    data.WriteByteSeq(id[5]);
    data.WriteByteSeq(id[2]);

    m_owner->GetSession()->SendPacket(&data);
}

void BattlePetMgr::SendBattlePetJournalLock()
{
    WorldPacket data(SMSG_BATTLE_PET_JOURNAL_LOCK_ACQUIRED, 0);
    m_owner->GetSession()->SendPacket(&data);

    /*WorldPacket data(SMSG_BATTLE_PET_JOURNAL_LOCK_DENINED, 0);
    m_owner->GetSession()->SendPacket(&data);*/
}

void BattlePetMgr::SendBattlePetJournal()
{
    uint32 petCount = 0;
    ByteBuffer journalData, slotData;

    // packet size is over estimated
    WorldPacket data(SMSG_BATTLE_PET_JOURNAL, 8 + BattlePets.size() * (2 + 1 + 8 + 1 + 4 + 2 + 4 + 2 + 4 + 2 + 4 + 4 + 4 + 16 + 4 + 2)
        + BATTLE_PET_MAX_LOADOUT_SLOTS * (1 + 1 + 8 + 1));

    size_t writePos = data.bitwpos();
    data.WriteBits(petCount, 19);               // placeholder
    
    for (BattlePetSet::const_iterator citr = BattlePets.begin(); citr != BattlePets.end(); citr++)
    {
        BattlePet const* battlePet = *citr;
        if (battlePet->GetDbState() == BATTLE_PET_DB_STATE_DELETE)
            continue;

        CreatureTemplate const* creatureTemplate = sObjectMgr->GetCreatureTemplate(sBattlePetSpeciesStore.LookupEntry(battlePet->GetSpecies())->NpcId);
        if (!creatureTemplate)
            continue;

        ObjectGuid petEntry = battlePet->GetId();

        data.WriteBit(!battlePet->GetFlags());
        data.WriteBit(petEntry[3]);
        data.WriteBit(petEntry[7]);
        data.WriteBits(battlePet->GetNickname().size(), 7);
        data.WriteBit(0);                       // BATTLE_PET_FLAG_NOT_ACCOUNT_BOUND
        data.WriteBit(petEntry[0]);
        data.WriteBit(petEntry[2]);
        data.WriteBit(petEntry[6]);
        data.WriteBit(0);                       // unknown
        data.WriteBit(petEntry[1]);
        data.WriteBit(petEntry[5]);
        data.WriteBit(!battlePet->GetBreed());
        data.WriteBit(petEntry[4]);
        data.WriteBit(!battlePet->GetQuality());

        if (battlePet->GetQuality())
            journalData << uint8(battlePet->GetQuality());

        journalData << uint32(battlePet->GetPower());
        journalData.WriteByteSeq(petEntry[7]);
        journalData << uint16(battlePet->GetLevel());
        journalData << uint32(battlePet->GetCurrentHealth());

        if (battlePet->GetBreed())
            journalData << uint16(battlePet->GetBreed());

        journalData << uint32(battlePet->GetSpecies());
        journalData.WriteByteSeq(petEntry[2]);

        if (battlePet->GetFlags())
            journalData << uint16(battlePet->GetFlags());

        journalData << uint32(creatureTemplate->Entry);
        journalData << uint32(creatureTemplate->GetModelByIdx(0)->CreatureDisplayID);
        journalData << uint32(battlePet->GetSpeed());
        journalData.WriteString(battlePet->GetNickname());
        journalData.WriteByteSeq(petEntry[6]);
        journalData.WriteByteSeq(petEntry[5]);
        journalData << uint32(battlePet->GetMaxHealth());
        journalData.WriteByteSeq(petEntry[4]);
        journalData << uint16(battlePet->GetXp());
        journalData.WriteByteSeq(petEntry[0]);
        journalData.WriteByteSeq(petEntry[1]);
        journalData.WriteByteSeq(petEntry[3]);

        petCount++;
    }

    data.WriteBit(1);                           // slots enabled
    data.WriteBits(BATTLE_PET_MAX_LOADOUT_SLOTS, 25);

    for (uint8 i = 0; i < BATTLE_PET_MAX_LOADOUT_SLOTS; i++)
    {
        ObjectGuid loadoutEntry = GetLoadoutSlot(i);

        data.WriteBit(!HasLoadoutSlot(i));
        data.WriteBit(1);                       // unknown
        data.WriteBit(0);                       // has slot number
        data.WriteBit(0);                       // fake
        data.WriteBit(loadoutEntry[0]);
        data.WriteBit(loadoutEntry[1]);
        data.WriteBit(loadoutEntry[7]);
        data.WriteBit(loadoutEntry[6]);
        data.WriteBit(loadoutEntry[4]);
        data.WriteBit(loadoutEntry[2]);
        data.WriteBit(loadoutEntry[5]);
        data.WriteBit(loadoutEntry[3]);

        slotData.WriteByteSeq(loadoutEntry[5]);
        slotData.WriteByteSeq(loadoutEntry[1]);
        slotData.WriteByteSeq(loadoutEntry[7]);
        slotData.WriteByteSeq(loadoutEntry[2]);
        slotData.WriteByteSeq(loadoutEntry[3]);
        slotData.WriteByteSeq(loadoutEntry[0]);
        slotData.WriteByteSeq(loadoutEntry[4]);
        slotData.WriteByteSeq(loadoutEntry[6]);
        slotData << uint8(i);
    }

    data.FlushBits();
    data.PutBits(writePos, petCount, 19);

    data.append(slotData);
    data.append(journalData);
    data << uint16(0);                          // unknown

    m_owner->GetSession()->SendPacket(&data);
}

void BattlePetMgr::SendBattlePetSlotUpdate(uint8 slot, bool notification, ObjectGuid id)
{
    WorldPacket data(SMSG_BATTLE_PET_SLOT_UPDATE, 5 + 1);
    data.WriteBits(1, 25);
    data.WriteBit(0);                   // unknown

    {
        data.WriteBit(0);               // has slot number
        data.WriteBit(1);               // unknown
        data.WriteBit(0);               // unknown
        data.WriteBit(0);               // fake
        data.WriteBit(id[4]);
        data.WriteBit(id[5]);
        data.WriteBit(id[2]);
        data.WriteBit(id[1]);
        data.WriteBit(id[0]);
        data.WriteBit(id[3]);
        data.WriteBit(id[7]);
        data.WriteBit(id[6]);
    }

    data.WriteBit(notification);
    data.FlushBits();

    {
        data.WriteByteSeq(id[0]);
        data.WriteByteSeq(id[3]);
        data.WriteByteSeq(id[2]);
        data.WriteByteSeq(id[1]);
        data.WriteByteSeq(id[6]);
        data.WriteByteSeq(id[4]);
        data.WriteByteSeq(id[5]);
        data.WriteByteSeq(id[7]);
        data << uint8(slot);
    }

    m_owner->GetSession()->SendPacket(&data);
}

void BattlePetMgr::SendBattlePetUpdate(BattlePet* battlePet, bool notification)
{
    ObjectGuid petEntry = battlePet->GetId();

    CreatureTemplate const* creatureTemplate = sObjectMgr->GetCreatureTemplate(sBattlePetSpeciesStore.LookupEntry(battlePet->GetSpecies())->NpcId);
    if (!creatureTemplate)
        return;

    WorldPacket data(SMSG_BATTLE_PET_UPDATE, 4 + 1 + 8 + battlePet->GetNickname().size() + 4 + 4 + 4 + 4 + 2 + 4 + 4 + 1 + 2 + 2 + 4 + 2);
    data.WriteBits(1, 19);
    data.WriteBit(petEntry[4]);
    data.WriteBit(petEntry[1]);
    data.WriteBit(petEntry[7]);
    data.WriteBit(!battlePet->GetQuality());
    data.WriteBit(!battlePet->GetBreed());
    data.WriteBit(petEntry[5]);
    data.WriteBit(0);                           // unknown
    data.WriteBit(petEntry[2]);
    data.WriteBit(!battlePet->GetFlags());
    data.WriteBit(0);                           // BATTLE_PET_FLAG_NOT_ACCOUNT_BOUND
    data.WriteBit(petEntry[6]);
    data.WriteBit(petEntry[3]);
    data.WriteBits(battlePet->GetNickname().size(), 7);
    data.WriteBit(petEntry[0]);
    data.WriteBit(notification);
    data.FlushBits();

    data.WriteByteSeq(petEntry[1]);
    data.WriteString(battlePet->GetNickname());
    data << uint32(battlePet->GetCurrentHealth());
    data << uint32(battlePet->GetSpecies());
    data.WriteByteSeq(petEntry[0]);
    data << uint32(battlePet->GetPower());
    data << uint32(battlePet->GetSpeed());

    if (battlePet->GetBreed())
        data << uint16(battlePet->GetBreed());

    data.WriteByteSeq(petEntry[4]);
    data << uint32(creatureTemplate->Entry);
    data << uint32(battlePet->GetMaxHealth());
    data.WriteByteSeq(petEntry[6]);

    if (battlePet->GetQuality())
        data << uint8(battlePet->GetQuality());

    data.WriteByteSeq(petEntry[2]);
    data.WriteByteSeq(petEntry[3]);
    data << uint16(battlePet->GetXp());
    data.WriteByteSeq(petEntry[7]);

    if (battlePet->GetFlags())
        data << uint16(battlePet->GetFlags());

    data.WriteByteSeq(petEntry[5]);
    data << uint32(creatureTemplate->GetModelByIdx(0)->CreatureDisplayID);
    data << uint16(battlePet->GetLevel());

    m_owner->GetSession()->SendPacket(&data);
}
