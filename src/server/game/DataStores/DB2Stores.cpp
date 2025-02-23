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

#include "BattlePet.h"
#include "DB2Stores.h"
#include "DB2fmt.h"
#include "DB2Utility.h"
#include "Common.h"
#include "Hash.h"
#include "Log.h"
#include "World.h"
#include "ObjectMgr.h"

DB2Storage<BattlePetAbilityEntry> sBattlePetAbilityStore("BattlePetAbility.db2", BattlePetAbilityfmt);
DB2Storage<BattlePetAbilityStateEntry> sBattlePetAbilityStateStore("BattlePetAbilityState.db2", BattlePetAbilityStatefmt);
DB2Storage<BattlePetAbilityEffectEntry> sBattlePetAbilityEffectStore("BattlePetAbilityEffect.db2", BattlePetAbilityEffectfmt);
DB2Storage<BattlePetAbilityTurnEntry> sBattlePetAbilityTurnStore("BattlePetAbilityTurn.db2", BattlePetAbilityTurnfmt);
DB2Storage<BattlePetBreedQualityEntry> sBattlePetBreedQualityStore("BattlePetBreedQuality.db2", BattlePetBreedQualityfmt);
DB2Storage<BattlePetBreedStateEntry> sBattlePetBreedStateStore("BattlePetBreedState.db2", BattlePetBreedStatefmt);
DB2Storage<BattlePetSpeciesEntry> sBattlePetSpeciesStore("BattlePetSpecies.db2", BattlePetSpeciesfmt);
DB2Storage<BattlePetSpeciesStateEntry> sBattlePetSpeciesStateStore("BattlePetSpeciesState.db2", BattlePetSpeciesStatefmt);
DB2Storage<BattlePetSpeciesXAbilityEntry> sBattlePetSpeciesXAbilityStore("BattlePetSpeciesXAbility.db2", BattlePetSpeciesXAbilityfmt);
DB2Storage<BattlePetStateEntry> sBattlePetStateStore("BattlePetState.db2", BattlePetStatefmt);
DB2Storage<ItemToBattlePetEntry> sItemToBattlePetStore("ItemToBattlePet.db2", ItemToBattlePetfmt);

BattlePetBreedSet sBattlePetBreedSet;
BattlePetItemXSpeciesStore sBattlePetItemXSpeciesStore;

DB2Storage<BroadcastTextEntry> sBroadcastTextStore("BroadcastText.db2", BroadcastTextfmt);
DB2Storage<ItemEntry> sItemStore("Item.db2", Itemfmt);
DB2Storage<ItemCurrencyCostEntry> sItemCurrencyCostStore("ItemCurrencyCost.db2", ItemCurrencyCostfmt);
DB2Storage<ItemExtendedCostEntry> sItemExtendedCostStore("ItemExtendedCost.db2", ItemExtendedCostEntryfmt);
DB2Storage<ItemSparseEntry> sItemSparseStore("Item-sparse.db2", ItemSparsefmt);
DB2Storage<ItemUpgradeEntry> sItemUpgradeStore("ItemUpgrade.db2", ItemUpgradeEntryfmt);
DB2Storage<ItemToMountSpellEntry> sItemItemToMountSpellStore("ItemToMountSpell.db2", ItemItemToMountSpellEntryfmt);
DB2Storage<KeyChainEntry> sKeyChainStore("KeyChain.db2", KeyChainfmt, WORLD_SEL_KEY_CHAIN);
DB2Storage<MapChallengeModeEntry> sMapChallengeModeStore("MapChallengeMode.db2", MapChallengeModeEntryfmt);
DB2Storage<QuestPackageItemEntry> sQuestPackageItemStore("QuestPackageItem.db2", QuestPackageItemfmt);
DB2Storage<RulesetItemUpgradeEntry> sRulesetItemUpgradeStore("RulesetItemUpgrade.db2", RulesetItemUpgradeEntryfmt);
DB2Storage<SceneScriptEntry> sSceneScriptStore("SceneScript.db2", SceneScriptfmt);
DB2Storage<SceneScriptPackageEntry> sSceneScriptPackageStore("SceneScriptPackage.db2", SceneScriptPackagefmt);
DB2Storage<SpellReagentsEntry> sSpellReagentsStore("SpellReagents.db2", SpellReagentsfmt);
DB2Storage<VignetteEntry> sVignetteStore("Vignette.db2", VignetteEntryfmt);

typedef std::unordered_map<uint32, std::pair<std::vector<QuestPackageItemEntry const*>, std::vector<QuestPackageItemEntry const*>>> QuestPackageItemContainer;

