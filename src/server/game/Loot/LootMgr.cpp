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

#include "Containers.h"
#include "Group.h"
#include "Guild.h"
#include "Log.h"
#include "LootLockoutMap.h"
#include "LootMgr.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Random.h"
#include "SharedDefines.h"
#include "SpellMgr.h"
#include "SpellInfo.h"
#include "Util.h"
#include "World.h"

static Rates const qualityToRate[MAX_ITEM_QUALITY] =
{
    RATE_DROP_ITEM_POOR,                                    // ITEM_QUALITY_POOR
    RATE_DROP_ITEM_NORMAL,                                  // ITEM_QUALITY_NORMAL
    RATE_DROP_ITEM_UNCOMMON,                                // ITEM_QUALITY_UNCOMMON
    RATE_DROP_ITEM_RARE,                                    // ITEM_QUALITY_RARE
    RATE_DROP_ITEM_EPIC,                                    // ITEM_QUALITY_EPIC
    RATE_DROP_ITEM_LEGENDARY,                               // ITEM_QUALITY_LEGENDARY
    RATE_DROP_ITEM_ARTIFACT,                                // ITEM_QUALITY_ARTIFACT
};

LootStore LootTemplates_Creature("creature_loot_template",           "creature entry",                  true);
LootStore LootTemplates_Disenchant("disenchant_loot_template",       "item disenchant id",              true);
LootStore LootTemplates_Fishing("fishing_loot_template",             "area id",                         true);
LootStore LootTemplates_Gameobject("gameobject_loot_template",       "gameobject entry",                true);
LootStore LootTemplates_Item("item_loot_template",                   "item entry",                      true);
LootStore LootTemplates_Mail("mail_loot_template",                   "mail template id",                false);
LootStore LootTemplates_Milling("milling_loot_template",             "item entry (herb)",               true);
LootStore LootTemplates_Pickpocketing("pickpocketing_loot_template", "creature pickpocket lootid",      true);
LootStore LootTemplates_Prospecting("prospecting_loot_template",     "item entry (ore)",                true);
LootStore LootTemplates_Reference("reference_loot_template",         "reference id",                    false);
LootStore LootTemplates_Skinning("skinning_loot_template",           "creature skinning id",            true);
LootStore LootTemplates_Spell("spell_loot_template",                 "spell id (random item creating)", false);

// Selects invalid loot items to be removed from group possible entries (before rolling)
struct LootGroupInvalidSelector
{
    explicit LootGroupInvalidSelector(Loot const& loot, uint32 lootmode, Player* player)
        : _loot(loot), _lootmode(lootmode), _player(player) { }

    bool operator()(LootStoreItem* item) const
    {
        if (item->lootmode && !(item->lootmode & _lootmode))
            return true;

        uint8 foundDuplicates = 0;
        for (std::vector<LootItem>::const_iterator itr = _loot.items.begin(); itr != _loot.items.end(); ++itr)
            if (itr->itemid == item->itemid)
                if (++foundDuplicates == _loot.maxDuplicates)
                    return true;

        if (_loot.containerItemTemplate && _loot.containerItemTemplate->FlagsCu & ITEM_FLAGS_CU_LOOT_GROUP_CHECK_COND)
            if (!sConditionMgr->IsObjectMeetToConditions(_player, item->conditions))
                return true;

        if (_loot.containerItemTemplate && _loot.containerItemTemplate->FlagsCu & ITEM_FLAGS_CU_CHECK_PLAYER_SPEC)
            if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(item->itemid))
                if (!_player->IsItemFitToSpecialization(proto))
                    return true;

        return false;
    }

private:
    Loot const& _loot;
    uint32 _lootmode;
    Player* _player;
};

class LootTemplate::LootGroup                               // A set of loot definitions for items (refs are not allowed)
{
    friend class BonusLoot;
    public:
        LootGroup() { }
        ~LootGroup();

        void AddEntry(LootStoreItem* item);                 // Adds an entry to the group (at loading stage)
        bool HasQuestDrop() const;                          // True if group includes at least 1 quest drop entry
        bool HasQuestDropForPlayer(Player const* player) const;
                                                            // The same for active quests of the player
        void Process(Loot& loot, uint32 lootmode, Player* player) const;    // Rolls an item from the group (if any) and adds the item to the loot
        float RawTotalChance(uint32 lootmode) const;        // Overall chance for the group (without equal chanced items)

        void Verify(LootStore const& lootstore, uint32 id, uint8 group_id) const;
        void CheckLootRefs(LootTemplateMap const& store, LootIdSet* ref_set) const;
        LootStoreItemList* GetExplicitlyChancedItemList() { return &ExplicitlyChanced; }
        LootStoreItemList* GetEqualChancedItemList() { return &EqualChanced; }
        void CopyConditions(ConditionContainer conditions);
    private:
        LootStoreItemList ExplicitlyChanced;                // Entries with chances defined in DB
        LootStoreItemList EqualChanced;                     // Zero chances - every entry takes the same chance

        LootStoreItem const* Roll(Loot& loot, uint32 lootmode, Player* player) const;   // Rolls an item from the group, returns NULL if all miss their chances

        // This class must never be copied - storing pointers
        LootGroup(LootGroup const&);
        LootGroup& operator=(LootGroup const&);
};

//Remove all data and free all memory
void LootStore::Clear()
{
    for (LootTemplateMap::const_iterator itr=m_LootTemplates.begin(); itr != m_LootTemplates.end(); ++itr)
        delete itr->second;
    m_LootTemplates.clear();
}

// Checks validity of the loot store
// Actual checks are done within LootTemplate::Verify() which is called for every template
void LootStore::Verify() const
{
    for (LootTemplateMap::const_iterator i = m_LootTemplates.begin(); i != m_LootTemplates.end(); ++i)
        i->second->Verify(*this, i->first);
}

// Loads a *_loot_template DB table into loot store
// All checks of the loaded template are called from here, no error reports at loot generation required
uint32 LootStore::LoadLootTable()
{
    LootTemplateMap::const_iterator tab;

    // Clearing store (for reloading case)
    Clear();

    //                                                  0     1            2               3             4         5             6
    QueryResult result = WorldDatabase.PQuery("SELECT entry, item, ChanceOrQuestChance, lootmode+0, groupid, mincountOrRef, maxcount FROM %s", GetName());

    if (!result)
        return 0;

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 entry               = fields[0].GetUInt32();
        uint32 item                = abs(fields[1].GetInt32());
        uint8 type = ((fields[1].GetInt32() > 0) ? LOOT_ITEM_TYPE_ITEM : LOOT_ITEM_TYPE_CURRENCY);
        float  chanceOrQuestChance = fields[2].GetFloat();
        uint32 lootmode            = fields[3].GetUInt32();
        uint8  group               = fields[4].GetUInt8();
        int32  mincountOrRef       = fields[5].GetInt32();
        int32  maxcount            = fields[6].GetUInt16();

        if (type == LOOT_ITEM_TYPE_ITEM && maxcount > std::numeric_limits<uint8>::max())
        {
            TC_LOG_ERROR("sql.sql", "Table '%s' entry %d item %d: maxcount value (%u) to large. must be less %u - skipped", GetName(), entry, item, maxcount, std::numeric_limits<uint8>::max());
            continue;                                   // error already printed to log/console.
        }

        if (type == LOOT_ITEM_TYPE_CURRENCY && mincountOrRef < 0)
        {
            TC_LOG_ERROR("sql.sql", "Table '%s' entry %d item %d: has type LOOT_ITEM_TYPE_CURRENCY and mincountOrRef < 0 (%i) - skipped", GetName(), entry, item, mincountOrRef);
            continue;
        }

        if (group >= 1 << 7)                                     // it stored in 7 bit field
        {
            TC_LOG_ERROR("sql.sql", "Table '%s' entry %d item %d: group (%u) must be less %u - skipped", GetName(), entry, item, group, 1 << 7);
            return false;
        }

        LootStoreItem* storeitem = new LootStoreItem(item, type, chanceOrQuestChance, lootmode, group, mincountOrRef, maxcount);

        if (!storeitem->IsValid(*this, entry))            // Validity checks
        {
            delete storeitem;
            continue;
        }

        // Looking for the template of the entry
                                                        // often entries are put together
        if (m_LootTemplates.empty() || tab->first != entry)
        {
            // Searching the template (in case template Id changed)
            tab = m_LootTemplates.find(entry);
            if (tab == m_LootTemplates.end())
            {
                std::pair< LootTemplateMap::iterator, bool > pr = m_LootTemplates.insert(LootTemplateMap::value_type(entry, new LootTemplate()));
                tab = pr.first;
            }
        }
        // else is empty - template Id and iter are the same
        // finally iter refers to already existed or just created <entry, LootTemplate>

        // Adds current row to the template
        tab->second->AddEntry(storeitem);
        ++count;
    }
    while (result->NextRow());

    Verify();                                           // Checks validity of the loot store

    return count;
}

bool LootStore::HaveQuestLootFor(uint32 loot_id) const
{
    LootTemplateMap::const_iterator itr = m_LootTemplates.find(loot_id);
    if (itr == m_LootTemplates.end())
        return false;

    // scan loot for quest items
    return itr->second->HasQuestDrop(m_LootTemplates);
}

bool LootStore::HaveQuestLootForPlayer(uint32 loot_id, Player* player) const
{
    LootTemplateMap::const_iterator tab = m_LootTemplates.find(loot_id);
    if (tab != m_LootTemplates.end())
        if (tab->second->HasQuestDropForPlayer(m_LootTemplates, player))
            return true;

    return false;
}

void LootStore::ResetConditions()
{
    for (LootTemplateMap::iterator itr = m_LootTemplates.begin(); itr != m_LootTemplates.end(); ++itr)
    {
        ConditionContainer empty;
        itr->second->CopyConditions(empty);
    }
}

LootTemplate const* LootStore::GetLootFor(uint32 loot_id) const
{
    LootTemplateMap::const_iterator tab = m_LootTemplates.find(loot_id);

    if (tab == m_LootTemplates.end())
        return NULL;

    return tab->second;
}

LootTemplate* LootStore::GetLootForConditionFill(uint32 loot_id)
{
    LootTemplateMap::iterator tab = m_LootTemplates.find(loot_id);

    if (tab == m_LootTemplates.end())
        return NULL;

    return tab->second;
}

uint32 LootStore::LoadAndCollectLootIds(LootIdSet& lootIdSet)
{
    uint32 count = LoadLootTable();

    for (LootTemplateMap::const_iterator tab = m_LootTemplates.begin(); tab != m_LootTemplates.end(); ++tab)
        lootIdSet.insert(tab->first);

    return count;
}

void LootStore::CheckLootRefs(LootIdSet* ref_set) const
{
    for (LootTemplateMap::const_iterator ltItr = m_LootTemplates.begin(); ltItr != m_LootTemplates.end(); ++ltItr)
        ltItr->second->CheckLootRefs(m_LootTemplates, ref_set);
}

void LootStore::ReportUnusedIds(LootIdSet const& lootIdSet) const
{
    // all still listed ids isn't referenced
    for (LootIdSet::const_iterator itr = lootIdSet.begin(); itr != lootIdSet.end(); ++itr)
        TC_LOG_ERROR("sql.sql", "Table '%s' entry %d isn't %s and not referenced from loot, and then useless.", GetName(), *itr, GetEntryName());
}

void LootStore::ReportNotExistedId(uint32 id) const
{
    TC_LOG_ERROR("sql.sql", "Table '%s' entry %d (%s) does not exist but used as loot id in DB.", GetName(), id, GetEntryName());
}

//
// --------- LootStoreItem ---------
//

// Checks if the entry (quest, non-quest, reference) takes it's chance (at loot generation)
// RATE_DROP_ITEMS is no longer used for all types of entries
bool LootStoreItem::Roll(bool rate) const
{
    if (chance >= 100.0f)
        return true;

    if (mincountOrRef < 0)                                   // reference case
        return roll_chance_f(chance* (rate ? sWorld->getRate(RATE_DROP_ITEM_REFERENCED) : 1.0f));

    if (type == LOOT_ITEM_TYPE_ITEM)
    {
        ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(itemid);
        float qualityModifier = pProto && rate ? sWorld->getRate(qualityToRate[pProto->Quality]) : 1.0f;
        return roll_chance_f(chance * qualityModifier);
    }
    else if (type == LOOT_ITEM_TYPE_CURRENCY)
        return roll_chance_f(chance);

    return false;
}

// Checks correctness of values
bool LootStoreItem::IsValid(LootStore const& store, uint32 entry) const
{
    if (group && type == LOOT_ITEM_TYPE_CURRENCY)
    {
        TC_LOG_ERROR("sql.sql", "Table '%s' entry %d currency %d: group is set, but currencies must not have group %d - skipped", store.GetName(), entry, itemid, group);
        return false;
    }

    if (mincountOrRef == 0)
    {
        TC_LOG_ERROR("sql.sql", "Table '%s' entry %d item %d: wrong mincountOrRef (%d) - skipped", store.GetName(), entry, itemid, mincountOrRef);
        return false;
    }

    if (mincountOrRef > 0)                                  // item (quest or non-quest) entry, maybe grouped
    {
        if (type == LOOT_ITEM_TYPE_ITEM)
        {
            ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemid);
            if (!proto)
            {
                TC_LOG_ERROR("sql.sql", "Table '%s' entry %d item %d: item entry not listed in `item_template` - skipped", store.GetName(), entry, itemid);
                return false;
            }
        }
        else if (type == LOOT_ITEM_TYPE_CURRENCY)
        {
            CurrencyTypesEntry const* currency = sCurrencyTypesStore.LookupEntry(itemid);
            if (!currency)
            {
                TC_LOG_ERROR("sql.sql", "Table '%s' entry %d: currency entry %u not exists - skipped", store.GetName(), entry, itemid);
                return false;
            }
        }
        else
        {
            TC_LOG_ERROR("sql.sql", "Table '%s' entry %d: has unknown item %u with type %u - skipped", store.GetName(), entry, itemid, type);
            return false;
        }

        if (chance == 0 && group == 0)                      // Zero chance is allowed for grouped entries only
        {
            TC_LOG_ERROR("sql.sql", "Table '%s' entry %d item %d: equal-chanced grouped entry, but group not defined - skipped", store.GetName(), entry, itemid);
            return false;
        }

        if (chance != 0 && chance < 0.000001f)             // loot with low chance
        {
            TC_LOG_ERROR("sql.sql", "Table '%s' entry %d item %d: low chance (%f) - skipped",
                store.GetName(), entry, itemid, chance);
            return false;
        }

        if (int32(maxcount) < mincountOrRef)                       // wrong max count
        {
            TC_LOG_ERROR("sql.sql", "Table '%s' entry %d item %d: max count (%u) less that min count (%i) - skipped", store.GetName(), entry, itemid, int32(maxcount), mincountOrRef);
            return false;
        }
    }
    else                                                    // mincountOrRef < 0
    {
        if (needs_quest)
            TC_LOG_ERROR("sql.sql", "Table '%s' entry %d item %d: quest chance will be treated as non-quest chance", store.GetName(), entry, itemid);
        else if (chance == 0)                              // no chance for the reference
        {
            TC_LOG_ERROR("sql.sql", "Table '%s' entry %d item %d: zero chance is specified for a reference, skipped", store.GetName(), entry, itemid);
            return false;
        }
    }
    return true;                                            // Referenced template existence is checked at whole store level
}

//
// --------- LootItem ---------
//

