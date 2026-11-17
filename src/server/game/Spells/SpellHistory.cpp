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

#include "SpellHistory.h"
#include "DBCStores.h"
#include "Player.h"
#include "Pet.h"

static TimeValue const infinityCooldownDelay{ Seconds(MONTH) };  // used for set "infinity cooldowns" for spells and check
static TimeValue const infinityCooldownDelayCheck{ Seconds(MONTH / 2) };

SpellHistory::SpellHistory(Unit* owner)
    : _owner(owner)
{
}

void SpellHistory::LoadFromDB(PreparedQueryResult&& result, PreparedQueryResult&& chargesResult)
{
    auto now = TimeValue::Now();

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 spellId = fields[0].GetUInt32();
            uint32 itemId = fields[1].GetUInt32();
            TimeValue end = Milliseconds(fields[2].GetUInt64());
            TimeValue categoryEnd = Milliseconds(fields[3].GetUInt64());
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
            if (!spellInfo)
            {
                TC_LOG_ERROR("entities.player.loading", "{} {} has unknown spell {} in `character_spell_cooldown`, skipping.",
                    _owner->GetTypeId() == TYPEID_PLAYER ? "Player" : "Pet", _owner->GetGUID().GetCounter(), spellId);
                continue;
            }

            // skip outdated cooldown
            if (end <= now && categoryEnd <= now)
                continue;

            auto& entry = _cooldowns[spellId];
            entry.SpellId = spellId;
            entry.ItemId = itemId;
            entry.CooldownEnd = end;
            entry.CategoryId = spellInfo->GetCategory();
            entry.CategoryEnd = categoryEnd;
            if (entry.CategoryId)
                _categoryCooldowns[entry.CategoryId] = entry;

        } while (result->NextRow());
    }

    if (chargesResult)
    {
        do
        {
            Field* fields = chargesResult->Fetch();
            uint32 category = fields[0].GetUInt32();
            auto catergoryEntry = sSpellCategoryStore.LookupEntry(category);
            if (!catergoryEntry)
                continue;

            auto& chargeData = _spellCharges[category];
            chargeData.BaseRegenTime = Milliseconds(catergoryEntry->ChargeRegenTime);
            chargeData.ConsumedCharges = fields[2].GetUInt32();
            chargeData.CurrentResetTime = Milliseconds(fields[1].GetUInt64());;

        } while (chargesResult->NextRow());

        UpdateCharges();
    }
}

template <>
void SpellHistory::SaveToDB<Player>(CharacterDatabaseTransaction trans)
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_SPELL_COOLDOWN);
    stmt->setUInt32(0, _owner->GetGUID().GetCounter());
    trans->Append(stmt);

    TimeValue now = TimeValue::Now();

    // remove outdated and save active
    for (auto it = _cooldowns.begin(); it != _cooldowns.end();)
    {
        auto& entry = it->second;
        if (entry.CooldownEnd <= now && entry.CategoryEnd <= now)
            _cooldowns.erase(it++);
        else
        {
            if (!entry.OnHold)
            {
                CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAR_SPELL_COOLDOWN);
                stmt->setUInt32(0, _owner->GetGUID().GetCounter());
                stmt->setUInt32(1, entry.SpellId);
                stmt->setUInt32(2, entry.ItemId);
                stmt->setUInt64(3, entry.CooldownEnd.ToMilliseconds());
                stmt->setUInt64(4, entry.CategoryEnd.ToMilliseconds());
                trans->Append(stmt);
            }
            ++it;
        }
    }

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_SPELL_CHARGES);
    stmt->setUInt32(0, _owner->GetGUID().GetCounter());
    trans->Append(stmt);

    UpdateCharges();

    for (auto&& itr : _spellCharges)
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAR_SPELL_CHARGES);
        stmt->setUInt32(0, _owner->GetGUID().GetCounter());
        stmt->setUInt32(1, itr.first);
        stmt->setUInt64(2, itr.second.CurrentResetTime.ToMilliseconds());
        stmt->setUInt32(3, itr.second.ConsumedCharges);
        trans->Append(stmt);
    }
}

