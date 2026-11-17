UPDATE `creature` SET `wander_distance` = 0, `movement_type` = 0 WHERE `guid` IN (221134, 221132, 221131, 221135, 221133, 220259, 220570, 220571, 220569, 220568,
                                                                                  220488, 220490, 220489, 220492, 220491);

UPDATE `game_weather` SET `spring_rain_chance` = 20, `summer_rain_chance` = 15, `fall_rain_chance` = 20, `winter_rain_chance` = 20, `winter_snow_chance` = 0 WHERE `zone` = 85;
UPDATE `creature_template` SET `unit_flags` = 32768 WHERE `entry` = 1934;

UPDATE `creature` SET `position_x` = 2220.16, `position_y` = 1034.98, `position_z` = 35.4363, `wander_distance` = 0, `movement_type` = 2 WHERE `guid` = 192606 AND `id` = 10666;

DELETE FROM `creature_addon` WHERE `guid` = 192606;
INSERT INTO `creature_addon` (`guid`, `path_id`, `bytes2`) VALUES
(192606, 192606, 1);

DELETE FROM `waypoint_data` WHERE `id` = 192606;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `action`, `action_chance`, `wpguid`) VALUES
(192606, 1, 2220.16, 1034.98, 35.4363, 0, 0, 0, 0, 100, 0),
(192606, 2, 2215.4, 1034.4, 35.4941, 0, 0, 0, 0, 100, 0),
(192606, 3, 2210.6, 1032.61, 35.5099, 0, 0, 0, 0, 100, 0),
(192606, 4, 2212.62, 1038.43, 34.859, 0, 0, 0, 0, 100, 0),
(192606, 5, 2211.79, 1034.79, 35.322, 0, 0, 0, 0, 100, 0),
(192606, 6, 2212.88, 1038.84, 34.8025, 0, 0, 0, 0, 100, 0),
(192606, 7, 2211.61, 1034.22, 35.3814, 0, 0, 0, 0, 100, 0),
(192606, 8, 2217.5, 1035.07, 35.4439, 0, 0, 0, 0, 100, 0),
(192606, 9, 2222.84, 1034.61, 35.5158, 0, 0, 0, 0, 100, 0),
(192606, 10, 2228.76, 1032.47, 35.7824, 0, 0, 0, 0, 100, 0),
(192606, 11, 2239.25, 1024.1, 36.4546, 0, 0, 0, 0, 100, 0),
(192606, 12, 2243.63, 1018.43, 36.6652, 0, 0, 0, 0, 100, 0),
(192606, 13, 2243.18, 1004.55, 36.5792, 0, 0, 0, 0, 100, 0),
(192606, 14, 2232.18, 994.276, 36.727, 0, 0, 0, 0, 100, 0),
(192606, 15, 2241.75, 1006.5, 36.6768, 0, 0, 0, 0, 100, 0),
(192606, 16, 2243.38, 1017.61, 36.7298, 0, 0, 0, 0, 100, 0),
(192606, 17, 2240.62, 1023.92, 36.4588, 0, 0, 0, 0, 100, 0),
(192606, 18, 2226.9, 1033.25, 35.6483, 0, 0, 0, 0, 100, 0);

-- Groundkeeper Shen
UPDATE `creature` SET `position_x` = 801.0035, `position_y` = 3632.609, `position_z` = 160.5113, `orientation` = 1.797689, `wander_distance` = 0, `movement_type` = 2 WHERE `guid` = 562753;

UPDATE `creature_template_addon` SET `path_id` = 562753 WHERE `entry` = 65088;