// Constructor, copies most fields from LootStoreItem and generates random count
LootItem::LootItem(LootStoreItem const& li)
{
    itemid      = li.itemid;
    type        = li.type;
    conditions  = li.conditions;
    count       = urand(li.mincountOrRef, li.maxcount);     // constructor called for mincountOrRef > 0 only

    currency = type == LOOT_ITEM_TYPE_CURRENCY;

    is_looted = 0;
    is_blocked = 0;
    is_underthreshold = 0;
    is_counted = 0;
    canSave = true;
    is_world_drop = false;
    personal = false;

    if (currency)
    {
        freeforall = false;
        needs_quest = false;
        follow_loot_rules = false;
        randomSuffix = 0;
        randomPropertyId = 0;
    }
    else
    {
        ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemid);
        freeforall  = proto && (proto->Flags & ITEM_PROTO_FLAG_PARTY_LOOT);
        follow_loot_rules = proto && (proto->FlagsCu & ITEM_FLAGS_CU_FOLLOW_LOOT_RULES);
        needs_quest = li.needs_quest;
        randomSuffix = GenerateEnchSuffixFactor(itemid);
        randomPropertyId = Item::GenerateItemRandomPropertyId(itemid);
    } 
}

LootItem::LootItem(WorldDropLootItem const& item)
{
    count = 1;
    currency = false;
    follow_loot_rules = false;
    is_blocked = false;
    is_counted = false;
    is_looted = false;
    freeforall = true;
    is_world_drop = true;
    itemid = item.Item;
    needs_quest = false;
    type = LOOT_ITEM_TYPE_ITEM;
    personal = false;

    ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemid);
    randomSuffix = GenerateEnchSuffixFactor(itemid);
    randomPropertyId = Item::GenerateItemRandomPropertyId(itemid);
}

// Basic checks for player/item compatibility - if false no chance to see the item in the loot
bool LootItem::AllowedForPlayer(Player const* player, Object* lootedObject) const
{
    // DB conditions check
    if (!sConditionMgr->IsObjectMeetToConditions(const_cast<Player*>(player), conditions))
        return false;

    WorldObject const* lootedWorldObject = nullptr;
    if (lootedObject && lootedObject->GetTypeId() != TYPEID_ITEM && lootedObject->GetTypeId() != TYPEID_CONTAINER)
        lootedWorldObject = static_cast<WorldObject const*>(lootedObject);

    if (lootedWorldObject)
        if (Map* map = lootedWorldObject->FindMap())
            if (map->IsRaid() && ((InstanceMap*)map)->IsLFGMap())
                if (player->HasLootLockout(LootLockoutType::PersonalLoot, lootedWorldObject->GetEntry(), map->GetDifficulty()))
                    return false;

    if (type == LOOT_ITEM_TYPE_ITEM)
    {
        ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(itemid);
        if (!pProto)
            return false;

        // not show loot for players without profession or those who already know the recipe
        if ((pProto->Flags & ITEM_PROTO_FLAG_SMART_LOOT) && (player->GetSkillValue(pProto->RequiredSkill) < pProto->RequiredSkillRank || player->HasSpell(pProto->Spells[1].SpellId)))
            return false;

        // not show loot for not own team
        if ((pProto->Flags2 & ITEM_FLAGS_EXTRA_HORDE_ONLY) && player->GetTeam() != HORDE)
            return false;

        if ((pProto->Flags2 & ITEM_FLAGS_EXTRA_ALLIANCE_ONLY) && player->GetTeam() != ALLIANCE)
            return false;

        // check quest requirements
        if (!(pProto->FlagsCu & ITEM_FLAGS_CU_IGNORE_QUEST_STATUS) && ((needs_quest || (pProto->StartQuest && player->GetQuestStatus(pProto->StartQuest) != QUEST_STATUS_NONE)) && !player->HasQuestForItem(itemid)))
            return false;

        if (lootedObject && pProto->Quality > ITEM_QUALITY_UNCOMMON)
        {
            WorldObject* lootSourceObject = nullptr;
            if (lootedObject->GetGUID().IsGameObject())
                lootSourceObject = player->GetMap()->GetGameObject(lootedObject->GetGUID());
            else if (lootedObject->GetGUID().IsCreature())
                lootSourceObject = player->GetMap()->GetCreature(lootedObject->GetGUID());

            if (lootSourceObject)
            {
                // If worldobject is quest tracked and player have the quest, player isn't allowed to loot more than green quality
                auto trackingQuestId = Vignette::GetTrackingQuestIdFromWorldObject(lootSourceObject);
                if (trackingQuestId && player->IsQuestRewarded(trackingQuestId))
                    return false;
            }
        }
    }
    else if (type == LOOT_ITEM_TYPE_CURRENCY)
    {
        CurrencyTypesEntry const* currency = sCurrencyTypesStore.LookupEntry(itemid);
        if (!currency)
            return false;

        if (!player->IsGameMaster())
        {
            if (currency->Category == CURRENCY_CATEGORY_META_CONQUEST)
                return false;

            if (currency->Category == CURRENCY_CATEGORY_ARCHAEOLOGY && !player->HasSkill(SKILL_ARCHAEOLOGY))
                return false;
        }
    }

    return true;
}

void LootItem::AddAllowedLooter(const Player* player)
{
    allowedGUIDs.insert(player->GetGUID());
}

//
// --------- Loot ---------
//

// Inserts the item into the loot (called by LootTemplate processors)
void Loot::AddItem(LootStoreItem const& item, Player* player)
{
    if (item.type == LOOT_ITEM_TYPE_CURRENCY)
    {
        if (m_mainLootGenerated)
            return;

        if (item.needs_quest)                                   // Quest drop
        {
            if (quest_items.size() < MAX_LOOT_ITEMS)
                quest_items.push_back(LootItem(item));
        }
        else if (items.size() < MAX_LOOT_ITEMS)              // Non-quest drop
            items.push_back(LootItem(item));
    }
    else if (!m_normalLootMode && !item.needs_quest)
    {
        if (items.size() >= MAX_LOOT_ITEMS)
            return;

        int32 slot = -1;
        for (size_t i = 0; i < items.size(); ++i)
        {
            if (items[i].personal && items[i].itemid == item.itemid)
            {
                if (!items[i].AllowedForPlayer(player, GetSource()))
                    return;
                slot = int32(i);
                break;
            }
        }
        if (slot == -1)
        {
            LootItem litem{ item };
            litem.personal = true;
            litem.freeforall = true;

            if (!litem.AllowedForPlayer(player, GetSource()))
                return;

            slot = int32(items.size());
            items.push_back(litem);
        }
        PlayerPersonalItems[player->GetGUID()].push_back(QuestItem{ uint8(slot) });
        ++unlootedCount;
    }
    else
    {
        ItemTemplate const* proto = sObjectMgr->GetItemTemplate(item.itemid);
        if (!proto)
            return;

        uint32 min = item.mincountOrRef;
        uint32 max = item.maxcount;
        if (!CanItemBeLooted(player, item, min, max))
            return;

        uint32 count = urand(min, max);
        uint32 stacks = count / proto->GetMaxStackSize() + ((count % proto->GetMaxStackSize()) ? 1 : 0);

        std::vector<LootItem>& lootItems = item.needs_quest ? quest_items : items;

        for (uint32 i = 0; i < stacks && lootItems.size() < MAX_LOOT_ITEMS; ++i)
        {
            LootItem generatedLoot(item);
            generatedLoot.count = std::min(count, proto->GetMaxStackSize());
            lootItems.push_back(generatedLoot);
            count -= proto->GetMaxStackSize();

            // non-conditional one-player only items are counted here,
            // free for all items are counted in FillFFALoot(),
            // currencies are counter in FillCurrencyLoot(),
            // non-ffa conditionals are counted in FillNonQuestNonFFAConditionalLoot()
            if (!item.needs_quest && item.conditions.empty() && !(proto->Flags & ITEM_PROTO_FLAG_PARTY_LOOT))
                ++unlootedCount;
        }
        if (!item.needs_quest && guildPerkChance && roll_chance_i(guildPerkChance))
        {
            uint32 count = urand(min, max);
            uint32 stacks = count / proto->GetMaxStackSize() + ((count % proto->GetMaxStackSize()) ? 1 : 0);

            std::vector<LootItem>& lootItems = items;

            for (uint32 i = 0; i < stacks && lootItems.size() < MAX_LOOT_ITEMS; ++i)
            {
                LootItem generatedLoot(item);
                generatedLoot.count = std::min(count, proto->GetMaxStackSize());
                lootItems.push_back(generatedLoot);
                count -= proto->GetMaxStackSize();
                ++unlootedCount;
            }
        }
    }
}

void Loot::AddItem(LootStoreItem const& item, uint32 count)
{
    ItemTemplate const* proto = sObjectMgr->GetItemTemplate(item.itemid);
    if (!proto)
        return;

    uint32 stacks = count / proto->GetMaxStackSize() + ((count % proto->GetMaxStackSize()) ? 1 : 0);

    std::vector<LootItem>& lootItems = items;

    for (uint32 i = 0; i < stacks && lootItems.size() < MAX_LOOT_ITEMS; ++i)
    {
        LootItem generatedLoot(item);
        generatedLoot.count = std::min(count, proto->GetMaxStackSize());
        lootItems.push_back(generatedLoot);
        count -= proto->GetMaxStackSize();
        ++unlootedCount;
    }
}

// Calls processor of corresponding LootTemplate (which handles everything including references)
bool Loot::FillLoot(Object* source, uint32 lootId, LootStore const& store, Player* lootOwner, bool personal, bool noEmptyError, Difficulty difficulty /*= LOOT_MODE_DEFAULT*/)
{
    // Must be provided
    if (!lootOwner)
        return false;

    m_source = source;

    LootTemplate const* tab = store.GetLootFor(lootId);

    if (!tab)
    {
        if (!noEmptyError)
            TC_LOG_ERROR("sql.sql", "Table '%s' loot id #%u used but it doesn't have records.", store.GetName(), lootId);
        return false;
    }

    if (source)
    {
        sourceTypeId = source->GetTypeId();
        sourceEntry = sourceTypeId == TYPEID_PLAYER ? source->GetGUID().GetCounter() : source->GetEntry();
        if (source->GetTypeId() == TYPEID_UNIT)
            m_normalLootMode = source->ToCreature()->HasNormalLootMode();
    }

    items.reserve(MAX_LOOT_ITEMS);

    uint32 lootmode = 1 << difficulty;
    
    tab->Process(*this, store.IsRatesAllowed(), lootmode, 0, lootOwner);          // Processing is done there, callback via Loot::AddItem()

    if (GetSource() && GetSource()->GetTypeId() == TYPEID_UNIT)
    {
        Creature* creature = GetSource()->ToCreature();
        if (auto const* loot = sLootMgr->GetWorldDrop(creature))
        {
            for (auto&& itr : *loot)
                if (itr.second.GetChance(creature->GetLevel()) > 0.0f)
                    items.push_back(LootItem{ itr.second });
        }
    }

    // Setting access rights for group loot case
    Group* group = lootOwner->GetGroup();
    if (!m_normalLootMode)
    {
        m_mainLootGenerated = true;

        std::set<Player*> looters;
        for (auto&& itr : source->ToCreature()->GetLootRecipients())
        {
            if (Player* player = ObjectAccessor::GetPlayer(*(WorldObject*)source, itr))
            {
                if (player->IsAtGroupRewardDistance((WorldObject*)source))
                    looters.insert(player);

                if (player->GetGroup())
                    for (auto&& member : *player->GetGroup())
                        if (member->IsAtGroupRewardDistance((WorldObject*)source))
                            looters.insert(member);
            }
        }

        for (auto&& looter : looters)
        {
            if (looter != lootOwner)
                tab->Process(*this, store.IsRatesAllowed(), lootmode, 0, looter);
            FillNotNormalLootFor(looter, true);
        }
    }
    else if (!personal && group)
    {
        roundRobinPlayer = lootOwner->GetGUID();

        for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
            if (Player* player = itr->GetSource())   // should actually be looted object instead of lootOwner but looter has to be really close so doesnt really matter
                FillNotNormalLootFor(player, player->IsAtGroupRewardDistance(lootOwner));

        for (uint8 i = 0; i < items.size(); ++i)
        {
            if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(items[i].itemid))
                if (proto->Quality < uint32(group->GetLootThreshold()))
                    items[i].is_underthreshold = true;
        }
    }
    // ... for personal loot
    else
        FillNotNormalLootFor(lootOwner, true);

    return true;
}