typedef std::map<uint32 /*hash*/, DB2StorageBase*> DB2StorageMap;
DB2StorageMap DB2Stores;

typedef std::list<std::string> DB2StoreProblemList;

DB2Manager& DB2Manager::Instance()
{
    static DB2Manager instance;
    return instance;
}

uint32 DB2FilesCount = 0;

struct QuestPackageItem
{
    uint32 ItemId;
    uint32 Count;
};

static std::multimap<uint32, QuestPackageItem> sQuestPackageItemMap;
static std::map<uint32, uint32> sItemUpgradeIdMap;
static std::multimap<uint32, uint32> sMountSpellToItemMap;


std::map<uint32, uint32> sBattlePetSpellXSpeciesStore;

static std::map<uint32, BattlePetAbilities> sBattlePetSpeciesXAbilityMap;

namespace
{
    QuestPackageItemContainer _questPackages;
}


template<class T>
inline void LoadDB2(uint32& availableDb2Locales, DB2StoreProblemList& errlist, DB2Manager::StorageMap& stores, DB2Storage<T>* storage, std::string const& db2Path, uint32 defaultLocale)
{
    // compatibility format and C++ structure sizes
    ASSERT(DB2FileLoader::GetFormatRecordSize(storage->GetFormat()) == sizeof(T),
        "Size of '%s' set by format string (%u) not equal size of C++ structure (" SZFMTD ").",
        storage->GetFileName().c_str(), DB2FileLoader::GetFormatRecordSize(storage->GetFormat()), sizeof(T));

    ++DB2FilesCount;

    if (storage->Load(db2Path + localeNames[defaultLocale] + '/', defaultLocale))
    {
        storage->LoadFromDB();
        // LoadFromDB() always loads strings into enUS locale, other locales are expected to have data in corresponding _locale tables
        // so we need to make additional call to load that data in case said locale is set as default by worldserver.conf (and we do not want to load all this data from .db2 file again)
        if (defaultLocale != LOCALE_enUS)
            storage->LoadStringsFromDB(LocaleConstant(defaultLocale));

        for (uint32 i = 0; i < TOTAL_LOCALES; ++i)
        {
            if (defaultLocale == i) // || i == LOCALE_NONE
                continue;

            if (availableDb2Locales & (1 << i))
                if (!storage->LoadStringsFrom((db2Path + localeNames[i] + '/'), i))
                    availableDb2Locales &= ~(1 << i);             // mark as not available for speedup next checks

            storage->LoadStringsFromDB(LocaleConstant(i));
        }
    }
    else
    {
        // sort problematic db2 to (1) non compatible and (2) nonexistent
        if (FILE* f = fopen((db2Path + storage->GetFileName()).c_str(), "rb"))
        {
            std::ostringstream stream;
            stream << storage->GetFileName() << " exists, and has " << storage->GetFieldCount() << " field(s) (expected " << strlen(storage->GetFormat())
                << "). Extracted file might be from wrong client version.";
            std::string buf = stream.str();
            errlist.push_back(buf);
            fclose(f);
        }
        else
            errlist.push_back(storage->GetFileName());
    }

    stores[storage->GetHash()] = storage;
}