template <>
void SpellHistory::SaveToDB<Pet>(CharacterDatabaseTransaction trans)
{
    auto charmInfo = _owner->GetCharmInfo();
    if (!charmInfo)
        return;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_PET_SPELL_COOLDOWNS);
    stmt->setUInt32(0, charmInfo->GetPetNumber());
    trans->Append(stmt);

    // remove oudated and save active
    TimeValue now = TimeValue::Now();

    // remove outdated and save active
    for (auto itr = _cooldowns.begin(); itr != _cooldowns.end();)
    {
        auto& entry = itr->second;
        if (entry.CooldownEnd <= now && entry.CategoryEnd <= now)
            _cooldowns.erase(itr++);
        else
        {
            if (!entry.OnHold)
            {
                CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_PET_SPELL_COOLDOWN);
                stmt->setUInt32(0, charmInfo->GetPetNumber());
                stmt->setUInt32(1, entry.SpellId);
                stmt->setUInt64(2, entry.CooldownEnd.ToMilliseconds());
                stmt->setUInt64(3, entry.CategoryEnd.ToMilliseconds());
                trans->Append(stmt);
            }
            ++itr;
        }
    }
}

bool SpellHistory::IsReady(uint32 spellId, uint32 itemId, bool ignoreCategoryCooldown) const
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
        return true;
    return IsReady(spellInfo, itemId, ignoreCategoryCooldown);
}

bool SpellHistory::IsReady(SpellInfo const* spellInfo, uint32 itemId, bool ignoreCategoryCooldown) const
{
    if (HasCooldown(spellInfo))
        return false;

    auto categories = spellInfo->GetSpellCategories();
    if (categories && categories->ChargesCategory && !HasCharge(categories->ChargesCategory))
        return false;


    if (!ignoreCategoryCooldown)
    {
        auto cooldown = GetCooldownInfo(spellInfo, itemId);
        if (HasCategoryCooldown(cooldown.Category))
            return false;
    }
    return true;
}

bool SpellHistory::HasCooldown(SpellInfo const* spellInfo) const
{
    auto it = _cooldowns.find(spellInfo->Id);
    if (it == _cooldowns.end())
        return false;

    auto& entry = it->second;
    if (entry.OnHold)
        return true;

    return entry.CooldownEnd > TimeValue::Now();
}

bool SpellHistory::HasCharge(uint32 categoryId) const
{
    auto category = sSpellCategoryStore.LookupEntry(categoryId);
    if (!category)
        return true;

    // Spell 127252 has charges category 133 with 0 regen time. Bad data in DBC?
    if (category->ChargeRegenTime == 0)
        return true;

    auto itr = _spellCharges.find(category->Id);
    if (itr == _spellCharges.end())
        return true;

    auto now = TimeValue::Now();

    uint8 usedCharges = itr->second.ConsumedCharges;
    TimeValue resetTime = itr->second.CurrentResetTime;
    while (usedCharges && resetTime <= now)
    {
        --usedCharges;
        resetTime += itr->second.BaseRegenTime;
    }

    if (!usedCharges)
        return true;

    uint32 maxCharges = category->MaxCharges;
    if (!maxCharges)
        maxCharges = uint32(_owner->GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_CHARGES, category->Id));

    return !maxCharges || usedCharges < maxCharges;
}

bool SpellHistory::HasCategoryCooldown(uint32 categoryId) const
{
    auto itr = _categoryCooldowns.find(categoryId);
    if (itr == _categoryCooldowns.end())
        return false;

    auto& entry = itr->second;
    if (entry.OnHold)
        return true;
    return entry.CategoryEnd > TimeValue::Now();
}

void SpellHistory::SetCooldownOnHold(SpellInfo const* spellInfo, uint32 itemId)
{
    if (HasCooldown(spellInfo)) // Alter Time returned Presence of Mind
        return;

    auto cooldown = GetCooldownInfo(spellInfo, itemId);

    auto& entry = _cooldowns[spellInfo->Id];
    entry.SpellId = spellInfo->Id;
    entry.ItemId = itemId;
    entry.CategoryId = cooldown.Category;
    entry.OnHold = true;
    entry.CooldownEnd = TimeValue::zero();
    entry.CategoryEnd = TimeValue::zero();
    if (cooldown.Category)
        _categoryCooldowns[cooldown.Category] = entry;
}