bool Loot::CanItemBeLooted(Player* player, LootStoreItem const& item, uint32& min, uint32& max)
{
    enum class GatheringLootType
    {
        LowLevel,
        Actual,
    };

    static std::map<uint32, GatheringLootType> const map
    {
        { 97512, GatheringLootType::LowLevel }, // Ghost Iron Nugget
        { 72092, GatheringLootType::Actual   }, // Ghost Iron Ore
        { 97546, GatheringLootType::LowLevel }, // Kyparite Fragment
        { 72093, GatheringLootType::Actual   }, // Kyparite
        { 97619, GatheringLootType::LowLevel }, // Torn Green Tea Leaf
        { 72234, GatheringLootType::Actual   }, // Green Tea Leaf
        { 97620, GatheringLootType::LowLevel }, // Rain Poppy Petal
        { 72237, GatheringLootType::Actual   }, // Rain Poppy
        { 97621, GatheringLootType::LowLevel }, // Silkweed Stem
        { 72235, GatheringLootType::Actual   }, // Silkweed
        { 97622, GatheringLootType::LowLevel }, // Snow Lily Petal
        { 79010, GatheringLootType::Actual   }, // Snow Lily
        { 97623, GatheringLootType::LowLevel }, // Fool's Cap Spores
        { 79011, GatheringLootType::Actual   }, // Fool's Cap
        { 97624, GatheringLootType::LowLevel }, // Desecrated Herb Pod
        { 89639, GatheringLootType::Actual   }, // Desecrated Herb
        { 89640, GatheringLootType::Actual   }, // Life Spirit
        { 89641, GatheringLootType::Actual   }, // Water Spirit
    };

    auto it = map.find(item.itemid);
    if (it != map.end())
    {
        if (gatheringCoefficient)
        {
            if (it->second == GatheringLootType::LowLevel)
            {
                int32 diff = int32(max) - int32(min);
                if (diff > 2)
                {
                    diff /= 2;
                    min = max - diff;
                }
                float coef = std::round(gatheringCoefficient * 10.0f) / 10.0f;
                min = std::max(uint32(item.mincountOrRef), uint32(std::floor(min * coef)));
                max = std::max(min, uint32(std::round(max * coef)));
                max = std::min(uint32(item.maxcount), max);
            }
            return it->second == GatheringLootType::LowLevel;
        }
        return it->second == GatheringLootType::Actual;
    }
    if (gatheringCoefficient)       // Thrillium, Golden Lotus
    {
        if (item.itemid == 72238)   // Golden Lotus
            if (m_source && m_source->GetTypeId() == TYPEID_GAMEOBJECT && m_source->GetEntry() != 209354 && m_source->GetEntry() != 221545)
                return false;
        max = min;
    }

    // DO NOT TOUCH!
    static std::set<uint32> const bloodSpirit
    {
        89768, 85986, 86128, 89767, 89819, 86041, 85923, 85378, 86047, 85975, 85376, 86139, 85924, 86081, 85993, 85990,
        85369, 85371, 85997, 85926, 86156, 85978, 85988, 85995, 85980, 86152, 86133, 89822, 86157, 86129, 86086, 89766,
        86083, 85976, 86137, 86027, 85364, 86038, 89829, 85991, 86040, 86170, 85979, 86161, 87827, 86151, 85363, 86167,
        86071, 86082, 87824, 85366, 86324, 89883, 89803, 86335, 86084, 86330, 89827, 86158, 85977, 85982, 90526, 85989,
        86182, 89818, 85987, 85361, 85925, 86334, 85922, 86080, 89824, 86146, 89825, 86142, 86076, 89802, 85994, 86132,
        85992, 86039, 86159, 89821, 86138, 85295, 85985, 86171, 85297, 86178, 86149, 86136, 86166, 86175, 89817, 86075,
        86044, 86127, 86153, 86229, 85984, 85996, 86134, 86388, 90527, 89931, 85983, 86389, 86148, 86141, 87027, 85300,
        86160, 89424, 86328, 86181, 86326, 89820, 86150, 90738, 89823, 85342, 87028, 85302, 86145, 89833, 86204, 89930,
        86135, 86169, 89831, 85337, 89842, 86179, 89826, 86163, 90517, 86329, 87013, 87014, 86140, 86332, 90524, 89839,
        89886, 86176, 89843, 86144, 89887, 89830, 86331, 86210, 87021, 86154, 87038, 86214, 86131, 86168, 86321, 87017,
        86391, 86203, 86514, 86173, 86180, 89884, 85335, 89929, 85340, 87012, 86042, 86205, 87023, 85358, 86155, 87030,
        86336, 85317, 89835, 90523, 86325, 86200, 90530, 86513, 87052, 90519, 86045, 90529, 86327, 86165, 86323, 87016,
        87044, 89938, 89836, 89834, 86333, 87034, 90518, 87064,
    };

    switch (item.itemid)
    {
        case 80433: // Blood Spirit
            if (loot_type == LOOT_DISENCHANTING)
                return bloodSpirit.find(sourceEntry) != bloodSpirit.end();
            break;
        case 94289: // Haunting Spirit
        case 102218:// Spirit of War
        {
            if (loot_type == LOOT_DISENCHANTING)
            {
                auto const* itemTemplate = sObjectMgr->GetItemTemplate(sourceEntry);
                if (!itemTemplate)
                    return false;
                if (itemTemplate->FlagsCu & ITEM_FLAGS_CU_VENDOR)
                    return false;

                if (item.itemid == 94289)
                {
                    switch (itemTemplate->ItemLevel)
                    {
                        case 503: // LFR
                            return roll_chance_i(50);
                        case 522: // Normal
                        case 535: // Heroic
                        case 541: // Heroic Thunderforged
                            return true;
                        case 528: // Normal Thunderforged
                            if (auto entry = sItemNameDescriptionStore.LookupEntry(itemTemplate->ItemId))
                                if (strcmp(entry->Description, "Thunderforged") == 0) // TODO change compare mode
                                    return true;
                            return false;
                    }
                }
                else
                {
                    switch (itemTemplate->ItemLevel)
                    {
                        case 528: // LFR
                            if (auto entry = sItemNameDescriptionStore.LookupEntry(itemTemplate->ItemId))
                                if (strcmp(entry->Description, "Raid Finder") == 0) // TODO change compare mode
                                    return roll_chance_i(50);
                            return false;
                        case 553: // Normal
                        case 559: // Normal Thunderforged
                        case 566: // Heroic
                        case 572: // Heroic Thunderforged
                            return true;
                    }
                }
                return false;
            }
            break;
        }
    }

    return true;
}

void Loot::FillNotNormalLootFor(Player* player, bool presentAtLooting)
{
    uint32 plguid = player->GetGUID().GetCounter();

    QuestItemMap::const_iterator qmapitr = PlayerCurrencies.find(plguid);
    if (qmapitr == PlayerCurrencies.end())
        FillCurrencyLoot(player);

    qmapitr = PlayerQuestItems.find(plguid);
    if (qmapitr == PlayerQuestItems.end())
        FillQuestLoot(player);

    qmapitr = PlayerFFAItems.find(plguid);
    if (qmapitr == PlayerFFAItems.end())
        FillFFALoot(player);

    qmapitr = PlayerNonQuestNonFFANonCurrencyConditionalItems.find(plguid);
    if (qmapitr == PlayerNonQuestNonFFANonCurrencyConditionalItems.end())
        FillNonQuestNonFFANonCurrencyConditionalLoot(player, presentAtLooting);

    // if not auto-processed player will have to come and pick it up manually
    if (!presentAtLooting)
        return;

    // Process currency items
    uint32 max_slot = GetMaxSlotInLootFor(player);
    LootItem const* item = NULL;
    uint32 itemsSize = uint32(items.size());
    for (uint32 i = 0; i < max_slot; ++i)
    {
        if (i < items.size())
            item = &items[i];
        else
            item = &quest_items[i-itemsSize];

        if (!item->is_looted && item->freeforall && item->AllowedForPlayer(player, GetSource()))
            if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(item->itemid))
                if (proto->IsCurrencyToken())
                    player->StoreLootItem(i, this);
    }
}

void Loot::FillCurrencyLoot(Player* player)
{
    QuestItemList* ql = new QuestItemList();

    for (uint8 i = 0; i < items.size(); ++i)
    {
        LootItem& item = items[i];
        if (!item.is_looted && item.currency && item.AllowedForPlayer(player, GetSource()))
        {
            ql->push_back(QuestItem(i));
            ++unlootedCount;
        }
    }
    if (ql->empty())
    {
        delete ql;
        return;
    }

    PlayerCurrencies[player->GetGUID().GetCounter()] = ql;
}

void Loot::FillFFALoot(Player* player)
{
    QuestItemList* ql = new QuestItemList();

    for (uint8 i = 0; i < items.size(); ++i)
    {
        LootItem &item = items[i];
        if (!item.is_looted && item.freeforall && !item.is_world_drop && !item.personal && item.AllowedForPlayer(player, GetSource()))
        {
            ql->push_back(QuestItem(i));
            ++unlootedCount;
        }
    }

    if (GetSource() && GetSource()->GetTypeId() == TYPEID_UNIT)
    {
        Creature* creature = GetSource()->ToCreature();
        if (auto const* worldDrop = sLootMgr->GetWorldDrop(creature))
        {
            struct WorldDropRoll
            {
                uint8 Slot;
                WorldDropLootItem const* Item;
            };
            std::map<uint32, std::vector<WorldDropRoll>> toRoll;

            for (uint8 i = 0; i < items.size(); ++i)
            {
                LootItem &item = items[i];
                if (!item.is_looted && item.is_world_drop && item.AllowedForPlayer(player, GetSource()))
                {
                    ItemTemplate const* proto = sObjectMgr->GetItemTemplate(item.itemid);
                    if (!proto)
                        continue;

                    // I don't know why not all of them has this flag
                    if (!(proto->Flags & ITEM_PROTO_FLAG_SMART_LOOT) && proto->RequiredSkill)
                        if (player->GetSkillValue(proto->RequiredSkill) < proto->RequiredSkillRank || player->HasSpell(proto->Spells[1].SpellId))
                            continue;

                    // TODO: ADD CONDITIONS
                    // Schematic: Chief Engineer Jard's Journal, Pattern: Hardened Magnificent Hide and Its Uses, Pattern: Celestial Cloth and Its Uses, Plans: Balanced Trillium Ingot and Its Uses. Has no spell 1.
                    if (item.itemid == 100910 && player->HasSpell(139176))
                        continue;
                    if (item.itemid == 100864 && player->HasSpell(142976))
                        continue;
                    if (item.itemid == 100863 && player->HasSpell(143011))
                        continue;
                    if (item.itemid == 100865 && player->HasSpell(143255))
                        continue;

                    // Plundered Treasure
                    if (item.itemid == 87391 && !player->HasAura(130653) && !player->HasAura(135855) && !player->HasAura(105699))
                        continue;

                    auto itr = worldDrop->find(item.itemid);
                    ASSERT(itr != worldDrop->end());

                    if (itr->second.Group)
                        toRoll[itr->second.Group].push_back({ i, &itr->second });
                    else
                    {
                        float chance = itr->second.GetChance(creature->GetLevel());
                        if (creature->isElite())
                            AddPct(chance, 20.0f);
                        if (roll_chance_f(chance))
                        {
                            ql->push_back(QuestItem{ i });
                            ++unlootedCount;
                        }
                    }
                }
            }

            for (auto&& itr : toRoll)
            {
                auto& rolled = itr.second[urand(0, itr.second.size() - 1)];
                float chance = rolled.Item->GetChance(creature->GetLevel());
                if (creature->isElite())
                    AddPct(chance, 20.0f);
                if (roll_chance_f(chance))
                {
                    ql->push_back(QuestItem{ rolled.Slot });
                    ++unlootedCount;
                }
            }
        }
    }

    if (ql->empty())
    {
        delete ql;
        return;
    }

    PlayerFFAItems[player->GetGUID().GetCounter()] = ql;
}

void Loot::FillQuestLoot(Player* player)
{
    if (items.size() == MAX_LOOT_ITEMS)
        return;

    QuestItemList* ql = new QuestItemList();

    for (uint8 i = 0; i < quest_items.size(); ++i)
    {
        LootItem &item = quest_items[i];

        if (!item.is_looted && !item.is_world_drop
            && (item.AllowedForPlayer(player, GetSource())
                || (item.follow_loot_rules && player->GetGroup() && ((player->GetGroup()->GetLootMethod() == MASTER_LOOT && player->GetGroup()->GetLooterGuid() == player->GetGUID()) || player->GetGroup()->GetLootMethod() != MASTER_LOOT))))
        {
            ql->push_back(QuestItem(i));

            // quest items get blocked when they first appear in a
            // player's quest vector
            //
            // increase once if one looter only, looter-times if free for all
            if (item.freeforall || !item.is_blocked)
                ++unlootedCount;
            if (!player->GetGroup() || (player->GetGroup()->GetLootMethod() != GROUP_LOOT && player->GetGroup()->GetLootMethod() != ROUND_ROBIN))
                item.is_blocked = true;

            if (items.size() + ql->size() == MAX_LOOT_ITEMS)
                break;
        }
    }
    if (ql->empty())
    {
        delete ql;
        return;
    }

    PlayerQuestItems[player->GetGUID().GetCounter()] = ql;
}

void Loot::FillNonQuestNonFFANonCurrencyConditionalLoot(Player* player, bool presentAtLooting)
{
    QuestItemList* ql = new QuestItemList();

    for (uint8 i = 0; i < items.size(); ++i)
    {
        LootItem &item = items[i];
        if (!item.is_looted && !item.currency && !item.freeforall && (item.AllowedForPlayer(player, GetSource()) || (item.follow_loot_rules && player->GetGroup() && ((player->GetGroup()->GetLootMethod() == MASTER_LOOT && player->GetGroup()->GetLooterGuid() == player->GetGUID()) || player->GetGroup()->GetLootMethod() != MASTER_LOOT ))))
        {
            if (presentAtLooting)
                item.AddAllowedLooter(player);
            if (!item.conditions.empty())
            {
                ql->push_back(QuestItem(i));
                if (!item.is_counted)
                {
                    ++unlootedCount;
                    item.is_counted = true;
                }
            }
        }
    }

    if (ql->empty())
    {
        delete ql;
        return;
    }

    PlayerNonQuestNonFFANonCurrencyConditionalItems[player->GetGUID().GetCounter()] = ql;
}

//===================================================

void Loot::NotifyItemRemoved(uint8 lootIndex)
{
    // notify all players that are looting this that the item was removed
    // convert the index to the slot the player sees
    std::set<ObjectGuid>::iterator i_next;
    for (std::set<ObjectGuid>::iterator i = PlayersLooting.begin(); i != PlayersLooting.end(); i = i_next)
    {
        i_next = i;
        ++i_next;
        if (Player* player = ObjectAccessor::FindPlayer(*i))
            player->SendNotifyLootItemRemoved(lootIndex, GetGUID());
        else
            PlayersLooting.erase(i);
    }
}

void Loot::NotifyMoneyRemoved()
{
    // notify all players that are looting this that the money was removed
    std::set<ObjectGuid>::iterator i_next;
    for (std::set<ObjectGuid>::iterator i = PlayersLooting.begin(); i != PlayersLooting.end(); i = i_next)
    {
        i_next = i;
        ++i_next;
        if (Player* player = ObjectAccessor::FindPlayer(*i))
            player->SendNotifyLootMoneyRemoved(GetGUID());
        else
            PlayersLooting.erase(i);
    }
}

void Loot::NotifyQuestItemRemoved(uint8 questIndex)
{
    // when a free for all questitem is looted
    // all players will get notified of it being removed
    // (other questitems can be looted by each group member)
    // bit inefficient but isn't called often

    std::set<ObjectGuid>::iterator i_next;
    for (std::set<ObjectGuid>::iterator i = PlayersLooting.begin(); i != PlayersLooting.end(); i = i_next)
    {
        i_next = i;
        ++i_next;
        if (Player* player = ObjectAccessor::FindPlayer(*i))
        {
            QuestItemMap::const_iterator pq = PlayerQuestItems.find(player->GetGUID().GetCounter());
            if (pq != PlayerQuestItems.end() && pq->second)
            {
                // find where/if the player has the given item in it's vector
                QuestItemList& pql = *pq->second;

                uint8 j;
                for (j = 0; j < pql.size(); ++j)
                    if (pql[j].index == questIndex)
                        break;

                if (j < pql.size())
                    player->SendNotifyLootItemRemoved(items.size() + j, GetGUID());
            }
        }
        else
            PlayersLooting.erase(i);
    }
}

void Loot::generateMoneyLoot(uint32 minAmount, uint32 maxAmount)
{
    if (maxAmount > 0)
    {
        if (maxAmount <= minAmount)
            gold = uint32(maxAmount * sWorld->getRate(RATE_DROP_MONEY));
        else if ((maxAmount - minAmount) < 32700)
            gold = uint32(urand(minAmount, maxAmount) * sWorld->getRate(RATE_DROP_MONEY));
        else
            gold = uint32(urand(minAmount >> 8, maxAmount >> 8) * sWorld->getRate(RATE_DROP_MONEY)) << 8;
    }
}

void Loot::DeleteLootItemFromContainerItemDB(uint32 itemID)
{
    // Deletes a single item associated with an openable item from the DB
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ITEMCONTAINER_ITEM);
    stmt->setUInt32(0, containerID);
    stmt->setUInt32(1, itemID);
    CharacterDatabase.Execute(stmt);

    // Mark the item looted to prevent resaving
    for (LootItemList::iterator _itr = items.begin(); _itr != items.end(); ++_itr)
    {
        if (_itr->itemid != itemID)
            continue;

        _itr->canSave = true;
        break;
    }
}

void Loot::DeleteLootMoneyFromContainerItemDB()
{
    // Deletes money loot associated with an openable item from the DB
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ITEMCONTAINER_MONEY);
    stmt->setUInt32(0, containerID);
    CharacterDatabase.Execute(stmt);
}

