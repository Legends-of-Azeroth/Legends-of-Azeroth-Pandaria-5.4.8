UPDATE `creature` SET `position_x` = -8824.64, `position_y` = 619.983, `position_z` = 94.4927, `orientation` = 0.959931, `spawndist` = 0, `MovementType` = 2 WHERE `guid` = 188769 AND `id` = 1976;

DELETE FROM `creature_addon` WHERE `guid` = 188769;
INSERT INTO `creature_addon` (`guid`, `path_id`, `bytes1`, `bytes2`, `auras`) VALUES
(188769, 188769, 0, 257, "");

DELETE FROM `waypoint_data` WHERE `id` = 188769;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `action`, `action_chance`, `wpguid`, `entry`) VALUES
(188769, 1, -8824.64, 619.983, 94.4927, 0, 0, 0, 0, 100, 0, 1976),
(188769, 2, -8825.31, 615.174, 94.8533, 0, 0, 0, 0, 100, 0, 1976),
(188769, 3, -8827.04, 611.305, 94.6672, 0, 0, 0, 0, 100, 0, 1976),
(188769, 4, -8827.04, 611.305, 94.6672, 0.959931, 18000, 0, 0, 100, 0, 1976),
(188769, 5, -8825.54, 619.632, 94.5323, 0, 0, 0, 0, 100, 0, 1976),
(188769, 6, -8826.18, 624.085, 94.3282, 0, 0, 0, 0, 100, 0, 1976),
(188769, 7, -8825.56, 626.854, 94.4532, 0, 0, 0, 0, 100, 0, 1976),
(188769, 8, -8824.12, 628.957, 94.4532, 0, 0, 0, 0, 100, 0, 1976),
(188769, 9, -8824.12, 628.957, 94.4532, 0.767945, 19000, 0, 0, 100, 0, 1976);

DELETE FROM `creature_equip_template` WHERE `entry` = 1976 AND `ID` = 3;
INSERT INTO `creature_equip_template` (`entry`, `id`, `itemEntry1`, `itemEntry2`, `itemEntry3`) VALUES
(1976, 3, 1902, 0, 2551);

UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE `entry` = 1976;
DELETE FROM `smart_scripts` WHERE `entryorguid` = -188769 AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 1976*100 AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(-188769, 0, 0, 0, 34, 0, 100, 0, 2, 8, 0, 0, 80, 1976*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Stormwind City Patroller - MI 8 Point - Start Script"),

(1976*100, 9, 0, 0, 0, 0, 100, 0, 7000, 7000, 0, 0, 71, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Stormwind City Patroller - On Script - Equip (3)"),
(1976*100, 9, 1, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 17, 234, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Stormwind City Patroller - On Script - Set Emote State (234)"),
(1976*100, 9, 2, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 17, 26, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Stormwind City Patroller - On Script - Set Emote State (26)"),
(1976*100, 9, 3, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 71, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Stormwind City Patroller - On Script - Equip (1)");