void SpellHistory::StartCooldown(SpellInfo const* spellInfo, uint32 itemId, Spell* spell)
{
    auto cooldown = GetCooldownInfo(spellInfo, itemId);

    auto curTime = TimeValue::Now();
    
    auto categories = spellInfo->GetSpellCategories();
    if (categories && categories->ChargesCategory != 0)
    {
        auto category = sSpellCategoryStore.LookupEntry(categories->ChargesCategory);
        if (category && category->ChargeRegenTime != 0)
        {
            //if (category->MaxCharges || _owner->HasAuraTypeWithMiscvalue(SPELL_AURA_MOD_CHARGES, category->Id))
            {
                auto res = _spellCharges.insert({ category->Id, ChargeData() });
                auto& chargeData = res.first->second;
                ++chargeData.ConsumedCharges;
                if (res.second)
                {
                    chargeData.BaseRegenTime = Milliseconds(category->ChargeRegenTime);
                    chargeData.CurrentResetTime = curTime + chargeData.BaseRegenTime;
                }
            }
        }
    }

    if (Player* modOwner = _owner->GetSpellModOwner())
    {
        // Now we have cooldown data (if found any), time to apply mods
        modOwner->ApplySpellMod(spellInfo->Id, SPELLMOD_COOLDOWN, cooldown.RecoveryTime, spell);

        if (cooldown.CategoryRecoveryTime > 0 && !(spellInfo->AttributesEx6 & SPELL_ATTR6_IGNORE_CATEGORY_COOLDOWN_MODS))
            modOwner->ApplySpellMod(spellInfo->Id, SPELLMOD_COOLDOWN, cooldown.CategoryRecoveryTime, spell);
    }
    // Apply SPELL_AURA_MOD_SPELL_CATEGORY_COOLDOWN modifiers
    // Note: This aura applies its modifiers to all cooldowns of spells with set category, not to category cooldown only
    if (cooldown.Category)
    {
        if (int32 categoryModifier = _owner->GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_SPELL_CATEGORY_COOLDOWN, cooldown.Category))
        {
            if (cooldown.RecoveryTime > 0)
                cooldown.RecoveryTime += categoryModifier;

            if (cooldown.CategoryRecoveryTime > 0)
                cooldown.CategoryRecoveryTime += categoryModifier;
        }

        SpellCategoryEntry const* categoryEntry = sSpellCategoryStore.LookupEntry(cooldown.Category);
        ASSERT(categoryEntry);
        if (categoryEntry->Flags & SPELL_CATEGORY_FLAG_COOLDOWN_EXPIRES_AT_MIDNIGHT)
        {
            tm date;
            time_t now = curTime.ToSeconds();
            localtime_r(&now, &date);
            cooldown.CategoryRecoveryTime = cooldown.CategoryRecoveryTime * DAY - (date.tm_hour * HOUR + date.tm_min * MINUTE + date.tm_sec) * IN_MILLISECONDS;
        }
    }

    if (cooldown.RecoveryTime < 0)
        cooldown.RecoveryTime = 0;
    if (cooldown.CategoryRecoveryTime < 0)
        cooldown.CategoryRecoveryTime = 0;

    // Basically can be only one, spell/melee taken into account in GetHasteMultiplier
    for (auto&& type : { SPELL_AURA_MOD_COOLDOWN_BY_HASTE, SPELL_AURA_MOD_COOLDOWN_BY_SPELL_HASTE })
    {
        for (auto&& itr : _owner->GetAuraEffectsByType(type))
        {
            if (itr->IsAffectingSpell(spellInfo))
            {
                float mult = _owner->GetHasteMultiplier();

                float hastepct = (1.0f - 1.0f / mult) * 100.0f;
                AddPct(cooldown.RecoveryTime, -hastepct);
                AddPct(cooldown.CategoryRecoveryTime, -hastepct);
            }
        }
    }

    if (cooldown.RecoveryTime == 0 && cooldown.CategoryRecoveryTime == 0)
        return;

    int64 catrecTime = cooldown.CategoryRecoveryTime;
    int64 recTime = cooldown.RecoveryTime ? cooldown.RecoveryTime : catrecTime;

    if (recTime || catrecTime)
        AddCooldown(spellInfo->Id, itemId, recTime, cooldown.Category, catrecTime);
}