LootItem* Loot::LootItemInSlot(uint32 lootSlot, Player* player, QuestItem** qitem, QuestItem** ffaitem, QuestItem** conditem, QuestItem** currency, QuestItem** persoanl)
{
    LootItem* item = NULL;
    bool is_looted = true;
    if (lootSlot >= items.size())
    {
        uint32 questSlot = lootSlot - items.size();
        QuestItemMap::const_iterator itr = PlayerQuestItems.find(player->GetGUID().GetCounter());
        if (itr != PlayerQuestItems.end() && questSlot < itr->second->size())
        {
            QuestItem* qitem2 = &itr->second->at(questSlot);
            if (qitem)
                *qitem = qitem2;
            item = &quest_items[qitem2->index];
            is_looted = qitem2->is_looted;
        }
    }
    else
    {
        item = &items[lootSlot];
        is_looted = item->is_looted;
        if (item->currency)
        {
            QuestItemMap::const_iterator itr = PlayerCurrencies.find(player->GetGUID().GetCounter());
            if (itr != PlayerCurrencies.end())
            {
                for (QuestItemList::const_iterator iter = itr->second->begin(); iter != itr->second->end(); ++iter)
                {
                    if (iter->index == lootSlot)
                    {
                        QuestItem* currency2 = (QuestItem*) & (*iter);
                        if (currency)
                            *currency = currency2;
                        is_looted = currency2->is_looted;
                        break;
                    }
                }
            }
        }
        else if (item->personal) // Before freeforall becasue it is also ture
        {
            auto itr = PlayerPersonalItems.find(player->GetGUID().GetCounter());
            if (itr != PlayerPersonalItems.end())
            {
                for (auto&& personalItem : itr->second)
                {
                    if (personalItem.index == lootSlot)
                    {
                        if (persoanl)
                            *persoanl = &personalItem;
                        is_looted = personalItem.is_looted;
                        break;
                    }
                }
            }
        }
        else if (item->freeforall)
        {
            QuestItemMap::const_iterator itr = PlayerFFAItems.find(player->GetGUID().GetCounter());
            if (itr != PlayerFFAItems.end())
            {
                for (QuestItemList::const_iterator iter=itr->second->begin(); iter!= itr->second->end(); ++iter)
                    if (iter->index == lootSlot)
                    {
                        QuestItem* ffaitem2 = (QuestItem*)&(*iter);
                        if (ffaitem)
                            *ffaitem = ffaitem2;
                        is_looted = ffaitem2->is_looted;
                        break;
                    }
            }
        }
        else if (!item->conditions.empty())
        {
            QuestItemMap::const_iterator itr = PlayerNonQuestNonFFANonCurrencyConditionalItems.find(player->GetGUID().GetCounter());
            if (itr != PlayerNonQuestNonFFANonCurrencyConditionalItems.end())
            {
                for (QuestItemList::const_iterator iter=itr->second->begin(); iter!= itr->second->end(); ++iter)
                {
                    if (iter->index == lootSlot)
                    {
                        QuestItem* conditem2 = (QuestItem*)&(*iter);
                        if (conditem)
                            *conditem = conditem2;
                        is_looted = conditem2->is_looted;
                        break;
                    }
                }
            }
        }
    }

    if (is_looted)
        return NULL;

    return item;
}

uint32 Loot::GetMaxSlotInLootFor(Player* player) const
{
    QuestItemMap::const_iterator itr = PlayerQuestItems.find(player->GetGUID().GetCounter());
    return items.size() + (itr != PlayerQuestItems.end() ?  itr->second->size() : 0);
}

// return true if there is any FFA, quest or conditional item for the player.
bool Loot::hasItemFor(Player* player) const
{
    QuestItemMap const& lootPlayerCurrencies = GetPlayerCurrencies();
    QuestItemMap::const_iterator cur_itr = lootPlayerCurrencies.find(player->GetGUID().GetCounter());
    if (cur_itr != lootPlayerCurrencies.end())
    {
        QuestItemList* cur_list = cur_itr->second;
        for (QuestItemList::const_iterator cui = cur_list->begin(); cui != cur_list->end(); ++cui)
        {
            if (cui->index < items.size())
            {
                const LootItem &item = items[cui->index];
                if (!cui->is_looted && !item.is_looted)
                    return true;
            }
            else
            {
                uint32 questSlot = cui->index - items.size();
                ASSERT(questSlot < quest_items.size());
                const LootItem &item = quest_items[questSlot];
                if (!cui->is_looted && !item.is_looted)
                    return true;
            }
        }
    }

    QuestItemMap const& lootPlayerQuestItems = GetPlayerQuestItems();
    QuestItemMap::const_iterator q_itr = lootPlayerQuestItems.find(player->GetGUID().GetCounter());
    if (q_itr != lootPlayerQuestItems.end())
    {
        QuestItemList* q_list = q_itr->second;
        for (QuestItemList::const_iterator qi = q_list->begin(); qi != q_list->end(); ++qi)
        {
            const LootItem &item = quest_items[qi->index];
            if (!qi->is_looted && !item.is_looted)
                return true;
        }
    }

    QuestItemMap const& lootPlayerFFAItems = GetPlayerFFAItems();
    QuestItemMap::const_iterator ffa_itr = lootPlayerFFAItems.find(player->GetGUID().GetCounter());
    if (ffa_itr != lootPlayerFFAItems.end())
    {
        QuestItemList* ffa_list = ffa_itr->second;
        for (QuestItemList::const_iterator fi = ffa_list->begin(); fi != ffa_list->end(); ++fi)
        {
            const LootItem &item = items[fi->index];
            if (!fi->is_looted && !item.is_looted)
                return true;
        }
    }

    QuestItemMap const& lootPlayerNonQuestNonFFANonCurrencyConditionalItems = GetPlayerNonQuestNonFFANonCurrencyConditionalItems();
    QuestItemMap::const_iterator nn_itr = lootPlayerNonQuestNonFFANonCurrencyConditionalItems.find(player->GetGUID().GetCounter());
    if (nn_itr != lootPlayerNonQuestNonFFANonCurrencyConditionalItems.end())
    {
        QuestItemList* conditional_list = nn_itr->second;
        for (QuestItemList::const_iterator ci = conditional_list->begin(); ci != conditional_list->end(); ++ci)
        {
            const LootItem &item = items[ci->index];
            if (!ci->is_looted && !item.is_looted)
                return true;
        }
    }

    return false;
}

// return true if there is any item over the group threshold (i.e. not underthreshold).
bool Loot::hasOverThresholdItem() const
{
    for (uint8 i = 0; i < items.size(); ++i)
    {
        if (!items[i].is_looted && !items[i].is_underthreshold && !items[i].freeforall)
            return true;
    }

    return false;
}

ItemPickupSourceType Loot::GetItemPickupSourceType() const
{
    switch (sourceTypeId)
    {
        case TYPEID_ITEM:       return ItemPickupSourceType::ItemLoot;
        case TYPEID_CONTAINER:  return ItemPickupSourceType::ItemLoot;
        case TYPEID_UNIT:       return ItemPickupSourceType::CreatureLoot;
        case TYPEID_PLAYER:     return ItemPickupSourceType::PlayerLoot;
        case TYPEID_GAMEOBJECT: return ItemPickupSourceType::GameObjectLoot;
        case TYPEID_CORPSE:     return ItemPickupSourceType::PlayerLoot;
        default:                return ItemPickupSourceType::Unknown;
    }
}

void LootItem::WriteBitDataPart(uint8 slotType, ByteBuffer* buff)
{
    if (currency)
    {
        buff->WriteBits(slotType, 3);
    }
    else
    {
        buff->WriteBits(slotType, 3);
        buff->WriteBit(0); // canTradeToTapList
        buff->WriteBit(1);
        buff->WriteBit(0); // Contains Slot - Always sent
        buff->WriteBits(0, 2); // Unk 2 bits
    }
}

void LootItem::WriteBasicDataPart(uint8 slot, ByteBuffer* buff)
{
    if (currency)
    {
        *buff << uint32(count);
        *buff << uint8(slot);
        *buff << uint32(itemid);
    }
    else
    {
        *buff << uint32(randomSuffix);
        *buff << uint32(count);
        *buff << uint32(itemid);
        *buff << uint32(4); // Send situ size
        *buff << uint32(0); // Blizz always send 0 uint32 as situ (read in packet handler)

        *buff << uint32(randomPropertyId);
        *buff << uint8(slot);
        uint32 displayId = 0;
        if (auto const* itemTemplate = sObjectMgr->GetItemTemplate(itemid))
            displayId = itemTemplate->DisplayInfoID;
        else
            TC_LOG_ERROR("shitlog", "LootItem::WriteBasicDataPart, possible corrupted memory: %u %u", itemid, uint32(type));
        *buff << uint32(displayId);
    }
}

void LootView::WriteData(ObjectGuid guid, LootType lootType, WorldPacket* data, bool isAoE)
{
    ObjectGuid lootGuid = loot.GetGUID();

    if (permission == NONE_PERMISSION)
    {
        data->WriteBit(1); // Missing unk8
        data->WriteBit(0); // lootType present
        data->WriteBit(guid[4]);
        data->WriteBits(0, 20);
        data->WriteBit(lootGuid[2]);
        data->WriteBit(lootGuid[3]);
        data->WriteBit(lootGuid[7]);
        data->WriteBit(lootGuid[1]);
        data->WriteBit(guid[6]);
        data->WriteBit(guid[7]);
        data->WriteBit(1);
        data->WriteBit(1); // isPersonalLooting
        data->WriteBit(1); // Missing unk8
        data->WriteBit(0); // isAoELooting
        data->WriteBit(guid[5]);
        data->WriteBit(lootGuid[6]);
        data->WriteBit(guid[1]);
        data->WriteBit(guid[0]);
        data->WriteBit(lootGuid[5]);
        data->WriteBit(guid[3]);
        data->WriteBit(lootGuid[4]);
        data->WriteBit(1); // Missing unk8
        data->WriteBit(guid[2]);
        data->FlushBits();
        data->WriteByteSeq(lootGuid[2]);
        data->WriteByteSeq(lootGuid[7]);
        data->WriteByteSeq(guid[5]);
        data->WriteByteSeq(lootGuid[3]);
        data->WriteByteSeq(lootGuid[4]);
        *data << uint8(lootType);
        data->WriteByteSeq(guid[4]);
        data->WriteByteSeq(lootGuid[5]);
        data->WriteByteSeq(guid[2]);
        data->WriteByteSeq(guid[3]);
        data->WriteByteSeq(lootGuid[1]);
        data->WriteByteSeq(guid[0]);
        data->WriteByteSeq(lootGuid[0]);
        data->WriteByteSeq(guid[6]);
        data->WriteByteSeq(guid[7]);
        data->WriteByteSeq(guid[1]);
        data->WriteByteSeq(lootGuid[6]);
        return;
    }

    uint8 itemsShown = 0;
    uint8 currenciesShown = 0;
    ByteBuffer itemBuff;
    ByteBuffer currencyBuff;

    data->WriteBit(1); // Missing unk8
    data->WriteBit(0); // lootType present
    data->WriteBit(guid[4]);

    size_t currencyCountPos = data->bitwpos();
    data->WriteBits(0, 20);

    data->WriteBit(lootGuid[2]);
    data->WriteBit(lootGuid[3]);
    data->WriteBit(lootGuid[7]);
    data->WriteBit(lootGuid[1]);
    data->WriteBit(guid[6]);
    data->WriteBit(guid[7]);
    data->WriteBit(!loot.gold);
    data->WriteBit(1); // isPersonalLooting
    data->WriteBit(1); // Missing unk8
    data->WriteBit(isAoE); // isAoELooting
    data->WriteBit(guid[5]);
    data->WriteBit(lootGuid[6]);

    size_t itemCountPos = data->bitwpos();
    data->WriteBits(0, 19);

    data->WriteBit(lootGuid[0]);

    bool hasPermission = true;

    switch (permission)
    {
        case GROUP_PERMISSION:
        {
            // if you are not the round-robin group looter, you can only see
            // blocked rolled items and quest items, and !ffa items
            for (uint8 i = 0; i < loot.items.size(); ++i)
            {
                if (!loot.items[i].currency && !loot.items[i].is_looted && !loot.items[i].freeforall && loot.items[i].conditions.empty() && loot.items[i].AllowedForPlayer(viewer, loot.GetSource()))
                {
                    LootSlotType slotType;

                    if (loot.items[i].is_blocked)
                        slotType = LOOT_SLOT_TYPE_ROLL_ONGOING;
                    else if (loot.roundRobinPlayer == 0 || !loot.items[i].is_underthreshold || viewer->GetGUID() == loot.roundRobinPlayer)
                    {
                        // no round robin owner or he has released the loot
                        // or it IS the round robin group owner
                        // => item is lootable
                        slotType = LOOT_SLOT_TYPE_ALLOW_LOOT;
                    }
                    else
                        continue; // item shall not be displayed.

                    loot.items[i].WriteBitDataPart(slotType, data);
                    loot.items[i].WriteBasicDataPart(i, &itemBuff);
                    ++itemsShown;
                }
            }
            break;
        }
        case ROUND_ROBIN_PERMISSION:
        {
            for (uint8 i = 0; i < loot.items.size(); ++i)
            {
                if (!loot.items[i].currency && !loot.items[i].is_looted && !loot.items[i].freeforall && loot.items[i].conditions.empty() && loot.items[i].AllowedForPlayer(viewer, loot.GetSource()))
                {
                    if (loot.roundRobinPlayer != 0 && viewer->GetGUID() != loot.roundRobinPlayer)
                        // item shall not be displayed.
                        continue;

                    loot.items[i].WriteBitDataPart(LOOT_SLOT_TYPE_ALLOW_LOOT, data);
                    loot.items[i].WriteBasicDataPart(i, &itemBuff);
                    ++itemsShown;
                }
            }
            break;
        }
        case ALL_PERMISSION:
        case MASTER_PERMISSION:
        case OWNER_PERMISSION:
        {
            LootSlotType slotType = LOOT_SLOT_TYPE_ALLOW_LOOT;
            switch (permission)
            {
                case MASTER_PERMISSION:
                    slotType = LOOT_SLOT_TYPE_MASTER;
                    break;
                case OWNER_PERMISSION:
                    slotType = LOOT_SLOT_TYPE_OWNER;
                    break;
                default:
                    break;
            }

            for (uint8 i = 0; i < loot.items.size(); ++i)
            {
                LootSlotType finalSlotType = slotType;
                if (slotType == LOOT_SLOT_TYPE_MASTER)
                {
                    if (!loot.items[i].is_blocked)
                        finalSlotType = LOOT_SLOT_TYPE_ALLOW_LOOT;
                    else if (viewer->GetGUID() != viewer->GetGroup()->GetLooterGuid())
                        finalSlotType = LOOT_SLOT_TYPE_OWNER; // Temporary hack
                }
                if (!loot.items[i].currency && !loot.items[i].is_looted && !loot.items[i].freeforall && loot.items[i].conditions.empty() && loot.items[i].AllowedForPlayer(viewer, loot.GetSource()))
                {
                    loot.items[i].WriteBitDataPart(finalSlotType, data);
                    loot.items[i].WriteBasicDataPart(i, &itemBuff);
                    ++itemsShown;
                }
            }
            break;
        }
        default:
            hasPermission = false;
            break;
    }

    if (hasPermission)
    {
        LootSlotType slotType = permission == OWNER_PERMISSION ? LOOT_SLOT_TYPE_OWNER : LOOT_SLOT_TYPE_ALLOW_LOOT;
        QuestItemMap const& lootPlayerQuestItems = loot.GetPlayerQuestItems();
        QuestItemMap::const_iterator q_itr = lootPlayerQuestItems.find(viewer->GetGUID().GetCounter());
        if (q_itr != lootPlayerQuestItems.end())
        {
            QuestItemList* q_list = q_itr->second;
            for (QuestItemList::const_iterator qi = q_list->begin(); qi != q_list->end(); ++qi)
            {
                LootItem &item = loot.quest_items[qi->index];
                if (!qi->is_looted && !item.is_looted)
                {
                    LootSlotType finalSlotType = slotType;
                    uint8 index = loot.items.size() + (qi - q_list->begin());

                    if (item.follow_loot_rules)
                    {
                        switch (permission)
                        {
                            case MASTER_PERMISSION:
                                finalSlotType = LOOT_SLOT_TYPE_MASTER;
                                break;
                            case GROUP_PERMISSION:
                            case ROUND_ROBIN_PERMISSION:
                                if (!item.is_blocked)
                                    finalSlotType = LOOT_SLOT_TYPE_ALLOW_LOOT;
                                else
                                    finalSlotType = LOOT_SLOT_TYPE_ROLL_ONGOING;
                                break;
                            default:
                                break;
                        }
                    }

                    item.WriteBitDataPart(finalSlotType, data);
                    item.WriteBasicDataPart(index, &itemBuff);
                    ++itemsShown;
                }
            }
        }

        QuestItemMap const& lootPlayerFFAItems = loot.GetPlayerFFAItems();
        QuestItemMap::const_iterator ffa_itr = lootPlayerFFAItems.find(viewer->GetGUID().GetCounter());
        if (ffa_itr != lootPlayerFFAItems.end())
        {
            QuestItemList* ffa_list = ffa_itr->second;
            for (QuestItemList::const_iterator fi = ffa_list->begin(); fi != ffa_list->end(); ++fi)
            {
                LootItem &item = loot.items[fi->index];
                if (!fi->is_looted && !item.is_looted)
                {
                    item.WriteBitDataPart(slotType, data);
                    item.WriteBasicDataPart(fi->index, &itemBuff);
                    ++itemsShown;
                }
            }
        }

        QuestItemMap const& lootPlayerNonQuestNonFFANonCurrencyConditionalItems = loot.GetPlayerNonQuestNonFFANonCurrencyConditionalItems();
        QuestItemMap::const_iterator nn_itr = lootPlayerNonQuestNonFFANonCurrencyConditionalItems.find(viewer->GetGUID().GetCounter());
        if (nn_itr != lootPlayerNonQuestNonFFANonCurrencyConditionalItems.end())
        {
            QuestItemList* conditional_list = nn_itr->second;
            for (QuestItemList::const_iterator ci = conditional_list->begin(); ci != conditional_list->end(); ++ci)
            {
                LootItem &item = loot.items[ci->index];
                if (!ci->is_looted && !item.is_looted)
                {
                    LootSlotType finalSlotType = slotType;

                    if (item.follow_loot_rules)
                    {
                        switch (permission)
                        {
                        case MASTER_PERMISSION:
                            finalSlotType = LOOT_SLOT_TYPE_MASTER;
                            break;
                        case GROUP_PERMISSION:
                        case ROUND_ROBIN_PERMISSION:
                            if (!item.is_blocked)
                                finalSlotType = LOOT_SLOT_TYPE_ALLOW_LOOT;
                            else
                                finalSlotType = LOOT_SLOT_TYPE_ROLL_ONGOING;
                            break;
                        default:
                            break;
                        }
                    }

                    item.WriteBitDataPart(finalSlotType, data);
                    item.WriteBasicDataPart(ci->index, &itemBuff);
                    ++itemsShown;
                }
            }
        }
    }

    auto& personalItems = loot.GetPersonalItems();
    auto itr = personalItems.find(viewer->GetGUID().GetCounter());
    if (itr != personalItems.end())
    {
        for (auto&& personalItem : itr->second)
        {
            LootItem& item = loot.items[personalItem.index];
            if (!personalItem.is_looted)
            {
                item.WriteBitDataPart(LOOT_SLOT_TYPE_OWNER, data);
                item.WriteBasicDataPart(personalItem.index, &itemBuff);
                ++itemsShown;
            }
        }
    }

    data->WriteBit(guid[1]);
    data->WriteBit(guid[0]);

    QuestItemMap const& lootPlayerCurrencies = loot.GetPlayerCurrencies();
    QuestItemMap::const_iterator currency_itr = lootPlayerCurrencies.find(viewer->GetGUID().GetCounter());
    if (currency_itr != lootPlayerCurrencies.end())
    {
        QuestItemList* currency_list = currency_itr->second;
        for (QuestItemList::const_iterator ci = currency_list->begin(); ci != currency_list->end(); ++ci)
        {
            LootItem& item = loot.items[ci->index];
            if (!ci->is_looted && !item.is_looted)
            {
                item.WriteBitDataPart(LOOT_SLOT_TYPE_OWNER, data);
                item.WriteBasicDataPart(ci->index, &currencyBuff);
                ++currenciesShown;
            }
        }
    }

    data->WriteBit(lootGuid[5]);
    data->WriteBit(guid[3]);
    data->WriteBit(lootGuid[4]);
    data->WriteBit(1); // Missing unk8
    data->WriteBit(guid[2]);

    data->FlushBits();

    // update number of items and currencies shown
    data->PutBits(itemCountPos, itemsShown, 19);
    data->PutBits(currencyCountPos, currenciesShown, 20);

    data->append(itemBuff);

    data->WriteByteSeq(lootGuid[2]);

    if (loot.gold)
        *data << uint32(loot.gold);

    data->WriteByteSeq(lootGuid[7]);
    data->WriteByteSeq(guid[5]);
    data->WriteByteSeq(lootGuid[3]);
    data->WriteByteSeq(lootGuid[4]);
    *data << uint8(lootType);
    data->WriteByteSeq(guid[4]);
    data->WriteByteSeq(lootGuid[5]);

    data->append(currencyBuff);

    data->WriteByteSeq(guid[2]);
    data->WriteByteSeq(guid[3]);
    data->WriteByteSeq(lootGuid[1]);
    data->WriteByteSeq(guid[0]);
    data->WriteByteSeq(lootGuid[0]);
    data->WriteByteSeq(guid[6]);
    data->WriteByteSeq(guid[7]);
    data->WriteByteSeq(guid[1]);
    data->WriteByteSeq(lootGuid[6]);
}

