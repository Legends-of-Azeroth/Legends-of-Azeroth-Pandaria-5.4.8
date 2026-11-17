-- borrando montura del Sha de la ira
DELETE FROM `personal_loot_item` WHERE entry = 60491 AND item = 87771;

-- borrando montura de galeon
DELETE FROM `creature_loot_template` WHERE entry = 62346 AND item = 89783;
DELETE FROM `personal_loot_item` WHERE entry = 62346 AND item = 89783;
