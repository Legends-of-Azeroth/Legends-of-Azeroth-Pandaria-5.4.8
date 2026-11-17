-- Change column names to match WPP generate SQL
-- Add `VerifiedBuild` to gameobject table
ALTER TABLE `gameobject`
ADD COLUMN `VerifiedBuild` int NOT NULL DEFAULT 0 AFTER `ScriptName`;

-- Add `VerifiedBuild` to creature table, change column movement_type to MovementType
ALTER TABLE `creature`
CHANGE COLUMN `movement_type` `MovementType` tinyint UNSIGNED NOT NULL DEFAULT 0 AFTER `curmana`,
ADD COLUMN `VerifiedBuild` int NOT NULL DEFAULT 0 AFTER `walk_mode`;

-- Rename creature_template table baseattacktime and rangeattacktime
ALTER TABLE `creature_template` 
CHANGE COLUMN `baseattacktime` `BaseAttackTime` int UNSIGNED NOT NULL DEFAULT 0 AFTER `dmg_multiplier`,
CHANGE COLUMN `rangeattacktime` `RangeAttackTime` int UNSIGNED NOT NULL DEFAULT 0 AFTER `BaseAttackTime`;