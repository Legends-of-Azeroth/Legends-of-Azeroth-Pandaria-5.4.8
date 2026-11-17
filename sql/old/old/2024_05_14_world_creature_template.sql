-- Drop column faction_H and change faction_A to faction
ALTER TABLE `creature_template` DROP COLUMN `faction_H`;
ALTER TABLE `creature_template` CHANGE COLUMN `faction_A` `faction` SMALLINT(5) UNSIGNED NOT NULL DEFAULT '0';
