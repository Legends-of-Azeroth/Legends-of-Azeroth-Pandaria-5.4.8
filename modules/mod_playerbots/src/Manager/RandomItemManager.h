#ifndef _PLAYERBOT_RANDOMITEMMGR_H
#define _PLAYERBOT_RANDOMITEMMGR_H

#include <map>
#include <set>
#include <unordered_set>
#include <vector>

#include "AiFactory.h"
#include "Common.h"
#include "Item.h"
#include "Player.h"

enum item_sub_class : uint32
{
    PLATE = 0,
    MAIL,
    LEATHER,
    CLOTH
};

class RandomItemManager
{
public:
    RandomItemManager();
    void Init();
    virtual ~RandomItemManager();
    static RandomItemManager* instance()
    {
        static RandomItemManager instance;
        return &instance;
    }

    uint32 FindBestItemForLevelAndEquip(Player* bot, InventoryType invType);
    bool IsTestItem(uint32 itemId) { return _itemForTest.find(itemId) != _itemForTest.end(); }

    std::vector<uint32> GetCachedEquipments(uint32 requiredLevel, uint32 inventoryType);

    bool CanEquipArmor(Player* bot, ItemTemplate const* proto);
    bool CanEquipWeapon(Player* bot, ItemTemplate const* proto);
    bool CanEquipSubArmor(Player* bot, ItemTemplate const* proto);
private:
    void AddItemStats(uint32 mod, uint8& sp, uint8& ap, uint8& tank);
    bool CanEquipItem(EquipmentSlots slot, ItemTemplate const* proto);
    
    bool CheckItemStats(Classes clazz, uint8 sp, uint8 ap, uint8 tank);
    bool ShouldEquipArmorForSpec(uint32 level, Classes playerclass, Specializations spec, ItemTemplate const* proto);
    bool ShouldEquipWeaponForSpec(Classes playerclass, Specializations spec, ItemTemplate const* proto);
private:
    std::map<uint32, std::map<uint32, std::vector<uint32>>> _equipCacheNew;
    std::map<EquipmentSlots, std::set<InventoryType>> _viable_slots;
    std::unordered_set<uint32> _itemForTest;
};

#define sRandomItemMgr RandomItemManager::instance()

#endif