void SpellHistory::StartCooldownOnEvent(SpellInfo const* spellInfo, uint32 itemId, Spell* spell)
{
    // Remove OnHold event if spell hasn't cooldown
    auto itr = _cooldowns.find(spellInfo->Id);
    if (itr != _cooldowns.end())
    {
        if (!itr->second.OnHold) // Returned by Alter Time Presence of Mind was used.
            return;
        itr->second.OnHold = false;
    }

    if (spellInfo->GetCategory() || itemId)
    {
        auto cooldown = GetCooldownInfo(spellInfo, itemId);
        auto citr = _categoryCooldowns.find(cooldown.Category);
        if (citr != _categoryCooldowns.end())
            citr->second.OnHold = false;
    }
    StartCooldown(spellInfo, itemId, spell);
}

void SpellHistory::ProhibitSpell(uint32 spellId, TimeValue cooldow)
{
    auto now = TimeValue::Now();

    auto itr = _cooldowns.find(spellId);
    if (itr == _cooldowns.end())
    {
        AddCooldown(spellId, 0, cooldow.ToMilliseconds(), 0, 0);
        _cooldowns[spellId].ProhibitionEnd = _cooldowns[spellId].CooldownEnd;
    }
    else
    {
        itr->second.ProhibitionEnd = now + cooldow;
    }
}

void SpellHistory::AddCooldown(uint32 spellId, uint32 itemId, int64 cooldown, uint32 categroy, int64 categoryCooldown)
{
    auto now = TimeValue::Now();

    auto& entry = _cooldowns[spellId];
    entry.SpellId = spellId;
    entry.OnHold = false;
    entry.ProhibitionEnd = TimeValue::zero();
    entry.CooldownEnd = now + Milliseconds(cooldown);
    entry.CategoryId = categroy;
    entry.ItemId = itemId;
    if (categroy)
    {
        entry.CategoryEnd = now + Milliseconds(categoryCooldown);
        _categoryCooldowns[categroy] = entry;
    }
}

void SpellHistory::RemoveCooldown(uint32 spellId, bool send)
{
    if (auto const* spellInfo = sSpellMgr->GetSpellInfo(spellId))
    {
        uint32 category = spellInfo->GetCategory();
        {
            auto itr = _cooldowns.find(spellId);
            if (itr != _cooldowns.end())
            {
                if (itr->second.ProhibitionEnd)
                {
                    if (itr->second.ProhibitionEnd < itr->second.CooldownEnd)
                        ModifyCooldown(spellId, -(itr->second.CooldownEnd - itr->second.ProhibitionEnd).ToMilliseconds());
                    return;
                }

                auto cooldown = GetCooldownInfo(spellInfo, itr->second.ItemId);
                category = cooldown.Category;
                _cooldowns.erase(itr);
            }
        }
        _categoryCooldowns.erase(category);

        if (send)
            GetPlayer()->SendClearCooldown(spellId, _owner);
    }
}