//
// --------- LootTemplate::LootGroup ---------
//

LootTemplate::LootGroup::~LootGroup()
{
    while (!ExplicitlyChanced.empty())
    {
        delete ExplicitlyChanced.back();
        ExplicitlyChanced.pop_back();
    }

    while (!EqualChanced.empty())
    {
        delete EqualChanced.back();
        EqualChanced.pop_back();
    }
}

// Adds an entry to the group (at loading stage)
void LootTemplate::LootGroup::AddEntry(LootStoreItem* item)
{
    if (item->chance != 0)
        ExplicitlyChanced.push_back(item);
    else
        EqualChanced.push_back(item);
}

// Rolls an item from the group, returns NULL if all miss their chances
LootStoreItem const* LootTemplate::LootGroup::Roll(Loot& loot, uint32 lootmode, Player* player) const
{
    // 1) Fuck floats
    const int32 precision = 10000;
    // 2) Normalize chance for guaranteed items from ExplicitlyChanced, so duplicates won't reduce total chance of looting item from 100% group
    float maxRoll = 100.0f;

    LootStoreItemList possibleLoot = ExplicitlyChanced;
    possibleLoot.remove_if(LootGroupInvalidSelector(loot, lootmode, player));

    bool guaranteedLoot = false;
    // Assume this is how we determine bosses
    if (player->GetMap()->IsDungeon())
    {
        // Before
        float sum = 0.0f;
        for (auto&& itr : ExplicitlyChanced)
            sum += itr->chance;

        // Poor man's check, but whatever
        guaranteedLoot = std::abs(sum - 100.0f) < 1.0f;
        if (guaranteedLoot && possibleLoot.size() < ExplicitlyChanced.size())
        {
            for (auto&& itr : ExplicitlyChanced)
                if (std::find(possibleLoot.begin(), possibleLoot.end(), itr) == possibleLoot.end())
                    maxRoll -= itr->chance;
        }
    }

    if (!possibleLoot.empty())                             // First explicitly chanced entries are checked
    {
        // 0 must not be included in the interval since we decreasing 'roll' to 0
        // i.e. irand(0, 100) can return both 0 and 100, and we will have 101 possible variation
        int32 roll = irand(1, (maxRoll * precision));
        for (LootStoreItemList::const_iterator itr = possibleLoot.begin(); itr != possibleLoot.end(); ++itr)   // check each explicitly chanced entry in the template and modify its chance based on quality.
        {
            LootStoreItem* item = *itr;
            if (item->chance >= 100.0f)
                return item;

            // Still error but at least we tried
            // e.g.: Someone wants 3 items to be rolled, and sets 33.3333 chance per item, but 333333 * 3 = 999999 and if we roll 1000000
            // 1000000 - 333333 - 333333 - 333333 = 1 still > 0 (same with floats, 100.0f - 33.3333333 - 33.3333333 - 33.3333333 will be like something > 0 depends on how many '3' after point chance has id DB) 
            // ceil, on the other hand, returns 333334 and even with 1000000 roll, it will be 1000000 - 333334 - 333334 - 333334 = -2
            roll -= int32(std::ceil(item->chance * precision));
            if (roll <= 0)
                return item;
        }
    }

    if (guaranteedLoot)
        TC_LOG_ERROR("shitlog", "LootTemplate::LootGroup::Roll %u", loot.sourceEntry);

    possibleLoot = EqualChanced;
    possibleLoot.remove_if(LootGroupInvalidSelector(loot, lootmode, player));
    if (!possibleLoot.empty())                              // If nothing selected yet - an item is taken from equal-chanced part
        return Trinity::Containers::SelectRandomContainerElement(possibleLoot);

    return NULL;                                            // Empty drop from the group
}

// True if group includes at least 1 quest drop entry
bool LootTemplate::LootGroup::HasQuestDrop() const
{
    for (LootStoreItemList::const_iterator i = ExplicitlyChanced.begin(); i != ExplicitlyChanced.end(); ++i)
        if ((*i)->needs_quest)
            return true;

    for (LootStoreItemList::const_iterator i = EqualChanced.begin(); i != EqualChanced.end(); ++i)
        if ((*i)->needs_quest)
            return true;

    return false;
}

// True if group includes at least 1 quest drop entry for active quests of the player
bool LootTemplate::LootGroup::HasQuestDropForPlayer(Player const* player) const
{
    for (LootStoreItemList::const_iterator i = ExplicitlyChanced.begin(); i != ExplicitlyChanced.end(); ++i)
        if (player->HasQuestForItem((*i)->itemid))
            return true;

    for (LootStoreItemList::const_iterator i = EqualChanced.begin(); i != EqualChanced.end(); ++i)
        if (player->HasQuestForItem((*i)->itemid))
            return true;

    return false;
}

void LootTemplate::LootGroup::CopyConditions(ConditionContainer /*conditions*/)
{
    for (LootStoreItemList::iterator i = ExplicitlyChanced.begin(); i != ExplicitlyChanced.end(); ++i)
        (*i)->conditions.clear();

    for (LootStoreItemList::iterator i = EqualChanced.begin(); i != EqualChanced.end(); ++i)
        (*i)->conditions.clear();
}

// Rolls an item from the group (if any takes its chance) and adds the item to the loot
void LootTemplate::LootGroup::Process(Loot& loot, uint32 lootmode, Player* player) const
{
    if (LootStoreItem const* item = Roll(loot, lootmode, player))
        loot.AddItem(*item, player);
}

// Overall chance for the group without equal chanced items
float LootTemplate::LootGroup::RawTotalChance(uint32 lootmode) const
{
    float result = 0;

    for (auto&& it : ExplicitlyChanced)
        if (it->needs_quest && it->lootmode && (it->lootmode & lootmode))
            result += it->chance;

    return result;
}

void LootTemplate::LootGroup::Verify(LootStore const& lootstore, uint32 id, uint8 group_id) const
{
    static std::vector<Difficulty> const difficulties =
    {
        REGULAR_DIFFICULTY,
        DUNGEON_DIFFICULTY_NORMAL,
        DUNGEON_DIFFICULTY_HEROIC,
        DUNGEON_DIFFICULTY_CHALLENGE,
        RAID_DIFFICULTY_10MAN_NORMAL,
        RAID_DIFFICULTY_25MAN_NORMAL,
        RAID_DIFFICULTY_10MAN_HEROIC,
        RAID_DIFFICULTY_25MAN_HEROIC,
        RAID_DIFFICULTY_25MAN_LFR,
        RAID_DIFFICULTY_1025MAN_FLEX,
        RAID_DIFFICULTY_40MAN,
        SCENARIO_DIFFICULTY_NORMAL,
        SCENARIO_DIFFICULTY_HEROIC
    };

    for (auto&& it : difficulties)
    {
        float chance = RawTotalChance(it);
        if (chance > 101.0f)                                    /// @todo replace with 100% when DBs will be ready
            TC_LOG_ERROR("sql.sql", "Table '%s' entry %u group %d has total chance > 100%% (%f)", lootstore.GetName(), id, group_id, chance);

        if (chance >= 100.0f && !EqualChanced.empty())
            TC_LOG_ERROR("sql.sql", "Table '%s' entry %u group %d has items with chance=0%% but group total chance >= 100%% (%f)", lootstore.GetName(), id, group_id, chance);
    }
}

void LootTemplate::LootGroup::CheckLootRefs(LootTemplateMap const& /*store*/, LootIdSet* ref_set) const
{
    for (LootStoreItemList::const_iterator ieItr = ExplicitlyChanced.begin(); ieItr != ExplicitlyChanced.end(); ++ieItr)
    {
        LootStoreItem* item = *ieItr;
        if (item->mincountOrRef < 0)
        {
            if (!LootTemplates_Reference.GetLootFor(-item->mincountOrRef))
                LootTemplates_Reference.ReportNotExistedId(-item->mincountOrRef);
            else if (ref_set)
                ref_set->erase(-item->mincountOrRef);
        }
    }

    for (LootStoreItemList::const_iterator ieItr = EqualChanced.begin(); ieItr != EqualChanced.end(); ++ieItr)
    {
        LootStoreItem* item = *ieItr;
        if (item->mincountOrRef < 0)
        {
            if (!LootTemplates_Reference.GetLootFor(-item->mincountOrRef))
                LootTemplates_Reference.ReportNotExistedId(-item->mincountOrRef);
            else if (ref_set)
                ref_set->erase(-item->mincountOrRef);
        }
    }
}

//
// --------- LootTemplate ---------
//

LootTemplate::~LootTemplate()
{
    for (LootStoreItemList::iterator i = Entries.begin(); i != Entries.end(); ++i)
        delete *i;

    for (size_t i = 0; i < Groups.size(); ++i)
        delete Groups[i];
}

// Adds an entry to the group (at loading stage)
void LootTemplate::AddEntry(LootStoreItem* item)
{
    if (item->group > 0 && item->mincountOrRef > 0)         // Group
    {
        if (item->group >= Groups.size())
            Groups.resize(item->group, NULL);               // Adds new group the the loot template if needed
        if (!Groups[item->group - 1])
            Groups[item->group - 1] = new LootGroup();

        Groups[item->group-1]->AddEntry(item);              // Adds new entry to the group
    }
    else                                                    // Non-grouped entries and references are stored together
        Entries.push_back(item);
}

void LootTemplate::CopyConditions(const ConditionContainer& conditions)
{
    for (LootStoreItemList::iterator i = Entries.begin(); i != Entries.end(); ++i)
        (*i)->conditions.clear();

    for (LootGroups::iterator i = Groups.begin(); i != Groups.end(); ++i)
        if (LootGroup* group = *i)
            group->CopyConditions(conditions);
}