DELETE FROM `waypoint_data` WHERE `id` = 562753;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `action`, `action_chance`, `wpguid`, `entry`) VALUES
(562753, 1, 793.4775, 3634.164, 160.5113, 0, 0, 0, 0, 100, 0, 65088),
(562753, 2, 790.5261, 3638.889, 160.5113, 0, 0, 0, 0, 100, 0, 65088),
(562753, 3, 790.6962, 3643.549, 160.5113, 0, 0, 0, 0, 100, 0, 65088),
(562753, 4, 792.6233, 3647.6, 160.5113, 5.585053, 20000, 0, 0, 100, 0, 65088),
(562753, 5, 801.6996, 3650.888, 160.5113, 0, 0, 0, 0, 100, 0, 65088),
(562753, 6, 805.7153, 3648.389, 160.5172, 3.839724, 17000, 0, 0, 100, 0, 65088),
(562753, 7, 800.2031, 3650.989, 160.5113, 0, 0, 0, 0, 100, 0, 65088),
(562753, 8, 794.257, 3649.206, 160.5113, 0, 0, 0, 0, 100, 0, 65088),
(562753, 9, 791.4861, 3645.849, 160.5113, 0, 0, 0, 0, 100, 0, 65088),
(562753, 10, 790.7587, 3641.228, 160.5113, 6.213372, 12000, 0, 0, 100, 0, 65088),
(562753, 11, 793.658, 3634.673, 160.5113, 0, 0, 0, 0, 100, 0, 65088),
(562753, 12, 796.7413, 3632.884, 160.5113, 0, 0, 0, 0, 100, 0, 65088),
(562753, 13, 801.0035, 3632.609, 160.5113, 1.797689, 16000, 0, 0, 100, 0, 65088);

-- Bai Hua
DELETE FROM `creature` WHERE `guid` = 562771 AND `id` = 65053;
DELETE FROM `creature_addon` WHERE `guid` = 562771;
UPDATE `creature` SET `position_x` = 740.3411, `position_y` = 3660.361, `position_z` = 154.1399, `orientation` = 4.331117, `wander_distance` = 0, `movement_type` = 2 WHERE `guid` = 562745;

UPDATE `creature_template_addon` SET `path_id` = 562745 WHERE `entry` = 65053;

DELETE FROM `waypoint_data` WHERE `id` = 562745;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `action`, `action_chance`, `wpguid`, `entry`) VALUES
(562745, 1, 738.8229, 3656.574, 154.135, 0, 0, 0, 0, 100, 0, 65053),
(562745, 2, 738.0156, 3653.259, 154.134, 0, 0, 0, 0, 100, 0, 65053),
(562745, 3, 741.1893, 3651.445, 154.1361, 0, 0, 0, 0, 100, 0, 65053),
(562745, 4, 745.1597, 3650.274, 154.1321, 0, 0, 0, 0, 100, 0, 65053),
(562745, 5, 748.743, 3651.259, 154.137, 0, 0, 0, 0, 100, 0, 65053),
(562745, 6, 749.0729, 3653.79, 154.137, 0, 0, 0, 0, 100, 0, 65053),
(562745, 7, 747.6545, 3656.969, 154.137, 0, 0, 0, 0, 100, 0, 65053),
(562745, 8, 746.8368, 3660.22, 154.137, 0, 0, 0, 0, 100, 0, 65053),
(562745, 9, 746.6441, 3663.448, 154.137, 0, 0, 0, 0, 100, 0, 65053),
(562745, 10, 747.7535, 3668.372, 154.137, 0, 0, 0, 0, 100, 0, 65053),
(562745, 11, 748.5452, 3671.731, 154.137, 0, 6000, 0, 0, 100, 0, 65053),
(562745, 12, 747.408, 3666.984, 154.137, 0, 0, 0, 0, 100, 0, 65053),
(562745, 13, 749.0452, 3663.945, 154.137, 0, 0, 0, 0, 100, 0, 65053),
(562745, 14, 752.3577, 3662.122, 154.137, 0, 7000, 0, 0, 100, 0, 65053),
(562745, 15, 748.8958, 3667.003, 154.137, 0, 0, 0, 0, 100, 0, 65053),
(562745, 16, 747.6441, 3669.635, 154.137, 0, 0, 0, 0, 100, 0, 65053),
(562745, 17, 748.4688, 3672.794, 154.1367, 0, 6000, 0, 0, 100, 0, 65053),
(562745, 18, 741.9184, 3669.666, 154.137, 0, 0, 0, 0, 100, 0, 65053),
(562745, 19, 739.6614, 3665.217, 154.1382, 0, 0, 0, 0, 100, 0, 65053),
(562745, 20, 740.6823, 3661.212, 154.1402, 0, 0, 0, 0, 100, 0, 65053);