void SpellHistory::RemoveArenaSpellCooldowns(bool removeActivePetCooldowns)
{
    ASSERT(_owner->GetTypeId() == TYPEID_PLAYER);

    // remove cooldowns on spells that have <= 10 min CD
    auto it = _cooldowns.begin();
    auto next = it;
    for (; it != _cooldowns.end(); it = next)
    {
        next = it;
        ++next;

        // skip items cooldownd
        auto item = sObjectMgr->GetItemTemplate(it->second.ItemId);
        if (item && item->InventoryType != INVTYPE_TRINKET)
            continue;

        SpellInfo const* entry = sSpellMgr->GetSpellInfo(it->first);
        // check if spellentry is present and if the cooldown is less or equal to 10 min
        if (entry &&
            entry->RecoveryTime <= 10 * MINUTE * IN_MILLISECONDS &&
            entry->CategoryRecoveryTime <= 10 * MINUTE * IN_MILLISECONDS)
        {
            if (entry->CategoryEntry && entry->CategoryEntry->Flags & SPELL_CATEGORY_FLAG_COOLDOWN_EXPIRES_AT_MIDNIGHT)
                continue;

            // remove & notify
            RemoveCooldown(it->first, true);
        }
    }

    for (auto it = _spellCharges.begin(); it != _spellCharges.end();)
    {
        // If we replenish last consumed charge, simply erase charge data
        if (it->second.BaseRegenTime < Minutes(10))
        {
            ClearChargesForSpell(it->first);
            _spellCharges.erase(it++);
        }
        else
            ++it;
    }

    SendSpellCharges();

    // pet cooldowns
    if (removeActivePetCooldowns)
        if (Pet* pet = _owner->ToPlayer()->GetPet())
        {
            // notify player
            for (CreatureSpellCooldowns::const_iterator itr2 = pet->m_CreatureSpellCooldowns.begin(); itr2 != pet->m_CreatureSpellCooldowns.end(); ++itr2)
                _owner->ToPlayer()->SendClearCooldown(itr2->first, pet);

            // actually clear cooldowns
            pet->m_CreatureSpellCooldowns.clear();
        }
}

void SpellHistory::RemoveAllCooldowns()
{
    ClearAllSpellCharges();
    _spellCharges.clear();

    if (_cooldowns.empty())
        return;

    uint32 spellCount = _cooldowns.size();
    ObjectGuid guid = _owner->GetGUID();

    WorldPacket data(SMSG_CLEAR_COOLDOWNS, 1 + 8 + 3 + (spellCount * 4));
    data.WriteBit(guid[5]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[2]);
    data.WriteBits(spellCount, 22); // Spell Count
    data.WriteBit(guid[1]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[4]);
    data.FlushBits();

    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[6]);

    for (auto&& it : _cooldowns)
        data << uint32(it.first);

    data.WriteByteSeq(guid[2]);

    GetPlayer()->SendDirectMessage(&data);

    _cooldowns.clear();
    _categoryCooldowns.clear();
}

void SpellHistory::ModifyCooldown(uint32 spellId, int32 cooldown)
{
    auto itr = _cooldowns.find(spellId);
    if (itr == _cooldowns.end())
        return;

    if (itr->second.OnHold)
        return;

    auto now = TimeValue::Now();
    TimeValue mscooldown = Milliseconds(cooldown);

    if (cooldown < 0 && itr->second.ProhibitionEnd)
    {
        // Nothing to do. Prohibition can't be removed.
        if (itr->second.CooldownEnd < itr->second.ProhibitionEnd)
            return;

        if (itr->second.CooldownEnd + mscooldown < itr->second.ProhibitionEnd)
        {
            cooldown = itr->second.CooldownEnd - itr->second.ProhibitionEnd;
            mscooldown = Milliseconds(cooldown);
        }
    }

    if (itr->second.CooldownEnd + mscooldown > now)
    {
        itr->second.CooldownEnd += mscooldown;
        if (itr->second.CategoryId)  // Probably wrong
        {
            itr->second.CategoryEnd += mscooldown;
            _categoryCooldowns[itr->second.CategoryId] = itr->second;
        }
    }
    else
    {
        _categoryCooldowns.erase(itr->second.CategoryId);
        _cooldowns.erase(itr);
    }

    ObjectGuid playerGuid = _owner->GetGUID();          // Player GUID
    WorldPacket data(SMSG_MODIFY_COOLDOWN, 4 + 8 + 4);

    data.WriteBit(playerGuid[2]);
    data.WriteBit(playerGuid[1]);
    data.WriteBit(playerGuid[0]);
    data.WriteBit(playerGuid[4]);
    data.WriteBit(playerGuid[7]);
    data.WriteBit(playerGuid[3]);
    data.WriteBit(playerGuid[6]);
    data.WriteBit(playerGuid[5]);

    data.WriteByteSeq(playerGuid[4]);
    data.WriteByteSeq(playerGuid[1]);
    data << uint32(cooldown);           // Cooldown mod in milliseconds
    data.WriteByteSeq(playerGuid[3]);
    data.WriteByteSeq(playerGuid[6]);
    data.WriteByteSeq(playerGuid[7]);
    data.WriteByteSeq(playerGuid[5]);
    data.WriteByteSeq(playerGuid[0]);
    data << int32(spellId);             // Spell ID
    data.WriteByteSeq(playerGuid[2]);

    GetPlayer()->SendDirectMessage(&data);
}

