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

#include "ItemEnchantmentMgr.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "ObjectMgr.h"
#include <list>
#include <vector>
#include "Util.h"

struct EnchStoreItem
{
    uint32  ench;
    float   chance;

    EnchStoreItem()
        : ench(0), chance(0) { }

    EnchStoreItem(uint32 _ench, float _chance)
        : ench(_ench), chance(_chance) { }
};

typedef std::vector<EnchStoreItem> EnchStoreList;
typedef std::unordered_map<uint32, EnchStoreList> EnchantmentStore;

static EnchantmentStore RandomPropertyItemEnch;
static EnchantmentStore RandomSuffixItemEnch;

void LoadRandomEnchantmentsTable()
{
    uint32 oldMSTime = getMSTime();

    RandomPropertyItemEnch.clear();                                 // for reload case
    RandomSuffixItemEnch.clear();

    //                                                 0      1      2     3
    QueryResult result = WorldDatabase.Query("SELECT entry, ench, chance, type FROM item_enchantment_template");

    if (result)
    {
        uint32 count = 0;

        do
        {
            Field* fields = result->Fetch();

            uint32 entry = fields[0].GetUInt32();
            uint32 ench = fields[1].GetUInt32();
            float chance = fields[2].GetFloat();
            ItemRandomEnchantmentType type = ItemRandomEnchantmentType(fields[3].GetUInt8());

            if (chance < 0.000001f || chance > 100.0f)
            {
                TC_LOG_ERROR("sql.sql", "Random item enchantment for entry %u type %u ench %u has invalid chance %f", entry, uint32(type), ench, chance);
                continue;
            }

            switch (type)
            {
                case ItemRandomEnchantmentType::Property:
                    RandomPropertyItemEnch[entry].push_back(EnchStoreItem(ench, chance));
                    break;
                case ItemRandomEnchantmentType::Suffix:
                    RandomSuffixItemEnch[entry].push_back(EnchStoreItem(ench, chance));
                    break;
                default:
                    TC_LOG_ERROR("sql.sql", "Invalid random enchantment type specified in `item_enchantment_template` table for `entry` %u `ench` %u", entry, ench);
                    break;
            }

            ++count;
        } while (result->NextRow());

        TC_LOG_INFO("server.loading", ">> Loaded %u Item Enchantment definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    }
    else
        TC_LOG_ERROR("server.loading", ">> Loaded 0 Item Enchantment definitions. DB table `item_enchantment_template` is empty.");
}

uint32 GetItemEnchantMod(int32 entry, ItemRandomEnchantmentType type)
{
    if (!entry)
        return 0;

    if (entry == -1)
        return 0;

    EnchantmentStore::const_iterator tab = type == ItemRandomEnchantmentType::Property ? RandomPropertyItemEnch.find(entry) : RandomSuffixItemEnch.find(entry);
    if (tab == (type == ItemRandomEnchantmentType::Property ? RandomPropertyItemEnch.end() : RandomSuffixItemEnch.end()))
    {
        TC_LOG_ERROR("sql.sql", "Item RandomProperty / RandomSuffix id #%u used in `item_template` but it does not have records in `item_enchantment_template` table.", entry);
        return 0;
    }

    double dRoll = rand_chance();
    float fCount = 0;

    for (auto&& ench_iter : tab->second)
    {
        fCount += ench_iter.chance;

        if (fCount > dRoll)
            return ench_iter.ench;
    }

    // we could get here only if sum of all enchantment chances is lower than 100%
    dRoll = (irand(0, (int)floor(fCount * 100) + 1)) / 100;
    fCount = 0;

    for (auto&& ench_iter : tab->second)
    {
        fCount += ench_iter.chance;

        if (fCount > dRoll)
            return ench_iter.ench;
    }

    return 0;
}

uint32 GenerateEnchSuffixFactor(uint32 itemId)
{
    ItemTemplate const* itemProto = sObjectMgr->GetItemTemplate(itemId);
    if (!itemProto)
        return 0;
    if (!itemProto->RandomSuffix)
        return 0;
    return itemProto->CalculateScalingStatValue(itemProto->ItemLevel);
}