-- Yi
UPDATE `creature` SET `position_x` = 759.5868, `position_y` = 3661.885, `position_z` = 143.4947, `orientation` = 6.083785, `wander_distance` = 0, `movement_type` = 2 WHERE `guid` = 562767;

UPDATE `creature_template_addon` SET `path_id` = 562767 WHERE `entry` = 65047;

DELETE FROM `waypoint_data` WHERE `id` = 562767;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `action`, `action_chance`, `wpguid`, `entry`) VALUES
(562767, 1, 755.9149, 3666.141, 143.4947, 0, 0, 0, 0, 100, 0, 65047),
(562767, 2, 754.533, 3668.841, 143.4959, 0, 0, 0, 0, 100, 0, 65047),
(562767, 3, 751.4566, 3671.986, 143.4925, 0, 8000, 0, 0, 100, 0, 65047),
(562767, 4, 753.7882, 3666.726, 143.3318, 0, 0, 0, 0, 100, 0, 65047),
(562767, 5, 753.1684, 3663.182, 143.3811, 3.089233, 8000, 0, 0, 100, 0, 65047),
(562767, 6, 759.5868, 3661.885, 143.4947, 0, 8000, 0, 0, 100, 0, 65047);

-- Er
UPDATE `creature` SET `position_x` = 737.5174, `position_y` = 3667.997, `position_z` = 143.3558, `orientation` = 2.70526, `wander_distance` = 0, `movement_type` = 2 WHERE `guid` = 562770;

UPDATE `creature_template_addon` SET `path_id` = 562770 WHERE `entry` = 65049;

DELETE FROM `waypoint_data` WHERE `id` = 562770;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `action`, `action_chance`, `wpguid`, `entry`) VALUES
(562770, 1, 742.1788, 3667.747, 143.2641, 0, 0, 0, 0, 100, 0, 65049),
(562770, 2, 743.0903, 3666.567, 143.2641, 4.29351, 5500, 0, 0, 100, 0, 65049),
(562770, 3, 744.8993, 3670.722, 143.2641, 0, 8000, 0, 0, 100, 0, 65049),
(562770, 4, 737.6493, 3667.642, 143.2793, 0, 0, 0, 0, 100, 0, 65049),
(562770, 5, 736.5364, 3664.528, 143.4456, 0, 0, 0, 0, 100, 0, 65049),
(562770, 6, 737.9271, 3660.398, 143.2641, 0.7504916, 7000, 0, 0, 100, 0, 65049),
(562770, 7, 736.8559, 3665.128, 143.3748, 0, 0, 0, 0, 100, 0, 65049),
(562770, 8, 737.5174, 3667.997, 143.3558, 2.70526, 7500, 0, 0, 100, 0, 65049);

UPDATE `gameobject` SET `position_x` = 747.483, `position_y` = 3675.645, `position_z` = 143.2588, `orientation` = 1.361356, `rotation0` = 0, `rotation1` = 0, `rotation2` = 0.6293201, `rotation3` = 0.7771462 WHERE `guid` = 540335;
UPDATE `gameobject` SET `position_x` = 761.499, `position_y` = 3661.257, `position_z` = 142.6234, `orientation` = 1.361356, `rotation0` = 0, `rotation1` = 0, `rotation2` = 0.6293201, `rotation3` = 0.7771462 WHERE `guid` = 540372;