void SpellHistory::SendCooldown(uint32 spellId, int32 cooldown)
{
    ObjectGuid guid = _owner->GetGUID();
    WorldPacket data(SMSG_SPELL_COOLDOWN, 9 + 3 + 8);
    data.WriteBit(guid[0]);
data.WriteBit(guid[6]);
    data.WriteBit(1); // Missing flags
    data.WriteBit(guid[7]);
data.WriteBit(guid[3]);
data.WriteBit(guid[1]);
data.WriteBit(guid[5]);
    data.WriteBits(1, 21);
    data.WriteBit(guid[2]);
data.WriteBit(guid[4]);

    data.FlushBits();

    data << uint32(spellId);
    data << uint32(cooldown);
    data.WriteByteSeq(guid[5]);
data.WriteByteSeq(guid[3]);
data.WriteByteSeq(guid[7]);
data.WriteByteSeq(guid[4]);
data.WriteByteSeq(guid[1]);
data.WriteByteSeq(guid[0]);
data.WriteByteSeq(guid[2]);
data.WriteByteSeq(guid[6]);
    GetPlayer()->SendDirectMessage(&data);
}

void SpellHistory::SendCooldownEvent(SpellInfo const* spellInfo, uint32 itemId, Spell* spell)
{
    Player* player = GetPlayer();
    if (!player)
        return;

    StartCooldownOnEvent(spellInfo, itemId, spell);

    ObjectGuid guid = _owner->GetGUID();

    WorldPacket data(SMSG_COOLDOWN_EVENT, 4 + 8);

    data.WriteBit(guid[4]);
data.WriteBit(guid[7]);
data.WriteBit(guid[1]);
data.WriteBit(guid[5]);
data.WriteBit(guid[6]);
data.WriteBit(guid[0]);
data.WriteBit(guid[2]);
data.WriteBit(guid[3]);
    data.WriteByteSeq(guid[5]);
data.WriteByteSeq(guid[7]);
    data << uint32(spellInfo->Id);
    data.WriteByteSeq(guid[3]);
data.WriteByteSeq(guid[1]);
data.WriteByteSeq(guid[2]);
data.WriteByteSeq(guid[4]);
data.WriteByteSeq(guid[6]);
data.WriteByteSeq(guid[0]);

    player->SendDirectMessage(&data);
}

uint32 SpellHistory::GetCooldownDelay(uint32 spellId, uint32 itemId) const
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
        return 0;
    auto it = _cooldowns.find(spellId);
    if (it == _cooldowns.end())
        return 0;

    auto& entry = it->second;
    auto now = TimeValue::Now();
    if (entry.CooldownEnd > now && (!itemId || itemId == itemId))
        return (entry.CooldownEnd - now).ToMilliseconds();
    if (entry.CategoryEnd > now && (!itemId || itemId == itemId))
        return (entry.CategoryEnd - now).ToMilliseconds();

    return 0;
}

TimeValue SpellHistory::GetChargeCooldown(uint32 spellId)
{
    TimeValue cooldown;

    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
        return cooldown;

    auto categories = spellInfo->GetSpellCategories();
    if (!categories || !categories->ChargesCategory)
        return cooldown;

    auto category = sSpellCategoryStore.LookupEntry(categories->ChargesCategory);
    if (!category)
        return cooldown;

    auto itr = _spellCharges.find(category->Id);
    if (itr != _spellCharges.end())
    {
        auto now = TimeValue::Now();
        TimeValue resetTime = itr->second.CurrentResetTime;

        if (now < resetTime)
            cooldown = resetTime - now;
    }
    return cooldown;
}

