-- Corina Steele
UPDATE `creature` SET `position_x` = -9464.87, `position_y` = 93.9896, `position_z` = 58.5271, `orientation` = 0, `wander_distance` = 0, `movement_type` = 2 WHERE `guid` = 177324;

UPDATE `creature_addon` SET `path_id` = 177324 WHERE `guid` = 177324;

UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` = 54;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 54 AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (54*100, 54*100+01) AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(54, 0, 0, 0, 34, 0, 100, 0, 2, 1, 0, 0, 80, 54*100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Corina Steele - On WP 1 Reached - Start Script"),
(54, 0, 1, 0, 34, 0, 100, 0, 2, 3, 0, 0, 80, 54*100+01, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Corina Steele - On WP 3 Reached - Start Script"),

(54*100, 9, 0, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0.811598, "Corina Steele - On Script - Set Orientation (0.811598)"),
(54*100, 9, 1, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 17, 233, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Corina Steele - On Script - Set Emote State (233)"),
(54*100, 9, 2, 0, 0, 0, 100, 0, 27000, 27000, 0, 0, 17, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Corina Steele - On Script - Set Emote State (0)"),

(54*100+01, 9, 0, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 3.124597, "Corina Steele - On Script - Set Orientation (3.124597)"),
(54*100+01, 9, 1, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 17, 69, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Corina Steele - On Script - Set Emote State (69)"),
(54*100+01, 9, 2, 0, 0, 0, 100, 0, 16000, 16000, 0, 0, 17, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Corina Steele - On Script - Set Emote State (0)"),
(54*100+01, 9, 3, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0.00000, "Corina Steele - On Script - Set Orientation (0.00000)");

DELETE FROM `waypoint_data` WHERE `id` = 177324;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `delay`, `entry`) VALUES
(177324, 1, -9460.31543, 101.147774, 58.344334, 0, 54),
(177324, 2, -9460.31543, 101.147774, 58.344334, 30000, 54),
(177324, 3, -9464.87, 93.9896, 58.5271, 0, 54),
(177324, 4, -9464.87, 93.9896, 58.5271, 80000, 54);

-- Lyria Du Lac & Brother Wilhelm
UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` IN (913, 927);
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (913, 927) AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(913, 0, 0, 0, 60, 0, 100, 0, 7000, 12000, 7000, 12000, 10, 396, 273, 274, 6, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lyria Du Lac - On Update - Play Random Emote (396, 273, 274, 6)"),
(927, 0, 0, 0, 60, 0, 100, 0, 7000, 12000, 7000, 12000, 10, 396, 273, 274, 6, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Brother Wilhelm - On Update - Play Random Emote (396, 273, 274, 6)");

-- Benjamin Foxworthy
UPDATE `creature_template_addon` SET `bytes2` = 2, `emote` = 214 WHERE `entry` = 46983;

UPDATE creature_template SET AIName = "SmartAI" WHERE entry = 46983;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 46983;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(46983, 0, 0, 0, 1, 0, 100, 0, 1, 1, 10000, 10000, 11, 45425, 0, 0, 0, 0, 0, 11, 46985, 25, 0, 0, 0, 0, 0, "Benjamin Foxworthy - OOC - Cast Spell 'Shot'");

UPDATE `creature_template` SET `flags_extra` = 128, `InhabitType` = 4 WHERE `entry` = 46985;
UPDATE `creature` SET `wander_distance` = 0, `movement_type` = 0 WHERE `guid` = 177316;

-- Tharynn Bouden & Lien Farner
UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` IN (66, 47384);
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (66, 47384) AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(66, 0, 0, 0, 60, 0, 100, 0, 7000, 12000, 7000, 12000, 10, 396, 273, 274, 6, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Tharynn Bouden - On Update - Play Random Emote (396, 273, 274, 6)"),
(47384, 0, 0, 0, 60, 0, 100, 0, 7000, 12000, 7000, 12000, 10, 396, 273, 274, 6, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Lien Farner - On Update - Play Random Emote (396, 273, 274, 6)");
