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

#ifndef TRINITY_DB2STORES_H
#define TRINITY_DB2STORES_H

#include "DB2Store.h"
#include "DB2Structure.h"
#include <map>
#include <string>

TC_GAME_API extern DB2Storage<BattlePetAbilityEntry> sBattlePetAbilityStore;
TC_GAME_API extern DB2Storage<BattlePetAbilityStateEntry> sBattlePetAbilityStateStore;
TC_GAME_API extern DB2Storage<BattlePetAbilityEffectEntry> sBattlePetAbilityEffectStore;
TC_GAME_API extern DB2Storage<BattlePetAbilityTurnEntry> sBattlePetAbilityTurnStore;
TC_GAME_API extern DB2Storage<BattlePetBreedQualityEntry> sBattlePetBreedQualityStore;
TC_GAME_API extern DB2Storage<BattlePetBreedStateEntry> sBattlePetBreedStateStore;
TC_GAME_API extern DB2Storage<BattlePetSpeciesEntry> sBattlePetSpeciesStore;
TC_GAME_API extern DB2Storage<BattlePetSpeciesStateEntry> sBattlePetSpeciesStateStore;
TC_GAME_API extern DB2Storage<BattlePetSpeciesXAbilityEntry> sBattlePetSpeciesXAbilityStore;
TC_GAME_API extern DB2Storage<BattlePetStateEntry> sBattlePetStateStore;
TC_GAME_API extern DB2Storage<ItemToBattlePetEntry> sItemToBattlePetStore;

TC_GAME_API extern BattlePetBreedSet sBattlePetBreedSet;
TC_GAME_API extern BattlePetItemXSpeciesStore sBattlePetItemXSpeciesStore;

TC_GAME_API extern DB2Storage<BroadcastTextEntry> sBroadcastTextStore;
TC_GAME_API extern DB2Storage<ItemEntry> sItemStore;
TC_GAME_API extern DB2Storage<ItemCurrencyCostEntry> sItemCurrencyCostStore;
TC_GAME_API extern DB2Storage<ItemExtendedCostEntry> sItemExtendedCostStore;
TC_GAME_API extern DB2Storage<ItemSparseEntry> sItemSparseStore;
TC_GAME_API extern DB2Storage<ItemUpgradeEntry> sItemUpgradeStore;
TC_GAME_API extern DB2Storage<ItemToMountSpellEntry> sItemItemToMountSpellStore;
TC_GAME_API extern DB2Storage<KeyChainEntry> sKeyChainStore;
TC_GAME_API extern DB2Storage<MapChallengeModeEntry> sMapChallengeModeStore;
TC_GAME_API extern DB2Storage<QuestPackageItemEntry> sQuestPackageItemStore;
TC_GAME_API extern DB2Storage<RulesetItemUpgradeEntry> sRulesetItemUpgradeStore;
TC_GAME_API extern DB2Storage<SceneScriptEntry> sSceneScriptStore;
TC_GAME_API extern DB2Storage<SceneScriptPackageEntry> sSceneScriptPackageStore;
TC_GAME_API extern DB2Storage<SpellReagentsEntry> sSpellReagentsStore;
TC_GAME_API extern DB2Storage<VignetteEntry> sVignetteStore;

float BattlePetSpeciesMainStat(uint16 stateId, uint16 speciesId);
float BattlePetBreedMainStatModifier(uint16 stateId, uint8 speciesId);
bool HasBattlePetSpeciesFlag(uint16 species, uint16 flag);

struct BattlePetAbilityPair
{
    BattlePetSpeciesXAbilityEntry const* FirstTier = nullptr;
    BattlePetSpeciesXAbilityEntry const* SecondTier = nullptr;
};

typedef std::array<BattlePetAbilityPair, 3> BattlePetAbilities;

namespace db2
{
    void LoadBattlePetCorrections();
    uint32 GetBattlePetSummonSpell(uint32 speciesId);
    uint32 GetBattlePetSpeciesFromSpell(uint32 spellId);
    uint32 GetBattlePetItem(uint32 species);
    BattlePetAbilities const* GetAbilitiesForPetSpecies(uint32 speciesId);
}

bool GetQuestRewardItemCountFromPackage(uint32 itemid, uint32 package, uint32& count);
uint32 GetUpgradeId(uint32 itemId);
bool IsMountCanBeAllowedForPlayer(uint32 spellId, uint32 raceMask);

struct HotfixNotify
{
    uint32 TableHash;
    uint32 Timestamp;
    uint32 Entry;
};

typedef std::vector<HotfixNotify> HotfixData;

class TC_GAME_API DB2Manager
{
public:
    typedef std::map<uint32 /*hash*/, DB2StorageBase*> StorageMap;

    static DB2Manager& Instance();

    uint32 LoadStores(std::string const& dataPath, uint32 defaultLocale);
    DB2StorageBase const* GetStorage(uint32 type) const;

    void LoadHotfixData();
    HotfixData const* GetHotfixData() const { return &_hotfixData; }

    std::vector<QuestPackageItemEntry const*> const* GetQuestPackageItems(uint32 questPackageID) const;
    std::vector<QuestPackageItemEntry const*> const* GetQuestPackageItemsFallback(uint32 questPackageID) const;

private:
    StorageMap _stores;
    HotfixData _hotfixData;
};

#define sDB2Manager DB2Manager::Instance()

#endif