void SpellHistory::RppmProcAttempt(uint32 spellId, WeaponAttackType attackType)
{
    auto bound = _procHistory.equal_range(spellId);
    for (auto itr = bound.first; itr != bound.second; ++itr)
    {
        if (itr->second.AttackTyep == attackType)
        {
            itr->second.LastAttempt = TimeValue::Now();
            return;
        }
    }
    RppmProcEntry entry;
    entry.LastAttempt = TimeValue::Now();
    entry.AttackTyep = attackType;
    _procHistory.emplace(spellId, entry);
}

void SpellHistory::RppmPorcSuccess(uint32 spellId, WeaponAttackType attackType)
{
    auto bound = _procHistory.equal_range(spellId);
    for (auto itr = bound.first; itr != bound.second; ++itr)
    {
        if (itr->second.AttackTyep == attackType)
        {
            itr->second.LastSuccess = TimeValue::Now();
            return;
        }
    }
    RppmProcEntry entry;
    entry.LastSuccess = TimeValue::Now();
    entry.AttackTyep = attackType;
    _procHistory.emplace(spellId, entry);
}

SpellHistory::RppmProcEntry const* SpellHistory::GetRppmProcEntry(uint32 spellId, WeaponAttackType attackType)
{
    auto bound = _procHistory.equal_range(spellId);
    for (auto itr = bound.first; itr != bound.second; ++itr)
        if (itr->second.AttackTyep == attackType)
            return &itr->second;

    return nullptr;
}

void SpellHistory::UpdateCharges()
{
    auto now = TimeValue::Now();
    for (auto it = _spellCharges.begin(); it != _spellCharges.end();)
    {
        auto& chargeData = it->second;
        bool erase = false;
        while (chargeData.CurrentResetTime <= now)
        {
            // If we replenish last consumed charge, simply erase charge data
            if (--chargeData.ConsumedCharges == 0)
            {
                erase = true;
                break;
            }
            // Otherwise, we have to start countdown again
            chargeData.CurrentResetTime += chargeData.BaseRegenTime;
        }
        if (erase)
        {
            ClearChargesForSpell(it->first);
            _spellCharges.erase(it++);
        }
        else
            ++it;
    }
}

void SpellHistory::SendSpellHistory() const
{
    ASSERT(_owner->GetTypeId() == TYPEID_PLAYER);

    auto now = TimeValue::Now();

    WorldPacket data(SMSG_SEND_SPELL_HISTORY);
    data.WriteBits(_cooldowns.size(), 19);
    for (auto&& it : _cooldowns)
        data.WriteBit(0);

    data.FlushBits();

    for (auto&& it : _cooldowns)
    {
        auto& entry = it.second;
        if (entry.OnHold)
        {
            data << uint32(0x80000000);                         // category cooldown
            data << uint32(entry.ItemId);                       // item entry
            data << uint32(1);                                  // cooldown
        }
        else
        {
            auto cooldown = entry.CooldownEnd > now ? (entry.CooldownEnd - now).ToMilliseconds() : 0;
            auto categoryCooldown = entry.CategoryEnd > now ? (entry.CategoryEnd - now).ToMilliseconds() : 0;

            data << uint32(categoryCooldown);                   // category cooldown
            data << uint32(entry.ItemId);                       // item entry
            data << uint32(cooldown);                           // cooldown
        }
        data << uint32(entry.CategoryId);                       // category
        data << uint32(entry.SpellId);                          // spell entry
    }
    GetPlayer()->SendDirectMessage(&data);

    SendSpellCharges();
}

void SpellHistory::OnEncounterStart()
{
    auto now = TimeValue::Now();
    TimeValue diff = Seconds(120);
    for (auto&& itr : _procHistory)
        if (itr.second.LastSuccess >= diff)
            itr.second.LastSuccess -= diff;
}

Player* SpellHistory::GetPlayer() const
{
    if (_owner->GetTypeId() == TYPEID_PLAYER)
        return _owner->ToPlayer();

    if (_owner->HasUnitTypeMask(UNIT_MASK_CONTROLABLE_GUARDIAN))
        return _owner->GetAffectingPlayer();

    return nullptr;
}