UPDATE `creature_template_addon` SET `auras` = "110846" WHERE `entry` IN (65248, 65070, 65059);
UPDATE `creature` SET `wander_distance` = 0, `movement_type` = 0 WHERE `guid` IN (562763, 562789);
DELETE FROM `creature` WHERE `guid` = 562740 AND `id` = 65147;
DELETE FROM `creature_addon` WHERE `guid` = 562740;

-- Summer Lily
UPDATE `creature` SET `position_x` = 724.8507, `position_y` = 3576.906, `position_z` = 141.4816, `orientation` = 5.550147, `wander_distance` = 0, `movement_type` = 2 WHERE `guid` = 562790;

UPDATE `creature_template_addon` SET `path_id` = 562790 WHERE `entry` = 65035;

DELETE FROM `waypoint_data` WHERE `id` = 562790;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `action`, `action_chance`, `wpguid`, `entry`) VALUES
(562790, 1, 728.8872, 3571.717, 141.2709, 0, 0, 0, 0, 100, 0, 65035),
(562790, 2, 733.8872, 3570.467, 141.0209, 0, 0, 0, 0, 100, 0, 65035),
(562790, 3, 741.6372, 3566.967, 141.0209, 0, 0, 0, 0, 100, 0, 65035),
(562790, 4, 743.1372, 3562.967, 141.0209, 0, 0, 0, 0, 100, 0, 65035),
(562790, 5, 744.3872, 3557.467, 140.7709, 0, 0, 0, 0, 100, 0, 65035),
(562790, 6, 745.8872, 3552.967, 140.7709, 0, 0, 0, 0, 100, 0, 65035),
(562790, 7, 749.3872, 3550.967, 140.5209, 0, 0, 0, 0, 100, 0, 65035),
(562790, 8, 753.1372, 3551.967, 141.2709, 0, 0, 0, 0, 100, 0, 65035),
(562790, 9, 759.4236, 3551.028, 141.5601, 0.9773844, 74000, 0, 0, 100, 0, 65035),
(562790, 10, 757.3872, 3550.217, 141.5209, 0, 0, 0, 0, 100, 0, 65035),
(562790, 11, 752.1372, 3551.717, 141.0209, 0, 0, 0, 0, 100, 0, 65035),
(562790, 12, 747.8872, 3552.467, 140.7709, 0, 0, 0, 0, 100, 0, 65035),
(562790, 13, 745.6372, 3554.967, 140.7709, 0, 0, 0, 0, 100, 0, 65035),
(562790, 14, 743.8872, 3560.967, 141.0209, 0, 0, 0, 0, 100, 0, 65035),
(562790, 15, 742.1372, 3565.967, 141.0209, 0, 0, 0, 0, 100, 0, 65035),
(562790, 16, 736.3872, 3569.717, 141.0209, 0, 0, 0, 0, 100, 0, 65035),
(562790, 17, 732.1372, 3571.717, 141.0209, 0, 0, 0, 0, 100, 0, 65035),
(562790, 18, 728.1372, 3574.217, 141.5209, 0, 0, 0, 0, 100, 0, 65035),
(562790, 19, 724.8507, 3576.906, 141.4816, 5.550147, 20000, 0, 0, 100, 0, 65035);

UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` = 65035;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 65035 AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 6503500 AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(65035, 0, 0, 0, 34, 0, 100, 0, 2, 8, 0, 0, 80, 6503500, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Summer Lily - On MI 8 - Start Script"),

(6503500, 9, 0, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 90, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Summer Lily - On Script - Set Unit Field Bytes 1 (8)"),
(6503500, 9, 1, 0, 0, 0, 100, 0, 60000, 60000, 0, 0, 91, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Summer Lily - On Script - Set Unit Field Bytes 1 (0)"),
(6503500, 9, 2, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 5, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Summer Lily - On Script - Play Emote (2)");