uint32 DB2Manager::LoadStores(std::string const& dataPath, uint32 defaultLocale)
{
    uint32 oldMSTime = getMSTime();

    std::string db2Path = dataPath + "dbc/";

    DB2StoreProblemList bad_db2_files;
    uint32 availableDb2Locales = 0xFFFFFFFF;

    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sBattlePetAbilityStore, db2Path, defaultLocale);
    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sBattlePetAbilityStateStore, db2Path, defaultLocale);
    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sBattlePetAbilityEffectStore, db2Path, defaultLocale);
    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sBattlePetAbilityTurnStore, db2Path, defaultLocale);
    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sBattlePetBreedQualityStore, db2Path, defaultLocale);
    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sBattlePetBreedStateStore, db2Path, defaultLocale);

    for (uint32 i = 0; i < sBattlePetBreedStateStore.GetNumRows(); i++)
        if (BattlePetBreedStateEntry const* breedStateEntry = sBattlePetBreedStateStore.LookupEntry(i))
            if (sBattlePetBreedSet.find(breedStateEntry->BreedId) == sBattlePetBreedSet.end())
                sBattlePetBreedSet.insert(breedStateEntry->BreedId);

    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sBattlePetSpeciesStore, db2Path, defaultLocale);
    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sBattlePetSpeciesStateStore, db2Path, defaultLocale);
    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sBattlePetSpeciesXAbilityStore, db2Path, defaultLocale);
    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sBattlePetStateStore, db2Path, defaultLocale);
    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sItemToBattlePetStore, db2Path, defaultLocale);

    for (uint32 i = 0; i < sItemToBattlePetStore.GetNumRows(); i++)
        if (ItemToBattlePetEntry const* itemEntry = sItemToBattlePetStore.LookupEntry(i))
            sBattlePetItemXSpeciesStore.insert(std::make_pair(itemEntry->ItemId, itemEntry->SpeciesId));

    for (uint32 i = 0; i < sBattlePetSpeciesXAbilityStore.GetNumRows(); ++i)
    {
        if (auto entry = sBattlePetSpeciesXAbilityStore.LookupEntry(i))
        {
            auto& abilities = sBattlePetSpeciesXAbilityMap[entry->SpeciesId][entry->SlotId];
            if (!abilities.FirstTier)
                abilities.FirstTier = entry;
            else
            {
                // Where are some unused shit where all 6 abilities have slot 0, just ignore that
                abilities.SecondTier = entry;
                if (abilities.FirstTier->RequiredLevel > abilities.SecondTier->RequiredLevel)
                    std::swap(abilities.FirstTier, abilities.SecondTier);
            }
        }
    }

    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sBroadcastTextStore, db2Path, defaultLocale);
    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sItemStore, db2Path, defaultLocale);
    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sItemCurrencyCostStore, db2Path, defaultLocale);
    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sItemSparseStore, db2Path, defaultLocale);
    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sItemUpgradeStore, db2Path, defaultLocale);
    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sItemItemToMountSpellStore, db2Path, defaultLocale);

    for (uint32 i = 0; i < sItemItemToMountSpellStore.GetNumRows(); ++i)
        if (auto entry = sItemItemToMountSpellStore.LookupEntry(i))
            sMountSpellToItemMap.emplace(entry->SpellId, entry->ItemId);

    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sItemExtendedCostStore, db2Path, defaultLocale);
    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sKeyChainStore, db2Path, defaultLocale);
    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sMapChallengeModeStore, db2Path, defaultLocale);
    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sQuestPackageItemStore, db2Path, defaultLocale);
    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sRulesetItemUpgradeStore, db2Path, defaultLocale);
    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sSceneScriptStore, db2Path, defaultLocale);
    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sSceneScriptPackageStore, db2Path, defaultLocale);
    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sSpellReagentsStore, db2Path, defaultLocale);
    LoadDB2(availableDb2Locales, bad_db2_files, _stores, &sVignetteStore, db2Path, defaultLocale);

    for (uint32 i = 0; i < sRulesetItemUpgradeStore.GetNumRows(); ++i)
    {
        if (auto entry = sRulesetItemUpgradeStore.LookupEntry(i))
        {
            if (entry->RulesetID != 1)
                continue;

            uint32 upgradeId = entry->ItemUpgradeID;
            ItemUpgradeEntry const* upgrade = sItemUpgradeStore.LookupEntry(upgradeId);
            while (upgrade && upgrade->PrevItemUpgradeID)
            {
                upgradeId = upgrade->PrevItemUpgradeID;
                upgrade = sItemUpgradeStore.LookupEntry(upgradeId);
            }
            sItemUpgradeIdMap[entry->ItemID] = upgradeId;
        }
    }

    for (size_t i = 0; i < sQuestPackageItemStore.GetNumRows(); ++i)
    {
        if (auto entry = sQuestPackageItemStore.LookupEntry(i))
            sQuestPackageItemMap.insert({ entry->PackageID, { entry->ItemID, entry->ItemQuantity} });
    }

    for (QuestPackageItemEntry const* questPackageItem : sQuestPackageItemStore)
    {
        if (questPackageItem->DisplayType != QUEST_PACKAGE_FILTER_UNMATCHED)
            _questPackages[questPackageItem->PackageID].first.push_back(questPackageItem);
        else
            _questPackages[questPackageItem->PackageID].second.push_back(questPackageItem);
    }

    // error checks
    if (bad_db2_files.size() >= DB2FilesCount)
    {
        TC_LOG_ERROR("misc", "\nIncorrect DataDir value in worldserver.conf or ALL required *.db2 files (%d) not found by path: %sdbc", DB2FilesCount, dataPath.c_str());
        exit(1);
    }
    else if (!bad_db2_files.empty())
    {
        std::string str;
        for (std::list<std::string>::iterator i = bad_db2_files.begin(); i != bad_db2_files.end(); ++i)
            str += *i + "\n";

        TC_LOG_ERROR("misc", "\nSome required *.db2 files (%u from %d) not found or not compatible:\n%s", (uint32)bad_db2_files.size(), DB2FilesCount, str.c_str());
        exit(1);
    }

    // Check loaded DB2 files proper version
    if (!sBattlePetAbilityStore.LookupEntry(1238)           // last battle pet ability added in 5.4.8 (18414)
        || !sBattlePetSpeciesStore.LookupEntry(1386)        // last battle pet species added in 5.4.8 (18414)
        || !sBattlePetStateStore.LookupEntry(176)           // last battle pet state added in 5.4.8 (18414)
      //|| !sItemToBattlePetStore.LookupEntry(109014)       // last battle pet item added in 5.4.8 (18414) without key, skip
        || !sBroadcastTextStore.LookupEntry(77161)          // last broadcast text added in 5.4.8 (18414)
        || !sItemStore.LookupEntry(112353)                  // last item added in 5.4.8 (18414)
        || !sItemExtendedCostStore.LookupEntry(5280)        // last item extended cost added in 5.4.8 (18414)
        || !sQuestPackageItemStore.LookupEntry(2256)        // last quest package item in 5.4.8 (18414)
        || !sSceneScriptStore.LookupEntry(11156))           // last scene script added in 5.4.8 (18414)
    {
        TC_LOG_ERROR("misc", "You have _outdated_ DB2 files, Please extract correct db2 files from client 5.4.8 18414.");
        exit(1);
    }

    TC_LOG_INFO("server.loading", ">> Initialized %d DB2 data stores in %u ms", DB2FilesCount, GetMSTimeDiffToNow(oldMSTime));

    return availableDb2Locales;
}

