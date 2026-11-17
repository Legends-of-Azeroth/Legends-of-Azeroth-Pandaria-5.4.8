UPDATE `creature_addon` SET `path_id` = 180224, `emote` = 0 WHERE `guid` = 180224;
UPDATE `creature` SET `position_x` = -9462.29, `position_y` = 87.8151, `position_z` = 58.4211, `orientation` = 0.733038, `wander_distance` = 0, `movement_type` = 2 WHERE `guid` = 180224;

UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` = 2046;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 2046 AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (2046*100, 2046*100+01) AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(2046, 0, 0, 0, 34, 0, 100, 0, 2, 1, 0, 0, 80, 2046*100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Andrew Krighton - On WP 1 Reached - Start Script"),
(2046, 0, 1, 0, 34, 0, 100, 0, 2, 3, 0, 0, 80, 2046*100+1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Andrew Krighton - On WP 3 Reached - Start Script"),

(2046*100, 9, 0, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 2.37365, "Andrew Krighton - On Script - Set Orientation (2.37365)"),
(2046*100, 9, 1, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 17, 233, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Andrew Krighton - On Script - Set Emote State (233)"),
(2046*100, 9, 2, 0, 0, 0, 100, 0, 17000, 17000, 0, 0, 17, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Andrew Krighton - On Script - Set Emote State (0)"),

(2046*100+01, 9, 0, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0.733038, "Andrew Krighton - On Script - Set Orientation (0.733038)");

DELETE FROM `waypoint_data` WHERE `id` = 180224;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `delay`, `entry`) VALUES
(180224, 1, -9464.05957, 88.933998, 58.3437, 0, 2046),
(180224, 2, -9464.05957, 88.933998, 58.3437, 20000, 2046),
(180224, 3, -9462.29, 87.8151, 58.4211, 0, 2046),
(180224, 4, -9462.29, 87.8151, 58.4211, 60000, 2046);
