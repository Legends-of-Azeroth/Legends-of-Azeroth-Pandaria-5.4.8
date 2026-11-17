-- Add column BaseVariance, RangeVariance, DamageModifier
ALTER TABLE `creature_template` 
ADD COLUMN `BaseVariance` float NOT NULL DEFAULT 1 AFTER `rangeattacktime`,
ADD COLUMN `RangeVariance` float NOT NULL DEFAULT 1 AFTER `BaseVariance`,
ADD COLUMN `DamageModifier` float NOT NULL DEFAULT 1 AFTER `Armor_mod`;

ALTER TABLE `creature_template` 
DROP COLUMN `mindmg`,
DROP COLUMN `maxdmg`,
DROP COLUMN `minrangedmg`,
DROP COLUMN `maxrangedmg`;

-- Thunderfury, Blessed Blade of the Windseeker Chance on hit effect. Thanks Conna
DELETE FROM `item_template_addon` WHERE `Id`=19019;
INSERT INTO `item_template_addon` (`Id`, `FlagsCu`, `FoodType`, `MinMoneyLoot`, `MaxMoneyLoot`, `SpellPPMChance`) 
VALUES (19019, 0, 0, 0, 0, 4);

-- Quest Much to Learn(30039) . Thanks jasondove
UPDATE `quest_template` 
SET `OfferRewardText`='Today is the day when your fate will be decided. Do you have the strength and cunning to be a $c? Or are you destined to a life working in the fields?$B$BEach has its place, but I can sense that you aspire to greatness.$B$BLet us begin our training, my newest pupil.' 
WHERE `Id`=30039;