DB2StorageBase const* DB2Manager::GetStorage(uint32 type) const
{
    StorageMap::const_iterator itr = _stores.find(type);
    if (itr != _stores.end())
        return itr->second;

    return nullptr;
}

#define BATTLE_PET_MAIN_STAT_VALUE  8.0f
#define BATTLE_PET_MAIN_STAT_DIV    200.0f
#define BATTLE_PET_MAIN_STAT_OFFSET 1600.0f

float BattlePetSpeciesMainStat(uint16 stateId, uint16 speciesId)
{
    if (stateId != BATTLE_PET_STATE_STAT_POWER
        && stateId != BATTLE_PET_STATE_STAT_STAMINA
        && stateId != BATTLE_PET_STATE_STAT_SPEED)
        return 0;

    for (uint32 i = 0; i < sBattlePetSpeciesStateStore.GetNumRows(); i++)
    {
        BattlePetSpeciesStateEntry const* stateEntry = sBattlePetSpeciesStateStore.LookupEntry(i);
        if (!stateEntry)
            continue;

        if (stateEntry->StateId == stateId && stateEntry->SpeciesId == speciesId)
            return BATTLE_PET_MAIN_STAT_VALUE + ((float)stateEntry->Modifier / BATTLE_PET_MAIN_STAT_DIV);
    }

    return BATTLE_PET_MAIN_STAT_VALUE;
}

float BattlePetBreedMainStatModifier(uint16 stateId, uint8 breedId)
{
    if (stateId != BATTLE_PET_STATE_STAT_POWER
        && stateId != BATTLE_PET_STATE_STAT_STAMINA
        && stateId != BATTLE_PET_STATE_STAT_SPEED)
        return 0;

    for (uint32 i = 0; i < sBattlePetBreedStateStore.GetNumRows(); i++)
    {
        BattlePetBreedStateEntry const* stateEntry = sBattlePetBreedStateStore.LookupEntry(i);
        if (!stateEntry)
            continue;

        if (stateEntry->StateId == stateId && stateEntry->BreedId == breedId)
            return ((float)stateEntry->Modifier - BATTLE_PET_MAIN_STAT_OFFSET) / BATTLE_PET_MAIN_STAT_DIV;
    }

    return 0;
}