void LootTemplate::CopyConditions(LootItem* li) const
{
    // Copies the conditions list from a template item to a LootItem
    for (LootStoreItemList::const_iterator _iter = Entries.begin(); _iter != Entries.end(); ++_iter)
    {
        LootStoreItem* item = *_iter;
        if (item->itemid != li->itemid)
            continue;

        li->conditions = item->conditions;
        break;
    }
}

// Rolls for every item in the template and adds the rolled items the the loot
void LootTemplate::Process(Loot& loot, bool rate, uint32 lootmode, uint8 groupId, Player* player) const
{
    if (ProcessScriptedLoot(loot, player))
        return;

    if (groupId)                                            // Group reference uses own processing of the group
    {
        if (groupId > Groups.size())
            return;                                         // Error message already printed at loading stage

        if (!Groups[groupId - 1])
            return;

        Groups[groupId - 1]->Process(loot, lootmode, player);
        return;
    }

    // Rolling non-grouped items
    for (LootStoreItemList::const_iterator i = Entries.begin(); i != Entries.end(); ++i)
    {
        LootStoreItem* item = *i;
        if (item->lootmode && !(item->lootmode & lootmode))
            continue;

        if (!item->Roll(rate))
            continue;                                           // Bad luck for the entry

        if (item->mincountOrRef < 0 && item->type == LOOT_ITEM_TYPE_ITEM) // References processing
        {
            LootTemplate const* Referenced = LootTemplates_Reference.GetLootFor(-item->mincountOrRef);
            if (!Referenced)
                continue;                                       // Error message already printed at loading stage

            uint32 maxcount = uint32(float(item->maxcount) * sWorld->getRate(RATE_DROP_ITEM_REFERENCED_AMOUNT));
            for (uint32 loop = 0; loop < maxcount; ++loop)      // Ref multiplicator
                Referenced->Process(loot, rate, lootmode, item->group, player);
        }
        else                                                    // Plain entries (not a reference, not grouped)
            loot.AddItem(*item, player);                        // Chance is already checked, just add
    }

    // Now processing groups
    for (LootGroups::const_iterator i = Groups.begin(); i != Groups.end(); ++i)
        if (LootGroup* group = *i)
            group->Process(loot, lootmode, player);
}

// True if template includes at least 1 quest drop entry
bool LootTemplate::HasQuestDrop(LootTemplateMap const& store, uint8 groupId) const
{
    if (groupId)                                            // Group reference
    {
        if (groupId > Groups.size())
            return false;                                   // Error message [should be] already printed at loading stage

        if (!Groups[groupId - 1])
            return false;

        return Groups[groupId-1]->HasQuestDrop();
    }

    for (LootStoreItemList::const_iterator i = Entries.begin(); i != Entries.end(); ++i)
    {
        LootStoreItem* item = *i;
        if (item->mincountOrRef < 0)                        // References
        {
            LootTemplateMap::const_iterator Referenced = store.find(-item->mincountOrRef);
            if (Referenced == store.end())
                continue;                                   // Error message [should be] already printed at loading stage
            if (Referenced->second->HasQuestDrop(store, item->group))
                return true;
        }
        else if (item->needs_quest)
            return true;                                    // quest drop found
    }

    // Now processing groups
    for (LootGroups::const_iterator i = Groups.begin(); i != Groups.end(); ++i)
        if (LootGroup* group = *i)
            if (group->HasQuestDrop())
                return true;

    return false;
}

// True if template includes at least 1 quest drop for an active quest of the player
bool LootTemplate::HasQuestDropForPlayer(LootTemplateMap const& store, Player const* player, uint8 groupId) const
{
    if (groupId)                                            // Group reference
    {
        if (groupId > Groups.size())
            return false;                                   // Error message already printed at loading stage

        if (!Groups[groupId - 1])
            return false;

        return Groups[groupId-1]->HasQuestDropForPlayer(player);
    }

    // Checking non-grouped entries
    for (LootStoreItemList::const_iterator i = Entries.begin(); i != Entries.end(); ++i)
    {
        LootStoreItem* item = *i;
        if (item->mincountOrRef < 0)                        // References processing
        {
            LootTemplateMap::const_iterator Referenced = store.find(-item->mincountOrRef);
            if (Referenced == store.end())
                continue;                                   // Error message already printed at loading stage
            if (Referenced->second->HasQuestDropForPlayer(store, player, item->group))
                return true;
        }
        else if (player->HasQuestForItem(item->itemid))
            return true;                                    // active quest drop found
    }

    // Now checking groups
    for (LootGroups::const_iterator i = Groups.begin(); i != Groups.end(); ++i)
        if (LootGroup* group = *i)
            if (group->HasQuestDropForPlayer(player))
                return true;

    return false;
}

// Checks integrity of the template
void LootTemplate::Verify(LootStore const& lootstore, uint32 id) const
{
    // Checking group chances
    for (uint32 i = 0; i < Groups.size(); ++i)
        if (Groups[i])
            Groups[i]->Verify(lootstore, id, i + 1);

    /// @todo References validity checks
}

void LootTemplate::CheckLootRefs(LootTemplateMap const& store, LootIdSet* ref_set) const
{
    for (LootStoreItemList::const_iterator ieItr = Entries.begin(); ieItr != Entries.end(); ++ieItr)
    {
        LootStoreItem* item = *ieItr;
        if (item->mincountOrRef < 0)
        {
            if (!LootTemplates_Reference.GetLootFor(-item->mincountOrRef))
                LootTemplates_Reference.ReportNotExistedId(-item->mincountOrRef);
            else if (ref_set)
                ref_set->erase(-item->mincountOrRef);
        }
    }

    for (LootGroups::const_iterator grItr = Groups.begin(); grItr != Groups.end(); ++grItr)
        if (LootGroup* group = *grItr)
            group->CheckLootRefs(store, ref_set);
}

