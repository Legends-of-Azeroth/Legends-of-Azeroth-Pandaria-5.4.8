-- Shu
UPDATE `creature` SET `position_x` = 928.6078, `position_y` = 3602.659, `position_z` = 195.2873, `orientation` = 4.867653, `wander_distance` = 0, `movement_type` = 2 WHERE `guid` IN (561812, 561707);

DELETE FROM `creature_addon` WHERE `guid` IN (561812, 561707, 561706, 561795, 561708, 561817);
INSERT INTO `creature_addon` (`guid`, `path_id`, `bytes1`, `bytes2`, `auras`) VALUES
(561812, 561812, 0, 1, "60921"),
(561707, 561707, 0, 1, "49415"),
(561706, 0, 50397184, 1, "49415"),
(561795, 0, 50397184, 1, "60921"),
(561708, 0, 3, 1, "49415"),
(561817, 0, 3, 1, "60921");

DELETE FROM `waypoint_data` WHERE `id` IN (561812, 561707);
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `action`, `action_chance`, `wpguid`, `entry`) VALUES
(561812, 1, 929.0764, 3599.665, 195.4497, 0, 0, 1, 0, 100, 0, 60918),
(561812, 2, 928.6511, 3606.639, 195.3147, 0, 0, 1, 0, 100, 0, 60918),
(561812, 3, 929.3073, 3609.622, 195.3814, 0, 0, 1, 0, 100, 0, 60918),

(561707, 1, 929.0764, 3599.665, 195.4497, 0, 0, 1, 0, 100, 0, 60918),
(561707, 2, 928.6511, 3606.639, 195.3147, 0, 0, 1, 0, 100, 0, 60918),
(561707, 3, 929.3073, 3609.622, 195.3814, 0, 0, 1, 0, 100, 0, 60918);

-- Big Bertha
DELETE FROM `creature` WHERE `guid` IN (563641, 563642) AND `id` = 57853;
DELETE FROM `creature_addon` WHERE `guid` IN (563641, 563642);
UPDATE `creature` SET `position_x` = 582.2813, `position_y` = 3123.172, `position_z` = 89.2438, `orientation` = 2.268393 WHERE `guid` = 563622;

-- Nourished Yak & Delivery Cart
DELETE FROM `conditions` WHERE `SourceEntry` IN (108877, 108878) AND `SourceGroup` = 1 AND `SourceTypeOrReferenceId` = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(13, 1, 108877, 0, 1, 31, 0, 3, 59499, 0, 0, 0, 0, "", "Ox Cart Rope Left target Nourished Yak"),
(13, 1, 108877, 0, 0, 31, 0, 3, 57709, 0, 0, 0, 0, "", "Ox Cart Rope Left target Nourished Yak"),
(13, 1, 108878, 0, 0, 31, 0, 3, 57710, 0, 0, 0, 0, "", "Ox Cart Rope Right target Delivery Cart"),
(13, 1, 108878, 0, 1, 31, 0, 3, 59497, 0, 0, 0, 0, "", "Ox Cart Rope Right target Delivery Cart");

UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` IN (59497, 57710);
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (59497, 57710) AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (5949700, 5771000) AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(59497, 0, 0, 0, 11, 0, 100, 0, 0, 0, 0, 0, 80, 5949700, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Delivery Cart - On Respawn - Start Script"),
(59497, 0, 1, 0, 31, 0, 100, 0, 108877, 0, 0, 0, 85, 108878, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Delivery Cart - On Spellhit Target - Invoker Cast Ox Cart Rope Right"),
(5949700, 9, 0, 0, 0, 0, 100, 0, 500, 500, 0, 0, 11, 108877, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Delivery Cart - Cast Ox Cart Rope Left"),

(57710, 0, 0, 0, 11, 0, 100, 0, 0, 0, 0, 0, 80, 5771000, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Delivery Cart - On Respawn - Start Script"),
(57710, 0, 1, 0, 31, 0, 100, 0, 108877, 0, 0, 0, 85, 108878, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Delivery Cart - On Spellhit Target - Invoker Cast Ox Cart Rope Right"),
(5771000, 9, 0, 0, 0, 0, 100, 0, 500, 500, 0, 0, 11, 108877, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Delivery Cart - Cast Ox Cart Rope Left");

UPDATE `creature` SET `wander_distance` = 50, `walk_mode` = 1 WHERE `id` = 64442;

-- Water Pincer
UPDATE `creature_template_addon` SET `auras` = "116552" WHERE `entry` = 60411;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 60411 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(60411, 0, 0, 0, 0, 0, 100, 0, 2700, 3600, 18000, 18000, 11, 128448, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, "Water Pincer - In Combat - Cast Phlogiston"),
(60411, 0, 1, 0, 1, 0, 100, 0, 2000, 10000, 8000, 10000, 128, 2354, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Water Pincer - OOC - Play Anim Kit");
