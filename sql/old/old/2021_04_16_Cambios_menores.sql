/*
precios:
	2256 - 6619 04
	2543 - 3500 00
	3879 - 1750 00
*/
-- actualizando precios del intendente de honor, SOLO EJECUTAR UNA VEZ!!!
UPDATE `npc_vendor` SET `ExtendedCost` = 3879 WHERE `ExtendedCost` = 2543 AND `entry` IN (66001, 66049);
UPDATE `npc_vendor` SET `ExtendedCost` = 2543 WHERE `ExtendedCost` = 2256 AND `entry` IN (66001, 66049);

-- Creature Galeon - Id : 89783 , se redujo el % de la montura
UPDATE `creature_loot_template` SET `ChanceOrQuestChance` = 1 WHERE `entry` = 62346 AND item=89783;
