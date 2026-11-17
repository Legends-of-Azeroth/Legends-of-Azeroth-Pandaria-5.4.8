ALTER TABLE `creature` CHANGE COLUMN `spawndist` `wander_distance` FLOAT NOT NULL DEFAULT "0" AFTER `spawntimesecs_max`;
ALTER TABLE `creature` CHANGE COLUMN `MovementType` `movement_type` TINYINT(3) unsigned NOT NULL DEFAULT "0" AFTER `curmana`;
ALTER TABLE `creature_text` CHANGE COLUMN `groupid` `text_group` TINYINT(3) unsigned NOT NULL DEFAULT "0" AFTER `entry`;
ALTER TABLE `creature_text` CHANGE COLUMN `TextRange` `text_range` TINYINT(3) unsigned NOT NULL DEFAULT "0" AFTER `text_female`;
ALTER TABLE `locales_creature_text` CHANGE COLUMN `groupid` `text_group` TINYINT(3) unsigned NOT NULL DEFAULT "0" AFTER `entry`;

DELETE FROM `command` WHERE `name` IN ("npc set spawndist", "npc set wanderdistance");
INSERT INTO `command` (`name`, `security`, `help`) VALUES
("npc set wanderdistance", 5, "Syntax: .npc set wanderdistance #dist\r\n\r\nAdjust wander distance of selected creature to dist.");

UPDATE `trinity_string` SET `content_default` = "Wander distance changed to: %f", `content_loc8` = "Дистанция брождения изменена на: %f" WHERE `entry` = 297;
