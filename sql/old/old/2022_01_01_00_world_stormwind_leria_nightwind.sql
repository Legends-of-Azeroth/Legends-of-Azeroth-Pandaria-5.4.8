UPDATE `creature` SET `position_x` = -8722.81, `position_y` = 522.099, `position_z` = 98.8999, `orientation` = 1.84176 WHERE `id` = 61837;

DELETE FROM `creature_addon` WHERE `guid` IN (SELECT `guid` FROM `creature` WHERE `id` = 61837);
INSERT INTO `creature_addon` (`guid`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `auras`) VALUES
((SELECT `guid` FROM `creature` WHERE `id` = 61837), 0, 0, 0, 257, 426, NULL);