void db2::LoadBattlePetCorrections()
{
    for (uint32 i = 0; i < sItemToBattlePetStore.GetNumRows(); ++i)
    {
        if (auto itemToSpecies = sItemToBattlePetStore.LookupEntry(i))
        {
            auto item = sObjectMgr->GetItemTemplate(itemToSpecies->ItemId);
            if (!item)
                continue;

            auto battlePet = sBattlePetSpeciesStore.LookupEntry(itemToSpecies->SpeciesId);
            if (!battlePet)
                continue;

            auto spell = sSpellMgr->GetSpellInfo(item->Spells[1].SpellId);
            if (!spell)
                continue;

            if (spell->Id == 154165) // Moonkin Hatchling, 2 summons, ignore, it's okay
                continue;

            // Some entries have invalid spell id. Take it form items.
            // ...hotfixes? Who cares
            if (spell->Effects[EFFECT_0].Effect == SPELL_EFFECT_SUMMON && spell->Effects[EFFECT_0].MiscValue == battlePet->NpcId)
                const_cast<BattlePetSpeciesEntry*>(battlePet)->SpellId = spell->Id;
        }
    }

    for (uint32 i = 0; i < sBattlePetSpeciesStore.GetNumRows(); i++)
    {
        auto speciesEntry = sBattlePetSpeciesStore.LookupEntry(i);
        if (!speciesEntry)
            continue;

        sBattlePetSpellXSpeciesStore[speciesEntry->SpellId] = speciesEntry->SpeciesId;
    }
}

uint32 db2::GetBattlePetSummonSpell(uint32 speciesId)
{
    BattlePetSpeciesEntry const* speciesEntry = sBattlePetSpeciesStore.LookupEntry(speciesId);
    if (!speciesEntry)
        return 0;

    return speciesEntry->SpellId;
}

bool HasBattlePetSpeciesFlag(uint16 speciesId, uint16 flag)
{
    BattlePetSpeciesEntry const* speciesEntry = sBattlePetSpeciesStore.LookupEntry(speciesId);
    if (!speciesEntry)
        return false;

    return (speciesEntry->Flags & flag) != 0;
}

uint32 db2::GetBattlePetSpeciesFromSpell(uint32 spellId)
{
    auto it = sBattlePetSpellXSpeciesStore.find(spellId);
    if (it == sBattlePetSpellXSpeciesStore.end())
        return 0;

    return it->second;
}

uint32 db2::GetBattlePetItem(uint32 species)
{
    for (uint32 i = 0; i < sItemToBattlePetStore.GetNumRows(); ++i)
        if (auto entry = sItemToBattlePetStore.LookupEntry(i))
            if (entry->ItemId == species)
                return entry->ItemId;
    return 0;
}

BattlePetAbilities const* db2::GetAbilitiesForPetSpecies(uint32 speciesId)
{
    auto itr = sBattlePetSpeciesXAbilityMap.find(speciesId);
    if (itr == sBattlePetSpeciesXAbilityMap.end())
        return nullptr;
    return &itr->second;
}

bool GetQuestRewardItemCountFromPackage(uint32 itemid, uint32 package, uint32& count)
{
    auto pair = sQuestPackageItemMap.equal_range(package);
    for (auto it = pair.first; it != pair.second; ++it)
    {
        if (it->second.ItemId == itemid)
        {
            count = it->second.Count;
            return true;
        }
    }
    return false;
}

uint32 GetUpgradeId(uint32 itemId)
{
    auto it = sItemUpgradeIdMap.find(itemId);
    if (it == sItemUpgradeIdMap.end())
        return 0;
    return it->second;
}

bool IsMountCanBeAllowedForPlayer(uint32 spellId, uint32 raceMask)
{
    if (sMountSpellToItemMap.find(spellId) == sMountSpellToItemMap.end())
        return true;

    auto pair = sMountSpellToItemMap.equal_range(spellId);
    for (auto it = pair.first; it != pair.second; ++it)
    {
        if (ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(it->second))
        {
            if (itemTemplate->AllowableRace && !(itemTemplate->AllowableRace & raceMask))
                continue;

            if (itemTemplate->Flags2 & ITEM_FLAGS_EXTRA_HORDE_ONLY && raceMask & RACEMASK_ALLIANCE)
                continue;

            if (itemTemplate->Flags2 & ITEM_FLAGS_EXTRA_ALLIANCE_ONLY && raceMask & RACEMASK_HORDE)
                continue;

            return true;
        }
    }
    return false;
}

std::vector<QuestPackageItemEntry const*> const* DB2Manager::GetQuestPackageItems(uint32 questPackageID) const
{
    auto itr = _questPackages.find(questPackageID);
    if (itr != _questPackages.end())
        return &itr->second.first;

    return nullptr;
}

std::vector<QuestPackageItemEntry const*> const* DB2Manager::GetQuestPackageItemsFallback(uint32 questPackageID) const
{
    auto itr = _questPackages.find(questPackageID);
    if (itr != _questPackages.end())
        return &itr->second.second;

    return nullptr;
}
