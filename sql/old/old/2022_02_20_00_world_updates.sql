DROP TABLE IF EXISTS `world_map_template`;
CREATE TABLE `world_map_template`(
    `mapID` INT(11) UNSIGNED NOT NULL DEFAULT "0",
    `ScriptName` TEXT,
    PRIMARY KEY (`mapID`));

DELETE FROM `world_map_template` WHERE `mapID` = 654;
INSERT INTO `world_map_template` (`mapID`, `ScriptName`) VALUES
(654, "world_map_gilneas");