void SpellHistory::SendClearCooldowns(std::vector<uint32> const& cooldowns)
{
    uint32 size = cooldowns.size();
    ObjectGuid guid = _owner->GetGUID();
    WorldPacket data(SMSG_CLEAR_COOLDOWNS, 1 + 8 + 3 + (size * 4));
    data.WriteBit(guid[5]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[2]);
    data.WriteBits(size, 22);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[4]);
    data.FlushBits();

    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[6]);

    for (auto&& it : cooldowns)
        data << uint32(it);

    data.WriteByteSeq(guid[2]);

    GetPlayer()->SendDirectMessage(&data);
}

void SpellHistory::SendSpellCharges() const
{
    WorldPacket data{ SMSG_SEND_SPELL_CHARGES };
    data.WriteBits(0, 21);
    data.FlushBits();

    auto now = TimeValue::Now();

    uint32 count = 0;

    for (auto&& itr : _spellCharges)
    {
        uint8 usedCharges = itr.second.ConsumedCharges;
        TimeValue resetTime = itr.second.CurrentResetTime;
        while (usedCharges && resetTime <= now)
        {
            --usedCharges;
            resetTime += itr.second.BaseRegenTime;
        }

        if (!usedCharges)
            continue;

        ++count;

        data << int32((resetTime - now).ToMilliseconds());
        data << uint8(usedCharges);
        data << uint32(itr.first);
    }

    data.PutBits(0, count, 21);

    _owner->ToPlayer()->SendDirectMessage(&data);
}

SpellHistory::CooldownInfo SpellHistory::GetCooldownInfo(SpellInfo const* spellInfo, uint32 itemId) const
{
    // some special item spells without correct cooldown in SpellInfo
    // cooldown information stored in item prototype
    // This used in same way in WorldSession::HandleItemQuerySingleOpcode data sending to client.
    if (itemId)
    {
        if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemId))
        {
            for (uint8 idx = 0; idx < MAX_ITEM_SPELLS; ++idx)
            {
                if (uint32(proto->Spells[idx].SpellId) == spellInfo->Id)
                {
                    return {
                        proto->Spells[idx].SpellCooldown,
                        proto->Spells[idx].SpellCategory,
                        proto->Spells[idx].SpellCategoryCooldown
                    };
                }
            }
        }
    }
    
    return { spellInfo->RecoveryTime, spellInfo->GetCategory(), spellInfo->CategoryRecoveryTime };
}

void SpellHistory::ClearAllSpellCharges()
{
    ObjectGuid guid = GetPlayer()->GetGUID();
    WorldPacket data(SMSG_CLEAR_ALL_SPELL_CHARGES);
    data.WriteBit(guid[0]);
data.WriteBit(guid[2]);
data.WriteBit(guid[5]);
data.WriteBit(guid[4]);
data.WriteBit(guid[3]);
data.WriteBit(guid[7]);
data.WriteBit(guid[6]);
data.WriteBit(guid[1]);
    data.WriteByteSeq(guid[6]);
data.WriteByteSeq(guid[0]);
data.WriteByteSeq(guid[1]);
data.WriteByteSeq(guid[7]);
data.WriteByteSeq(guid[3]);
data.WriteByteSeq(guid[2]);
data.WriteByteSeq(guid[5]);
data.WriteByteSeq(guid[4]);
    GetPlayer()->SendDirectMessage(&data);
}

void SpellHistory::ClearChargesForSpell(uint32 spell)
{
    ObjectGuid guid = GetPlayer()->GetGUID();
    WorldPacket data(SMSG_CLEAR_SPELL_CHARGES);
    data.WriteBit(guid[6]);
data.WriteBit(guid[0]);
data.WriteBit(guid[2]);
data.WriteBit(guid[7]);
data.WriteBit(guid[5]);
data.WriteBit(guid[4]);
data.WriteBit(guid[3]);
data.WriteBit(guid[1]);
    data.WriteByteSeq(guid[7]);
data.WriteByteSeq(guid[6]);
data.WriteByteSeq(guid[4]);
    data << uint32(spell);
    data.WriteByteSeq(guid[1]);
data.WriteByteSeq(guid[3]);
data.WriteByteSeq(guid[2]);
data.WriteByteSeq(guid[0]);
data.WriteByteSeq(guid[5]);
    GetPlayer()->SendDirectMessage(&data);
}
