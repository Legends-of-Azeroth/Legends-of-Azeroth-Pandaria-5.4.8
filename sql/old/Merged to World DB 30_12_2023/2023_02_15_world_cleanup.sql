-- Remove non exist creature entry 33150
DELETE FROM `creature_template_addon` WHERE `entry`=33150;

-- Remove non exist creature entry 66001,66033,66049,66076,66098,66103,190058,190060
DELETE FROM `creature` WHERE `id` IN (66001,66033,66049,66076,66098,66103,190058,190060);

-- Remove non exist creature record in creature_addon
DELETE FROM `creature_addon` WHERE `guid` IN (315824,315825,315851,315854,316785,316786,357467);

-- Remove non exist creature record in creature of entry Minigob Manabonk <The Mischievous Mage> (32838)
DELETE FROM `pool_creature` WHERE `guid` IN (54462,54527,54528,54531,54532,54553);

-- Remove non exist quest in pool_quest
DELETE FROM `pool_quest` WHERE `entry` IN(40313,40325,40326,40327);

-- Fix SmartAI of Earthen Familiar(73556)
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=73556;
-- Fix SmartAI of Water Familiar(73559)
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=73559;
-- Fix SmartAI of Fire Familiar(73560)
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=73560;