bool LootTemplate::addConditionItem(Condition* cond)
{
    if (!cond || !cond->isLoaded())//should never happen, checked at loading
    {
        TC_LOG_ERROR("loot", "LootTemplate::addConditionItem: condition is null");
        return false;
    }

    if (!Entries.empty())
    {
        for (LootStoreItemList::iterator i = Entries.begin(); i != Entries.end(); ++i)
        {
            if ((*i)->itemid == uint32(cond->SourceEntry))
            {
                (*i)->conditions.push_back(cond);
                return true;
            }
        }
    }

    if (!Groups.empty())
    {
        for (LootGroups::iterator groupItr = Groups.begin(); groupItr != Groups.end(); ++groupItr)
        {
            LootGroup* group = *groupItr;
            if (!group)
                continue;

            LootStoreItemList* itemList = group->GetExplicitlyChancedItemList();
            if (!itemList->empty())
            {
                for (LootStoreItemList::iterator i = itemList->begin(); i != itemList->end(); ++i)
                {
                    if ((*i)->itemid == uint32(cond->SourceEntry))
                    {
                        (*i)->conditions.push_back(cond);
                        return true;
                    }
                }
            }

            itemList = group->GetEqualChancedItemList();
            if (!itemList->empty())
            {
                for (LootStoreItemList::iterator i = itemList->begin(); i != itemList->end(); ++i)
                {
                    if ((*i)->itemid == uint32(cond->SourceEntry))
                    {
                        (*i)->conditions.push_back(cond);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool LootTemplate::isReference(uint32 id)
{
    for (LootStoreItemList::const_iterator ieItr = Entries.begin(); ieItr != Entries.end(); ++ieItr)
        if ((*ieItr)->itemid == id && (*ieItr)->mincountOrRef < 0)
            return true;

    return false;//not found or not reference
}

bool LootTemplate::ProcessScriptedLoot(Loot& loot, Player* player) const
{
    // TODO: We probably need to implement LootScript and remove CanItemBeLooted or move it here
    if (loot.GetSource())
    {
        // Pandaria Herbs. Must drop at least 20 herbs
        if (loot.GetSource()->GetTypeId() == TYPEID_ITEM && loot.GetSource()->GetEntry() == 89610)
        {
            auto herbs = Entries;
            int32 totalCount = 0;
            while (!herbs.empty())
            {
                auto herb = Trinity::Containers::SelectRandomContainerElement(herbs);

                // 2 items can be produced due to max stack value
                if (herb->mincountOrRef < 0)
                {
                    TC_LOG_ERROR("loot", "Invalid loot for Pandaria Herbs");
                    continue;
                }

                int32 count = urand(herb->mincountOrRef, herb->maxcount);
                if (totalCount + count > 25)
                    count = std::min(25 - totalCount, herb->mincountOrRef);

                loot.AddItem(*herb, count);
                totalCount += count;

                if (totalCount >= 20)
                    break;

                herbs.remove(herb);
            }
            return true;
        }
    }
    return false;
}

void LoadLootTemplates_Creature()
{
    TC_LOG_INFO("server.loading", "Loading creature loot templates...");

    uint32 oldMSTime = getMSTime();

    LootIdSet lootIdSet, lootIdSetUsed;
    uint32 count = LootTemplates_Creature.LoadAndCollectLootIds(lootIdSet);

    // Remove real entries and check loot existence
    CreatureTemplateContainer const* ctc = sObjectMgr->GetCreatureTemplates();
    for (CreatureTemplateContainer::const_iterator itr = ctc->begin(); itr != ctc->end(); ++itr)
    {
        if (uint32 lootid = itr->second.lootid)
        {
            if (lootIdSet.find(lootid) == lootIdSet.end())
                LootTemplates_Creature.ReportNotExistedId(lootid);
            else
                lootIdSetUsed.insert(lootid);
        }
    }

    for (LootIdSet::const_iterator itr = lootIdSetUsed.begin(); itr != lootIdSetUsed.end(); ++itr)
        lootIdSet.erase(*itr);

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Creature.ReportUnusedIds(lootIdSet);

    if (count)
        TC_LOG_INFO("server.loading", ">> Loaded %u creature loot templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    else
        TC_LOG_ERROR("server.loading", ">> Loaded 0 creature loot templates. DB table `creature_loot_template` is empty");
}

void LoadLootTemplates_Disenchant()
{
    TC_LOG_INFO("server.loading", "Loading disenchanting loot templates...");

    uint32 oldMSTime = getMSTime();

    LootIdSet lootIdSet, lootIdSetUsed;
    uint32 count = LootTemplates_Disenchant.LoadAndCollectLootIds(lootIdSet);

    for (uint32 i = 0; i < sItemDisenchantLootStore.GetNumRows(); ++i)
    {
        ItemDisenchantLootEntry const* disenchant = sItemDisenchantLootStore.LookupEntry(i);
        if (!disenchant)
            continue;

        uint32 lootid = disenchant->Id;
        if (lootIdSet.find(lootid) == lootIdSet.end())
            LootTemplates_Disenchant.ReportNotExistedId(lootid);
        else
            lootIdSetUsed.insert(lootid);
    }

    for (LootIdSet::const_iterator itr = lootIdSetUsed.begin(); itr != lootIdSetUsed.end(); ++itr)
        lootIdSet.erase(*itr);

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Disenchant.ReportUnusedIds(lootIdSet);

    if (count)
        TC_LOG_INFO("server.loading", ">> Loaded %u disenchanting loot templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    else
        TC_LOG_ERROR("server.loading", ">> Loaded 0 disenchanting loot templates. DB table `disenchant_loot_template` is empty");
}

void LoadLootTemplates_Fishing()
{
    TC_LOG_INFO("server.loading", "Loading fishing loot templates...");

    uint32 oldMSTime = getMSTime();

    LootIdSet lootIdSet;
    uint32 count = LootTemplates_Fishing.LoadAndCollectLootIds(lootIdSet);

    // remove real entries and check existence loot
    for (uint32 i = 1; i < sAreaTableStore.GetNumRows(); ++i)
        if (AreaTableEntry const* areaEntry = sAreaTableStore.LookupEntry(i))
            if (lootIdSet.find(areaEntry->ID) != lootIdSet.end())
                lootIdSet.erase(areaEntry->ID);

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Fishing.ReportUnusedIds(lootIdSet);

    if (count)
        TC_LOG_INFO("server.loading", ">> Loaded %u fishing loot templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    else
        TC_LOG_ERROR("server.loading", ">> Loaded 0 fishing loot templates. DB table `fishing_loot_template` is empty");
}

void LoadLootTemplates_Gameobject()
{
    TC_LOG_INFO("server.loading", "Loading gameobject loot templates...");

    uint32 oldMSTime = getMSTime();

    LootIdSet lootIdSet, lootIdSetUsed;
    uint32 count = LootTemplates_Gameobject.LoadAndCollectLootIds(lootIdSet);

    // remove real entries and check existence loot
    GameObjectTemplateContainer const* gotc = sObjectMgr->GetGameObjectTemplates();
    for (GameObjectTemplateContainer::const_iterator itr = gotc->begin(); itr != gotc->end(); ++itr)
    {
        if (uint32 lootid = itr->second.GetLootId())
        {
            if (lootIdSet.find(lootid) == lootIdSet.end())
                LootTemplates_Gameobject.ReportNotExistedId(lootid);
            else
                lootIdSetUsed.insert(lootid);
        }
    }

    for (LootIdSet::const_iterator itr = lootIdSetUsed.begin(); itr != lootIdSetUsed.end(); ++itr)
        lootIdSet.erase(*itr);

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Gameobject.ReportUnusedIds(lootIdSet);

    if (count)
        TC_LOG_INFO("server.loading", ">> Loaded %u gameobject loot templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    else
        TC_LOG_ERROR("server.loading", ">> Loaded 0 gameobject loot templates. DB table `gameobject_loot_template` is empty");
}

void LoadLootTemplates_Item()
{
    TC_LOG_INFO("server.loading", "Loading item loot templates...");

    uint32 oldMSTime = getMSTime();

    LootIdSet lootIdSet;
    uint32 count = LootTemplates_Item.LoadAndCollectLootIds(lootIdSet);

    // remove real entries and check existence loot
    ItemTemplateContainer const* its = sObjectMgr->GetItemTemplateStore();
    for (ItemTemplateContainer::const_iterator itr = its->begin(); itr != its->end(); ++itr)
        if (lootIdSet.find(itr->second.ItemId) != lootIdSet.end() && itr->second.Flags & ITEM_FLAG_HAS_LOOT)
            lootIdSet.erase(itr->second.ItemId);

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Item.ReportUnusedIds(lootIdSet);

    if (count)
        TC_LOG_INFO("server.loading", ">> Loaded %u item loot templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    else
        TC_LOG_ERROR("server.loading", ">> Loaded 0 item loot templates. DB table `item_loot_template` is empty");
}

void LoadLootTemplates_Milling()
{
    TC_LOG_INFO("server.loading", "Loading milling loot templates...");

    uint32 oldMSTime = getMSTime();

    LootIdSet lootIdSet;
    uint32 count = LootTemplates_Milling.LoadAndCollectLootIds(lootIdSet);

    // remove real entries and check existence loot
    ItemTemplateContainer const* its = sObjectMgr->GetItemTemplateStore();
    for (ItemTemplateContainer::const_iterator itr = its->begin(); itr != its->end(); ++itr)
    {
        if (!(itr->second.Flags & ITEM_PROTO_FLAG_MILLABLE))
            continue;

        if (lootIdSet.find(itr->second.ItemId) != lootIdSet.end())
            lootIdSet.erase(itr->second.ItemId);
    }

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Milling.ReportUnusedIds(lootIdSet);

    if (count)
        TC_LOG_INFO("server.loading", ">> Loaded %u milling loot templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    else
        TC_LOG_ERROR("server.loading", ">> Loaded 0 milling loot templates. DB table `milling_loot_template` is empty");
}

void LoadLootTemplates_Pickpocketing()
{
    TC_LOG_INFO("server.loading", "Loading pickpocketing loot templates...");

    uint32 oldMSTime = getMSTime();

    LootIdSet lootIdSet, lootIdSetUsed;
    uint32 count = LootTemplates_Pickpocketing.LoadAndCollectLootIds(lootIdSet);

    // Remove real entries and check loot existence
    CreatureTemplateContainer const* ctc = sObjectMgr->GetCreatureTemplates();
    for (CreatureTemplateContainer::const_iterator itr = ctc->begin(); itr != ctc->end(); ++itr)
    {
        if (uint32 lootid = itr->second.pickpocketLootId)
        {
            if (lootIdSet.find(lootid) == lootIdSet.end())
                LootTemplates_Pickpocketing.ReportNotExistedId(lootid);
            else
                lootIdSetUsed.insert(lootid);
        }
    }

    for (LootIdSet::const_iterator itr = lootIdSetUsed.begin(); itr != lootIdSetUsed.end(); ++itr)
        lootIdSet.erase(*itr);

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Pickpocketing.ReportUnusedIds(lootIdSet);

    if (count)
        TC_LOG_INFO("server.loading", ">> Loaded %u pickpocketing loot templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    else
        TC_LOG_ERROR("server.loading", ">> Loaded 0 pickpocketing loot templates. DB table `pickpocketing_loot_template` is empty");
}

void LoadLootTemplates_Prospecting()
{
    TC_LOG_INFO("server.loading", "Loading prospecting loot templates...");

    uint32 oldMSTime = getMSTime();

    LootIdSet lootIdSet;
    uint32 count = LootTemplates_Prospecting.LoadAndCollectLootIds(lootIdSet);

    // remove real entries and check existence loot
    ItemTemplateContainer const* its = sObjectMgr->GetItemTemplateStore();
    for (ItemTemplateContainer::const_iterator itr = its->begin(); itr != its->end(); ++itr)
    {
        if (!(itr->second.Flags & ITEM_PROTO_FLAG_PROSPECTABLE))
            continue;

        if (lootIdSet.find(itr->second.ItemId) != lootIdSet.end())
            lootIdSet.erase(itr->second.ItemId);
    }

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Prospecting.ReportUnusedIds(lootIdSet);

    if (count)
        TC_LOG_INFO("server.loading", ">> Loaded %u prospecting loot templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    else
        TC_LOG_ERROR("server.loading", ">> Loaded 0 prospecting loot templates. DB table `prospecting_loot_template` is empty");
}

void LoadLootTemplates_Mail()
{
    TC_LOG_INFO("server.loading", "Loading mail loot templates...");

    uint32 oldMSTime = getMSTime();

    LootIdSet lootIdSet;
    uint32 count = LootTemplates_Mail.LoadAndCollectLootIds(lootIdSet);

    // remove real entries and check existence loot
    for (uint32 i = 1; i < sMailTemplateStore.GetNumRows(); ++i)
        if (sMailTemplateStore.LookupEntry(i))
            if (lootIdSet.find(i) != lootIdSet.end())
                lootIdSet.erase(i);

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Mail.ReportUnusedIds(lootIdSet);

    if (count)
        TC_LOG_INFO("server.loading", ">> Loaded %u mail loot templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    else
        TC_LOG_ERROR("server.loading", ">> Loaded 0 mail loot templates. DB table `mail_loot_template` is empty");
}

void LoadLootTemplates_Skinning()
{
    TC_LOG_INFO("server.loading", "Loading skinning loot templates...");

    uint32 oldMSTime = getMSTime();

    LootIdSet lootIdSet, lootIdSetUsed;
    uint32 count = LootTemplates_Skinning.LoadAndCollectLootIds(lootIdSet);

    // remove real entries and check existence loot
    CreatureTemplateContainer const* ctc = sObjectMgr->GetCreatureTemplates();
    for (CreatureTemplateContainer::const_iterator itr = ctc->begin(); itr != ctc->end(); ++itr)
    {
        if (uint32 lootid = itr->second.SkinLootId)
        {
            if (lootIdSet.find(lootid) == lootIdSet.end())
                LootTemplates_Skinning.ReportNotExistedId(lootid);
            else
                lootIdSetUsed.insert(lootid);
        }
    }

    for (LootIdSet::const_iterator itr = lootIdSetUsed.begin(); itr != lootIdSetUsed.end(); ++itr)
        lootIdSet.erase(*itr);

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Skinning.ReportUnusedIds(lootIdSet);

    if (count)
        TC_LOG_INFO("server.loading", ">> Loaded %u skinning loot templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    else
        TC_LOG_ERROR("server.loading", ">> Loaded 0 skinning loot templates. DB table `skinning_loot_template` is empty");
}

void LoadLootTemplates_Spell()
{
    TC_LOG_INFO("server.loading", "Loading spell loot templates...");

    uint32 oldMSTime = getMSTime();

    LootIdSet lootIdSet;
    uint32 count = LootTemplates_Spell.LoadAndCollectLootIds(lootIdSet);

    // remove real entries and check existence loot
    for (uint32 spell_id = 1; spell_id < sSpellMgr->GetSpellInfoStoreSize(); ++spell_id)
    {
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spell_id);
        if (!spellInfo)
            continue;

        // possible cases
        if (!spellInfo->IsLootCrafting())
            continue;

        if (lootIdSet.find(spell_id) == lootIdSet.end())
        {
            // not report about not trainable spells (optionally supported by DB)
            // ignore 61756 (Northrend Inscription Research (FAST QA VERSION) for example
            if (!(spellInfo->Attributes & SPELL_ATTR0_NOT_SHAPESHIFT) || (spellInfo->Attributes & SPELL_ATTR0_TRADESPELL))
            {
                LootTemplates_Spell.ReportNotExistedId(spell_id);
            }
        }
        else
            lootIdSet.erase(spell_id);
    }

    // output error for any still listed (not referenced from appropriate table) ids
    LootTemplates_Spell.ReportUnusedIds(lootIdSet);

    if (count)
        TC_LOG_INFO("server.loading", ">> Loaded %u spell loot templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    else
        TC_LOG_ERROR("server.loading", ">> Loaded 0 spell loot templates. DB table `spell_loot_template` is empty");
}

void LoadLootTemplates_Reference()
{
    TC_LOG_INFO("server.loading", "Loading reference loot templates...");

    uint32 oldMSTime = getMSTime();

    LootIdSet lootIdSet;
    LootTemplates_Reference.LoadAndCollectLootIds(lootIdSet);

    // check references and remove used
    LootTemplates_Creature.CheckLootRefs(&lootIdSet);
    LootTemplates_Fishing.CheckLootRefs(&lootIdSet);
    LootTemplates_Gameobject.CheckLootRefs(&lootIdSet);
    LootTemplates_Item.CheckLootRefs(&lootIdSet);
    LootTemplates_Milling.CheckLootRefs(&lootIdSet);
    LootTemplates_Pickpocketing.CheckLootRefs(&lootIdSet);
    LootTemplates_Skinning.CheckLootRefs(&lootIdSet);
    LootTemplates_Disenchant.CheckLootRefs(&lootIdSet);
    LootTemplates_Prospecting.CheckLootRefs(&lootIdSet);
    LootTemplates_Mail.CheckLootRefs(&lootIdSet);
    LootTemplates_Reference.CheckLootRefs(&lootIdSet);
    LootTemplates_Spell.CheckLootRefs(&lootIdSet);

    // output error for any still listed ids (not referenced from any loot table)
    LootTemplates_Reference.ReportUnusedIds(lootIdSet);

    TC_LOG_INFO("server.loading", ">> Loaded refence loot templates in %u ms", GetMSTimeDiffToNow(oldMSTime));
}

uint32 BonusLootTemplate::GetLootIdForDifficulty(Difficulty difficulty) const
{
    switch (difficulty)
    {
        case REGULAR_DIFFICULTY:
        case RAID_DIFFICULTY_25MAN_LFR:
        case RAID_DIFFICULTY_1025MAN_FLEX:
            return LootIdPersonal;
        case RAID_DIFFICULTY_10MAN_NORMAL:
        case RAID_DIFFICULTY_25MAN_NORMAL:
            return LootIdNormal;
        case RAID_DIFFICULTY_10MAN_HEROIC:
        case RAID_DIFFICULTY_25MAN_HEROIC:
            return LootIdHeroic;
        default:
            break;
    }
    return 0;
}

PersonalLoot::PersonalLoot(uint32 lootId)
{
    m_loot = sLootMgr->GetPersonalLoot(lootId);
}

void PersonalLoot::Reward(Player* player)
{
    if (!m_loot)
        return;

    bool isFlexDifficulty = player->GetMap()->GetDifficulty() == RAID_DIFFICULTY_1025MAN_FLEX;

    float percent = 1.0f;
    if (isFlexDifficulty)
    {
        uint32 members = player->GetMap()->GetPlayersCountExceptGMs();
        if (members > 10)
            percent = (1.0f * ((float)members - 10));
    }

    uint32 itemId = 0;
    if (roll_chance_i(sWorld->getFloatConfig(CONFIG_LFR_LOOT_CHANCE) + percent))
    {
        std::vector<ItemTemplate const*> items;
        for (auto&& item : m_loot->Items)
        {
            auto entry = sItemNameDescriptionStore.LookupEntry(item);
            // Flexible items must not drop not in flexible difficulty
            if (entry && strcmp(entry->Description, "Flexible") == 0 && !isFlexDifficulty) // TODO change compare mode
                continue;
            // Non flexible items must not drop in flexible difficulty
            if (entry && strcmp(entry->Description, "Flexible") != 0 && isFlexDifficulty) // TODO change compare mode
                continue;

            if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(item))
                if (player->IsItemFitToSpecialization(proto))
                    items.push_back(proto);
        }

        if (!items.empty())
        {
            ItemTemplate const* newItem = Trinity::Containers::SelectRandomContainerElement(items);
            itemId = newItem->ItemId;
        }
    }
    else
    {
        if (isFlexDifficulty)
            itemId = m_loot->MoneyBagFlex;
        else
            itemId = m_loot->MoneyBag;
    }

    if (itemId)
    {
        Item* item = Item::CreateItem(itemId, 1, player);
        player->SendDisplayToast(item, 0, 0, TOAST_TYPE_ITEM, TOAST_DISPLAY_TYPE_ITEM);
        player->StoreNewItem(item);

        uint32 questId = m_loot->QuestTracker ? m_loot->QuestTracker : 0;
        if (questId)
            if (player->GetQuestStatus(questId) != QUEST_STATUS_REWARDED)
                if (Quest const* quest = sObjectMgr->GetQuestTemplate(questId))
                    player->RewardQuest(quest, 0, player, false);
    }
}

BonusLoot::BonusLoot(uint32 lootId, Difficulty difficulty)
{
    m_test = true;
    if (auto const* loot = sLootMgr->GetBonusLoot(lootId))
    {
        m_lootTemplate = loot;
        Fill(loot, difficulty);
    }
}

BonusLoot::BonusLoot(BonusLootTemplate const* lootTemplate, Difficulty difficulty)
{
    m_lootTemplate = lootTemplate;
    Fill(lootTemplate, difficulty);
}

void BonusLoot::Fill(BonusLootTemplate const* lootTemplate, Difficulty difficulty)
{
    m_difficultyMask = 1 << difficulty;
    uint32 lootId = lootTemplate->GetLootIdForDifficulty(difficulty);
    if (lootTemplate->Source == BonusLootTemplate::Item)
    {
        if (auto const* loot = LootTemplates_Item.GetLootFor(lootId))
            Fill(loot);
    }
    else if (difficulty == RAID_DIFFICULTY_25MAN_LFR || difficulty == RAID_DIFFICULTY_1025MAN_FLEX || difficulty == REGULAR_DIFFICULTY)
    {
        if (auto const* loot = sLootMgr->GetPersonalLoot(lootId))
            for (auto&& itemId : loot->Items)
            {
                auto entry = sItemNameDescriptionStore.LookupEntry(itemId);
                // Flexible items must not drop not in flexible difficulty
                if (entry && strcmp(entry->Description, "Flexible") == 0 && difficulty != RAID_DIFFICULTY_1025MAN_FLEX) // TODO change compare mode
                    continue;
                // Non flexible items must not drop in flexible difficulty
                if (entry && strcmp(entry->Description, "Flexible") != 0 && difficulty == RAID_DIFFICULTY_1025MAN_FLEX) // TODO change compare mode
                    continue;

                if (ItemTemplate const* item = sObjectMgr->GetItemTemplate(itemId))
                    m_items.push_back(item);
            }
    }
    else if (lootTemplate->Source == BonusLootTemplate::GameObject)
    {
        if (auto const* loot = LootTemplates_Gameobject.GetLootFor(lootId))
            Fill(loot);
    }
    else if (auto const* loot = LootTemplates_Creature.GetLootFor(lootId))
        Fill(loot);
}

void BonusLoot::Reward(Player* player)
{
    if (!m_lootTemplate)
        return;

    float bonusChance = player->GetBonusRollExtraChance();

    bool alwaysLoot = m_lootTemplate->Spell == 142901 || m_test;
    bool bonusLoot = m_lootTemplate->Source != BonusLootTemplate::Item && !m_test;

    if (m_currency)
    {
        if (m_currency->mincountOrRef < 0 || uint32(m_currency->mincountOrRef) > m_currency->maxcount)
        {
            TC_LOG_ERROR("shitlog", "BonusLoot::Reward you are messed up with bonus roll %u", m_lootTemplate->LootIdPersonal);
            return;
        }
        uint32 count = urand(m_currency->mincountOrRef, m_currency->maxcount);
        player->ModifyCurrency(m_currency->itemid, count);
        player->SendDisplayToast(0, m_currency->itemid, count, TOAST_TYPE_CURRENCY, TOAST_DISPLAY_TYPE_UNK);
    }
    else if (alwaysLoot || roll_chance_f(bonusChance + sWorld->getFloatConfig(CONFIG_BONUS_ROLL_LOOT_CHANCE)))
    {
        std::list<ItemTemplate const*>* items = nullptr;
        if (m_sharedItems.empty() || !roll_chance_i(10)) // I don't know how the fuck this must be, also FIXME
            items = &m_items;
        else
            items = &m_sharedItems;

        items->remove_if([&](ItemTemplate const* item)
        {
            return !player->IsItemFitToSpecialization(item);
        });

        if (m_items.empty())
            return;

        ItemTemplate const* newItem = Trinity::Containers::SelectRandomContainerElement(*items);

        Item* item = Item::CreateItem(newItem->ItemId, 1, player);
        player->SendDisplayToast(item, 0, 0, TOAST_TYPE_ITEM, TOAST_DISPLAY_TYPE_ITEM, bonusLoot);
        player->StoreNewItem(item);
        player->SetBonusRollExtraChance(0.0f);

        if (Guild* guild = player->GetGuild())
            guild->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_CRAFT_ITEMS_GUILD, newItem->ItemId, 1, 0, nullptr, player);
    }
    else
    {
        if (m_lootTemplate->Source != BonusLootTemplate::Item)
            player->SetBonusRollExtraChance(bonusChance + sWorld->getFloatConfig(CONFIG_BONUS_ROLL_BONUS_CHANCE));

        uint32 money = urand(sWorld->getIntConfig(CONFIG_BONUS_ROLL_MONEY_MIN), sWorld->getIntConfig(CONFIG_BONUS_ROLL_MONEY_MAX));
        player->ModifyMoney(money);
        player->SendDisplayToast(nullptr, 0, money, TOAST_TYPE_MONEY, TOAST_DISPLAY_TYPE_ITEM, bonusLoot);

    }
}

void BonusLoot::Fill(LootTemplate const* lootTemplate, bool shared)
{
    for (auto&& it : lootTemplate->Entries)
        AddItem(it, shared);

    for (auto&& group : lootTemplate->Groups)
    {
        if (!group) // Legal statement, fuck trinitycore
            continue;
        for (auto&& it : group->EqualChanced)
            AddItem(it, shared);
        for (auto&& it : group->ExplicitlyChanced)
            AddItem(it, shared);
    }
}

void BonusLoot::AddItem(LootStoreItem const* lootItem, bool shared)
{
    if (lootItem->type != LOOT_ITEM_TYPE_ITEM)  // currency
    {
        m_currency = lootItem;
        return;
    }

    if (lootItem->chance < 0)                   // quest items
        return;

    if (!lootItem->conditions.empty())          // TODO: May be need check it?
        return;

    if (lootItem->itemid == 105751 || lootItem->itemid == 87777 || lootItem->itemid == 104253) // Korkron Shamans Treasure, Astral Cloud Serpent, Korkron Juggernaut
        return;

    if (!lootItem->lootmode || lootItem->lootmode & m_difficultyMask)
    {
        if (lootItem->mincountOrRef < 0)
        {
            // FIXME
            shared = lootItem->mincountOrRef == -95502 || lootItem->mincountOrRef == -96622;
            if (auto refernce = LootTemplates_Reference.GetLootFor(uint32(-lootItem->mincountOrRef)))
                Fill(refernce, shared);
        }
        else if (ItemTemplate const* item = sObjectMgr->GetItemTemplate(lootItem->itemid))
        {
            if (shared)
                m_sharedItems.push_back(item);
            else
                m_items.push_back(item);
        }
    }
}

void AELootResult::Add(Item* item, uint8 count, LootType lootType)
{
    auto itr = m_byItem.find(item);
    if (itr != m_byItem.end())
        m_byOrder[itr->second].Count += count;
    else
    {
        m_byItem[item] = m_byOrder.size();
        ResultValue value;
        value.Item = item;
        value.Count = count;
        value.Type = lootType;
        m_byOrder.push_back(value);
    }
}

std::map<uint32, WorldDropLootItem> const* LootMgr::GetWorldDrop(Creature* creature) const
{
    // TODO: rank == 6 is "weak" creatures
    if (creature->isWorldBoss() || creature->IsDungeonBoss() || creature->GetCreatureTemplate()->rank == 6)
        return nullptr;

    return &m_worldDropLoot[creature->GetMap()->GetEntry()->Expansion()];
}

bool LootMgr::IsRewardOnKill(uint32 entry)
{
    switch (entry)
    {
        case 59915: // Stone Gurad
        case 60585: // Protectors of the Endless
        case 60583: // Protectors of the Endless (Elite)
        case 60709: // Spirit Kings
        case 62983: // Lei Shi
        case 68065: // Megaera
        case 69135: // Council of Elders
        case 69473: // Ra-den
        case 71543: // Immerseus
        case 71889: // Spoils of Pandaria
        case 71161: // Paragons of the Klaxxi
            return false;
    }
    return true;
}

enum
{
    ITEM_SIGIL_OF_POWER                 = 87208,
    ITEM_SIGIL_OF_WISDOM                = 87209,
    ITEM_SECRETS_OF_THE_EMPIRE          = 94593,
    ITEM_TITANE_RUNESTONE               = 94594,
    ITEM_CHIMERA_OF_FEAR                = 87210,
    ITEM_HEART_OF_THE_THUNDER_KING      = 94867,

    QUEST_THE_STRANGTH_OF_ONES_FOES     = 31473,
    QUEST_SECRETS_OF_THE_FIRST_EMPIRE   = 32591,
    QUEST_ECHO_OF_THE_TITANS            = 32596,
    QUEST_FEAR_ITSELF                   = 31481,
    QUEST_HEART_OF_THE_THUNDER_KING     = 32597,
};

struct LegendaryQuestLootTemplate
{
    LegendaryQuestLootType Type;
    uint32 Item;
    uint32 Quest;
    uint32 MaxCount;
    float Chance;
    LootLockoutType Lockout;
};

LegendaryQuestLootTemplate const legendaryLootTemplates[]
{
    { LegendaryQuestLootType::MoguSeals,             ITEM_SIGIL_OF_POWER,            QUEST_THE_STRANGTH_OF_ONES_FOES,   10, 30.0f,  LootLockoutType::MoguSeals          },
    { LegendaryQuestLootType::MoguSeals,             ITEM_SIGIL_OF_WISDOM,           QUEST_THE_STRANGTH_OF_ONES_FOES,   10, 30.0f,  LootLockoutType::MoguSeals          },
    { LegendaryQuestLootType::SecretsOfEmpire,       ITEM_SECRETS_OF_THE_EMPIRE,     QUEST_SECRETS_OF_THE_FIRST_EMPIRE, 20, 30.0f,  LootLockoutType::SecretOfTheEmpire  },
    { LegendaryQuestLootType::TitanRunestone,        ITEM_TITANE_RUNESTONE,          QUEST_ECHO_OF_THE_TITANS,          12, 30.0f,  LootLockoutType::TitanRunestone     },
    { LegendaryQuestLootType::ChimeraOfFear,         ITEM_CHIMERA_OF_FEAR,           QUEST_FEAR_ITSELF,                 1,  100.0f, LootLockoutType::Max                },
    { LegendaryQuestLootType::HeartOfTheThunderKing, ITEM_HEART_OF_THE_THUNDER_KING, QUEST_HEART_OF_THE_THUNDER_KING,   1,  100.0f, LootLockoutType::Max                },
};

bool operator & (LegendaryQuestLootType lhs, LegendaryQuestLootType rhs)
{
    return (std::underlying_type<LegendaryQuestLootType>::type(rhs) & std::underlying_type<LegendaryQuestLootType>::type(lhs)) != 0;
}

void LootMgr::RewardLegendaryQuestLoot(Player* player, uint32 entry)
{
    auto itr = m_legendaryQuestLoot.find(entry);
    if (itr == m_legendaryQuestLoot.end())
        return;

    if (entry == 60583) // Protectors Elite
        entry = 60585;  // Use lockout for normal

    std::set<LootLockoutType> lockouts;

    for (auto&& loot : legendaryLootTemplates)
    {
        if (loot.Type & itr->second)
        {
            if (player->GetQuestStatus(loot.Quest) != QUEST_STATUS_INCOMPLETE)
                continue;

            if (loot.Lockout != LootLockoutType::Max && player->HasLootLockout(loot.Lockout, entry, REGULAR_DIFFICULTY))
                continue;

            if (loot.Lockout != LootLockoutType::Max)
                lockouts.insert(loot.Lockout);

            float chance = loot.Chance;
            if (entry == 68397 || entry == 71734) // Lei Shen, Sha of Pride
                chance = 100.0f;

            if (chance < 100.0f && player->HasAura(161780))
                chance *= 2.0f;

            if (!roll_chance_f(chance))
                continue;
            if (player->HasItemCount(loot.Item, loot.MaxCount))
                continue;

            Item* item = Item::CreateItem(loot.Item, 1);
            player->SendDisplayToast(item, loot.Item, 1, TOAST_TYPE_ITEM, TOAST_DISPLAY_TYPE_ITEM);
            player->StoreNewItem(item);

            if (Guild* guild = player->GetGuild())
                guild->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_CRAFT_ITEMS_GUILD, loot.Item, 1, 0, nullptr, player);
        }
    }

    for (auto&& itr : lockouts)
        player->AddLootLockout(itr, entry, REGULAR_DIFFICULTY);
}

void LootMgr::LoadFromDB()
{
    LoadLootTables();
    LoadCreatureLootCurrency();
    LoadPersonalLoot();
    LoadBonusLoot();
    LoadWorldDrop();
}

void LootMgr::LoadCreatureLootCurrency()
{
    uint32 oldMSTime = getMSTime();
    m_lootCurrncy.clear();

    //                                                 0        1            2        3 
    QueryResult result = WorldDatabase.Query("SELECT entry, lootmode + 0, currency, count FROM creature_loot_currency");
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 creature loot currency template. DB table `creature_loot_currency` is empty.");
        return;
    }

    do
    {
        Field* fields = result->Fetch();
        uint32 creature = fields[0].GetUInt32();
        if (!sObjectMgr->GetCreatureTemplate(creature))
        {
            TC_LOG_ERROR("sql.sql", "LootMgr::LoadCreatureLootCurrency, not existed creature entry (%u), skipped", creature);
            continue;
        }

        CreatureLootCurrencyTemplate currencyTemplate;
        currencyTemplate.Lootmode = fields[1].GetUInt32();
        currencyTemplate.Currency = fields[2].GetUInt16();
        currencyTemplate.Count = fields[3].GetUInt32();

        if (currencyTemplate.Currency && !sCurrencyTypesStore.LookupEntry(currencyTemplate.Currency))
        {
            TC_LOG_ERROR("sql.slq", "LootMgr::LoadCreatureLootCurrency, not existed currency %u for creature %u", currencyTemplate.Currency, creature);
            continue;
        }

        m_lootCurrncy[creature].push_back(currencyTemplate);
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u creature loot currency templates in %u ms.", uint32(m_lootCurrncy.size()), GetMSTimeDiffToNow(oldMSTime));
}

void LootMgr::LoadPersonalLoot()
{
    m_personalLoot.clear();

    uint32 count = 0;
    uint32 starttime = getMSTime();

    auto result = WorldDatabase.PQuery("SELECT entry, money_bag, money_bag_flex, quest, legendary_quest_loot+0 FROM personal_loot_template ");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            PersonalLootTemplate loot;
            uint32 entry = fields[0].GetUInt32();
            loot.MoneyBag  = fields[1].GetUInt32();
            loot.MoneyBagFlex = fields[2].GetUInt32();
            loot.QuestTracker = fields[3].GetUInt32();

            if (!sObjectMgr->GetItemTemplate(loot.MoneyBag))
            {
                TC_LOG_ERROR("sql.sql", "Table personal_loot_template has entry for non existing item %u", loot.MoneyBag);
                continue;
            }

            if (loot.MoneyBagFlex && !sObjectMgr->GetItemTemplate(loot.MoneyBagFlex))
            {
                TC_LOG_ERROR("sql.sql", "Table personal_loot_template has entry for non existing item %u", loot.MoneyBagFlex);
                continue;
            }

            m_personalLoot[entry] = loot;

            // Meh, good to have another table but who cares
            auto questLootType = LegendaryQuestLootType(fields[4].GetUInt32());
            if (questLootType != LegendaryQuestLootType::None)
            {
                if (questLootType >= LegendaryQuestLootType::Max)
                    TC_LOG_ERROR("sql.sql", "Table bonus_loot_template invalid legendary loot for entry %u", entry);
                else
                    m_legendaryQuestLoot[entry] = questLootType;
            }

        } while (result->NextRow());
    }

    result = WorldDatabase.PQuery("SELECT entry, item FROM personal_loot_item");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 entry = fields[0].GetUInt32();
            uint32 item = fields[1].GetUInt32();
            if (!sObjectMgr->GetItemTemplate(item))
            {
                TC_LOG_ERROR("sql.sql", "Table personal_loot_item has entry for non existing item %u", item);
                continue;
            }
            auto& loot = m_personalLoot[entry];
            loot.Items.push_back(item);
        } while (result->NextRow());
    }

    TC_LOG_INFO("server.loading", "Loaded %u personal loot templates in %u ms", uint32(m_personalLoot.size()), GetMSTimeDiffToNow(starttime));
}

void LootMgr::LoadBonusLoot()
{
    m_bonusLoot.clear();

    uint32 starttime = getMSTime();
    //                                          0       1        2           3               4              5
    auto result = WorldDatabase.PQuery("SELECT entry, spell, currency, loot_id_normal, loot_id_heroic, loot_source+0 FROM bonus_loot_template ORDER BY entry");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            BonusLootTemplate loot;
            uint32 entry = fields[0].GetUInt32();
            loot.Entry = entry;
            loot.Spell = fields[1].GetUInt32();
            loot.Currency = fields[2].GetUInt32();
            loot.LootIdNormal = fields[3].GetUInt32();
            loot.LootIdHeroic = fields[4].GetUInt32();
            loot.Source = BonusLootTemplate::LootSource(fields[5].GetUInt32());

            auto spellInfo = sSpellMgr->GetSpellInfo(loot.Spell);
            if (!spellInfo || !spellInfo->HasAura(SPELL_AURA_BONUS_ROLL_TRIGGER))
            {
                TC_LOG_ERROR("sql.sql", "Table bonus_loot_template has invalid spell for entry %u", entry);
                continue;
            }
            if (!sCurrencyTypesStore.LookupEntry(loot.Currency))
            {
                TC_LOG_ERROR("sql.sql", "Table bonus_loot_template has invalid currency for %u", entry);
                continue;
            }
            if (loot.Source != BonusLootTemplate::Creautre && loot.Source != BonusLootTemplate::GameObject)
            {
                TC_LOG_ERROR("sql.sql", "Table bonus_loot_template invalid loot source for entry %u", entry);
                continue;
            }
            loot.LootIdPersonal = GetPersonalLoot(entry) ? entry : 0;   // Yep
            m_bonusLoot[entry] = loot;

        } while (result->NextRow());
    }

    TC_LOG_INFO("server.loading", "Loaded %u bonus loot templates in %u ms", uint32(m_bonusLoot.size()), GetMSTimeDiffToNow(starttime));
}

void LootMgr::LoadWorldDrop()
{
    for (auto&& itr : m_worldDropLoot)
        itr.clear();

    uint32 starttime = getMSTime();
    uint32 count = 0;
    //                                              0         1         2          3             4          5
    auto result = WorldDatabase.PQuery("SELECT `expansion`, `item`,  `chance`, `level_min`, `level_max`, `group` FROM world_drop_loot_template");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            Expansions expansion = Expansions(fields[0].GetUInt8());
            if (expansion >= MAX_EXPANSIONS)
            {
                TC_LOG_ERROR("sql.sql", "LootMgr::LoadWorldDrop invalid expansion for item %u: %u", fields[1].GetUInt32(), expansion);
                continue;
            }

            auto itr = m_worldDropLoot[expansion].find(fields[1].GetUInt32());
            if (itr != m_worldDropLoot[expansion].end())
            {
                if (itr->second.Group != fields[5].GetUInt32())
                {
                    TC_LOG_ERROR("sql.sql", "LootMgr::LoadWorldDrop item %u has different groups (in one expansion)", fields[1].GetUInt32());
                    continue;
                }
                itr->second.AddDrop(fields[3].GetUInt8(), fields[4].GetUInt8(), fields[2].GetFloat());
            }
            else
            {
                auto& item = m_worldDropLoot[expansion][fields[1].GetUInt32()];
                item.Group = fields[5].GetUInt32();
                item.Item = fields[1].GetUInt32();
                item.AddDrop(fields[3].GetUInt8(), fields[4].GetUInt8(), fields[2].GetFloat());
            }
            ++count;

        } while (result->NextRow());
    }

    TC_LOG_INFO("server.loading", "Loaded %u world drop loot templates in %u ms", count, GetMSTimeDiffToNow(starttime));
}
