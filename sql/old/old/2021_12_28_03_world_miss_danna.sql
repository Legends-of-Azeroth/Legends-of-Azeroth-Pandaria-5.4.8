-- Miss Danna
UPDATE `creature` SET `position_x` = -8838.52, `position_y` = 670.28, `position_z` = 98.0987, `orientation` = 0.553153, `spawntimesecs` = 43200, `spawndist` = 0, `MovementType` = 2 WHERE `guid` = 188983 AND `id` = 3513;
UPDATE `creature` SET `position_x` = -8841.64, `position_y` = 671.515, `position_z` = 97.9739, `orientation` = 0.556232, `spawntimesecs` = 43200, `spawndist` = 0, `MovementType` = 0 WHERE `guid` = 188985 AND `id` = 3512;
UPDATE `creature` SET `position_x` = -8841.74, `position_y` = 670.394, `position_z` = 97.9404, `orientation` = 0.629918, `spawntimesecs` = 43200, `spawndist` = 0, `MovementType` = 0 WHERE `guid` = 188986 AND `id` = 3508;
UPDATE `creature` SET `position_x` = -8838.57, `position_y` = 667.07, `position_z` = 98.0103, `orientation` = 0.553137, `spawntimesecs` = 43200, `spawndist` = 0, `MovementType` = 0 WHERE `guid` = 188990 AND `id` = 3505;
UPDATE `creature` SET `position_x` = -8841.58, `position_y` = 669.168, `position_z` = 97.9151, `orientation` = 0.55451, `spawntimesecs` = 43200, `spawndist` = 0, `MovementType` = 0 WHERE `guid` = 188987 AND `id` = 3511;
UPDATE `creature` SET `position_x` = -8841.32, `position_y` = 667.768, `position_z` = 97.8914, `orientation` = 0.553006, `spawntimesecs` = 43200, `spawndist` = 0, `MovementType` = 0 WHERE `guid` = 188988 AND `id` = 3510;
UPDATE `creature` SET `position_x` = -8840.46, `position_y` = 666.881, `position_z` = 97.9108, `orientation` = 0.553158, `spawntimesecs` = 43200, `spawndist` = 0, `MovementType` = 0 WHERE `guid` = 188989 AND `id` = 3507;
UPDATE `creature` SET `position_x` = -8841.45, `position_y` = 668.822, `position_z` = 97.95883, `orientation` = 0.507234, `spawntimesecs` = 43200, `spawndist` = 0, `MovementType` = 0 WHERE `guid` = 188984 AND `id` = 3509;

DELETE FROM `creature_template_addon` WHERE `entry` IN (3513, 3512, 3508, 3505, 3511, 3510, 3507, 3509);

DELETE FROM `creature_addon` WHERE `guid` IN (188983, 188985, 188986, 188990, 188987, 188988, 188989, 188984);
INSERT INTO `creature_addon` (`guid`, `path_id`, `bytes2`) VALUES
(188983, 188983, 0);

DELETE FROM `waypoint_data` WHERE `id` IN (188983, 188985, 188986, 188990, 188987, 188988, 188989, 188984);
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `action`, `action_chance`, `wpguid`, `entry`) VALUES
(188983, 1, -8824.65, 677.359, 97.6724, 0, 1000, 0, 0, 100, 0, 3513),
(188983, 2, -8844.06, 721.315, 97.3127, 0, 1000, 0, 0, 100, 0, 3513),
(188983, 3, -8794.66, 745.738, 97.8358, 0, 2500, 0, 0, 100, 0, 3513),
(188983, 4, -8765.71, 737.339, 98.9571, 0, 1000, 0, 0, 100, 0, 3513),
(188983, 5, -8744.1, 711.742, 98.1589, 0, 0, 0, 0, 100, 0, 3513),
(188983, 6, -8736.13, 717.57, 101.035, 0, 0, 0, 0, 100, 0, 3513),
(188983, 7, -8714.09, 735.726, 97.8129, 0, 0, 0, 0, 100, 0, 3513),
(188983, 8, -8698.83, 714.642, 97.0168, 0, 0, 0, 0, 100, 0, 3513),
(188983, 9, -8657.55, 748.029, 96.6892, 0, 0, 0, 0, 100, 0, 3513),
(188983, 11, -8658.68, 767.899, 96.6925, 0, 0, 0, 0, 100, 0, 3513),
(188983, 12, -8634.49, 787.411, 96.6512, 0, 1500, 0, 0, 100, 0, 3513),
(188983, 13, -8662.17, 821.239, 96.6386, 0, 0, 0, 0, 100, 0, 3513),
(188983, 14, -8653.53, 826.735, 96.5285, 0, 2000, 0, 0, 100, 0, 3513),
(188983, 15, -8608.62, 861.791, 96.6798, 0, 0, 0, 0, 100, 0, 3513),
(188983, 16, -8600.32, 850.058, 96.6911, 0, 0, 0, 0, 100, 0, 3513),
(188983, 17, -8616.11, 837.915, 96.7385, 0, 3000, 0, 0, 100, 0, 3513),
(188983, 18, -8637.49, 816.942, 96.6393, 0, 2500, 0, 0, 100, 0, 3513),
(188983, 19, -8629.56, 781.904, 96.6514, 0, 0, 0, 0, 100, 0, 3513),
(188983, 20, -8623.89, 774.974, 96.6518, 0, 0, 0, 0, 100, 0, 3513),
(188983, 21, -8623.29, 775.529, 96.6512, 0, 1000, 0, 0, 100, 0, 3513),
(188983, 22, -8623.29, 775.529, 96.6512, 0, 22000, 0, 0, 100, 0, 3513),
(188983, 23, -8594.06, 757.656, 96.6537, 0, 0, 0, 0, 100, 0, 3513),
(188983, 24, -8578.89, 737.947, 96.6719, 0, 0, 0, 0, 100, 0, 3513),
(188983, 25, -8596.77, 717.424, 96.6572, 0, 1000, 0, 0, 100, 0, 3513),
(188983, 26, -8562.25, 673.301, 97.0168, 0, 0, 0, 0, 100, 0, 3513),
(188983, 27, -8536.52, 690.402, 97.6668, 0, 0, 0, 0, 100, 0, 3513),
(188983, 28, -8522.24, 670.618, 102.794, 0, 0, 0, 0, 100, 0, 3513),
(188983, 29, -8508.77, 650.151, 100.292, 0, 0, 0, 0, 100, 0, 3513),
(188983, 30, -8557.68, 615.023, 102.352, 0, 0, 0, 0, 100, 0, 3513),
(188983, 31, -8568.14, 613.353, 102.382, 0, 0, 0, 0, 100, 0, 3513),
(188983, 32, -8581.15, 593.053, 103.562, 0, 3000, 0, 0, 100, 0, 3513),
(188983, 33, -8585.6, 562.85, 101.927, 0, 1000, 0, 0, 100, 0, 3513),
(188983, 34, -8573.45, 540.833, 101.757, 0, 0, 0, 0, 100, 0, 3513),
(188983, 35, -8535.68, 488.072, 101.081, 0, 0, 0, 0, 100, 0, 3513),
(188983, 36, -8538.69, 479.398, 102.572, 0, 0, 0, 0, 100, 0, 3513),
(188983, 37, -8548.29, 467.484, 104.514, 0, 0, 0, 0, 100, 0, 3513),
(188983, 38, -8544.94, 464.148, 104.414, 0, 1000, 0, 0, 100, 0, 3513),
(188983, 39, -8544.94, 464.148, 104.414, 0, 15000, 0, 0, 100, 0, 3513),
(188983, 40, -8564.17, 466.02, 104.524, 0, 1000, 0, 0, 100, 0, 3513),
(188983, 41, -8583.28, 480.99, 104.214, 0, 0, 0, 0, 100, 0, 3513),
(188983, 42, -8606.08, 504.922, 103.722, 0, 0, 0, 0, 100, 0, 3513),
(188983, 43, -8633.15, 534.792, 100.272, 0, 0, 0, 0, 100, 0, 3513),
(188983, 44, -8655.92, 552.797, 96.9437, 0, 0, 0, 0, 100, 0, 3513),
(188983, 45, -8671.53, 554.116, 97.1805, 0, 0, 0, 0, 100, 0, 3513),
(188983, 46, -8713.5, 519.816, 97.1699, 0, 0, 0, 0, 100, 0, 3513),
(188983, 47, -8731.17, 541.957, 101.124, 0, 1500, 0, 0, 100, 0, 3513),
(188983, 48, -8749.98, 561.766, 97.3988, 0, 0, 0, 0, 100, 0, 3513),
(188983, 49, -8736.87, 574.559, 97.3823, 0, 0, 0, 0, 100, 0, 3513),
(188983, 50, -8770.53, 609.421, 97.2463, 0, 0, 0, 0, 100, 0, 3513),
(188983, 51, -8795.9, 587.923, 97.3792, 0, 0, 0, 0, 100, 0, 3513),
(188983, 52, -8827.8, 624.687, 93.8494, 0, 0, 0, 0, 100, 0, 3513),
(188983, 53, -8928.94, 540.519, 94.3157, 0, 0, 0, 0, 100, 0, 3513),
(188983, 54, -8907.67, 509.135, 93.8416, 0, 0, 0, 0, 100, 0, 3513),
(188983, 55, -8933.21, 488.088, 93.8429, 0, 1000, 0, 0, 100, 0, 3513),
(188983, 56, -8933.21, 488.088, 93.8429, 0, 16000, 0, 0, 100, 0, 3513),
(188983, 57, -8911.65, 505.615, 93.8585, 0, 0, 0, 0, 100, 0, 3513),
(188983, 58, -8925.24, 543.334, 94.268, 0, 1250, 0, 0, 100, 0, 3513),
(188983, 59, -8833.34, 620.843, 93.4683, 0, 0, 0, 0, 100, 0, 3513),
(188983, 60, -8854.67, 660.036, 96.8874, 0, 0, 0, 0, 100, 0, 3513),
(188983, 61, -8838.52, 670.28, 98.0987, 0, 5000, 0, 0, 100, 0, 3513);

DELETE FROM `creature_formations` WHERE `leaderGUID` = 188983;
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES 
(188983, 188983, 0, 0, 2),
(188983, 188990, 3, 30, 2),
(188983, 188986, 3, 60, 2),
(188983, 188988, 3, 90, 2),
(188983, 188987, 3, 330, 2),
(188983, 188985, 3, 300, 2),
(188983, 188989, 3, 270, 2),
(188983, 188984, 3, 360, 2);

UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` IN (3513, 3512, 3508, 3505, 3511, 3510, 3507, 3509);
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (3513, 3512, 3508, 3505, 3511, 3510, 3507, 3509) AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (3513*100, 3513*100+01, 3513*100+02, 3513*100+03, 3513*100+04, 3513*100+05, 3513*100+06) AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(3513, 0, 0, 0, 34, 0, 100, 0, 2, 20, 0, 0, 80, 3513*100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Miss Danna - On MI 20 P - Start Script (00)"),
(3513, 0, 1, 0, 34, 0, 100, 0, 2, 34, 0, 0, 80, 3513*100+01, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Miss Danna - On MI 34 P - Start Script (01)"),
(3513, 0, 2, 0, 34, 0, 100, 0, 2, 35, 0, 0, 80, 3513*100+02, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Miss Danna - On MI 35 P - Start Script (02)"),
(3513, 0, 3, 0, 34, 0, 100, 0, 2, 37, 0, 0, 80, 3513*100+03, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Miss Danna - On MI 37 P - Start Script (03)"),
(3513, 0, 4, 0, 34, 0, 100, 0, 2, 51, 0, 0, 80, 3513*100+04, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Miss Danna - On MI 51 P - Start Script (04)"),
(3513, 0, 5, 0, 34, 0, 100, 0, 2, 52, 0, 0, 80, 3513*100+05, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Miss Danna - On MI 52 P - Start Script (05)"),
(3513, 0, 6, 0, 34, 0, 100, 0, 2, 54, 0, 0, 80, 3513*100+06, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Miss Danna - On MI 54 P - Start Script (06)"),

(3512, 0, 0, 0, 38, 0, 100, 0, 1, 1, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0.658629, "Jimmy - On Data Set 1 1 - Set Orientation (0.658629)"),
(3512, 0, 1, 0, 38, 0, 100, 0, 2, 2, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 5.378945, "Jimmy - On Data Set 2 2 - Set Orientation (5.378945)"),
(3512, 0, 2, 0, 38, 0, 100, 0, 3, 3, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 3.849771, "Jimmy - On Data Set 3 3 - Set Orientation (3.849771)"),

(3508, 0, 0, 0, 38, 0, 100, 0, 1, 1, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0.658629, "Mikey - On Data Set 1 1 - Set Orientation (0.658629)"),
(3508, 0, 1, 0, 38, 0, 100, 0, 2, 2, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 5.378945, "Mikey - On Data Set 2 2 - Set Orientation (5.378945)"),
(3508, 0, 2, 0, 38, 0, 100, 0, 3, 3, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 3.849771, "Mikey - On Data Set 3 3 - Set Orientation (3.849771)"),

(3505, 0, 0, 0, 38, 0, 100, 0, 1, 1, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0.658629, "Pat - On Data Set 1 1 - Set Orientation (0.658629)"),
(3505, 0, 1, 0, 38, 0, 100, 0, 2, 2, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 5.378945, "Pat - On Data Set 2 2 - Set Orientation (5.378945)"),
(3505, 0, 2, 0, 38, 0, 100, 0, 3, 3, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 3.849771, "Pat - On Data Set 3 3 - Set Orientation (3.849771)"),

(3511, 0, 0, 0, 38, 0, 100, 0, 1, 1, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0.658629, "Steven - On Data Set 1 1 - Set Orientation (0.658629)"),
(3511, 0, 1, 0, 38, 0, 100, 0, 2, 2, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 5.378945, "Steven - On Data Set 2 2 - Set Orientation (5.378945)"),
(3511, 0, 2, 0, 38, 0, 100, 0, 3, 3, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 3.849771, "Steven - On Data Set 3 3 - Set Orientation (3.849771)"),

(3510, 0, 0, 0, 38, 0, 100, 0, 1, 1, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0.658629, "Twain - On Data Set 1 1 - Set Orientation (0.658629)"),
(3510, 0, 1, 0, 38, 0, 100, 0, 2, 2, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 5.378945, "Twain - On Data Set 2 2 - Set Orientation (5.378945)"),
(3510, 0, 2, 0, 38, 0, 100, 0, 3, 3, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 3.849771, "Twain - On Data Set 3 3 - Set Orientation (3.849771)"),

(3507, 0, 0, 0, 38, 0, 100, 0, 1, 1, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0.658629, "Andi - On Data Set 1 1 - Set Orientation (0.658629)"),
(3507, 0, 1, 0, 38, 0, 100, 0, 2, 2, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 5.378945, "Andi - On Data Set 2 2 - Set Orientation (5.378945)"),
(3507, 0, 2, 0, 38, 0, 100, 0, 3, 3, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 3.849771, "Andi - On Data Set 3 3 - Set Orientation (3.849771)"),

(3509, 0, 0, 0, 38, 0, 100, 0, 1, 1, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0.658629, "Geoff - On Data Set 1 1 - Set Orientation (0.658629)"),
(3509, 0, 1, 0, 38, 0, 100, 0, 2, 2, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 5.378945, "Geoff - On Data Set 2 2 - Set Orientation (5.378945)"),
(3509, 0, 2, 0, 38, 0, 100, 0, 3, 3, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 3.849771, "Geoff - On Data Set 3 3 - Set Orientation (3.849771)"),

(3513*100, 9, 0, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0.658629, "Miss Danna - On Script - Set Orientation (0.658629)"),
(3513*100, 9, 1, 0, 0, 0, 100, 0, 500, 500, 0, 0, 45, 1, 1, 0, 0, 0, 0, 19, 3512, 25, 0, 0, 0, 0, 0, "Miss Danna - On Script - Set Data 1 1 (Creature ID: 3512, 25 Yards)"),
(3513*100, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 19, 3508, 25, 0, 0, 0, 0, 0, "Miss Danna - On Script - Set Data 1 1 (Creature ID: 3508, 25 Yards)"),
(3513*100, 9, 3, 0, 0, 0, 100, 0, 500, 500, 0, 0, 45, 1, 1, 0, 0, 0, 0, 19, 3505, 25, 0, 0, 0, 0, 0, "Miss Danna - On Script - Set Data 1 1 (Creature ID: 3505, 25 Yards)"),
(3513*100, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 19, 3511, 25, 0, 0, 0, 0, 0, "Miss Danna - On Script - Set Data 1 1 (Creature ID: 3511, 25 Yards)"),
(3513*100, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 19, 3510, 25, 0, 0, 0, 0, 0, "Miss Danna - On Script - Set Data 1 1 (Creature ID: 3510, 25 Yards)"),
(3513*100, 9, 6, 0, 0, 0, 100, 0, 500, 500, 0, 0, 45, 1, 1, 0, 0, 0, 0, 19, 3507, 25, 0, 0, 0, 0, 0, "Miss Danna - On Script - Set Data 1 1 (Creature ID: 3507, 25 Yards)"),
(3513*100, 9, 7, 0, 0, 0, 100, 0, 100, 100, 0, 0, 45, 1, 1, 0, 0, 0, 0, 19, 3509, 25, 0, 0, 0, 0, 0, "Miss Danna - On Script - Set Data 1 1 (Creature ID: 3509, 25 Yards)"),
(3513*100, 9, 8, 0, 0, 0, 100, 0, 1500, 1500, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Miss Danna - On Script - Say Text Line 0"),
(3513*100, 9, 9, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 5, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Miss Danna - On Script - Play Emote (1)"),
(3513*100, 9, 10, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 1, 0, 0, 0, 0, 0, 0, 19, 3512, 25, 0, 0, 0, 0, 0, "Miss Danna - On Script - Say Text Line 0 (Creature ID: 3512, 25 Yards)"),
(3513*100, 9, 11, 0, 0, 0, 100, 0, 3500, 3500, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Miss Danna - On Script - Say Text Line 1"),
(3513*100, 9, 12, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Miss Danna - On Script - Say Text Line 2"),

(3513*100+01, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Miss Danna - On Script - Say Text Line 3"),
(3513*100+01, 9, 1, 0, 0, 0, 100, 0, 200, 200, 0, 0, 5, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Miss Danna - On Script - Play Emote (1)"),

(3513*100+02, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Miss Danna - On Script - Say Text Line 4"),
(3513*100+02, 9, 1, 0, 0, 0, 100, 0, 200, 200, 0, 0, 5, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Miss Danna - On Script - Play Emote (1)"),

(3513*100+03, 9, 0, 0, 0, 0, 100, 0, 1500, 1500, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 5.378945, "Miss Danna - On Script - Set Orientation (5.378945)"),
(3513*100+03, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 5, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Miss Danna - On Script - Play Emote (1)"),
(3513*100+03, 9, 2, 0, 0, 0, 100, 0, 500, 500, 0, 0, 45, 2, 2, 0, 0, 0, 0, 19, 3512, 25, 0, 0, 0, 0, 0, "Miss Danna - On Script - Set Data 2 2 (Creature ID: 3512, 25 Yards)"),
(3513*100+03, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 2, 2, 0, 0, 0, 0, 19, 3508, 25, 0, 0, 0, 0, 0, "Miss Danna - On Script - Set Data 2 2 (Creature ID: 3508, 25 Yards)"),
(3513*100+03, 9, 4, 0, 0, 0, 100, 0, 500, 500, 0, 0, 45, 2, 2, 0, 0, 0, 0, 19, 3505, 25, 0, 0, 0, 0, 0, "Miss Danna - On Script - Set Data 2 2 (Creature ID: 3505, 25 Yards)"),
(3513*100+03, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 2, 2, 0, 0, 0, 0, 19, 3511, 25, 0, 0, 0, 0, 0, "Miss Danna - On Script - Set Data 2 2 (Creature ID: 3511, 25 Yards)"),
(3513*100+03, 9, 6, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 2, 2, 0, 0, 0, 0, 19, 3510, 25, 0, 0, 0, 0, 0, "Miss Danna - On Script - Set Data 2 2 (Creature ID: 3510, 25 Yards)"),
(3513*100+03, 9, 7, 0, 0, 0, 100, 0, 500, 500, 0, 0, 45, 2, 2, 0, 0, 0, 0, 19, 3507, 25, 0, 0, 0, 0, 0, "Miss Danna - On Script - Set Data 2 2 (Creature ID: 3507, 25 Yards)"),
(3513*100+03, 9, 8, 0, 0, 0, 100, 0, 100, 100, 0, 0, 45, 2, 2, 0, 0, 0, 0, 19, 3509, 25, 0, 0, 0, 0, 0, "Miss Danna - On Script - Set Data 2 2 (Creature ID: 3509, 25 Yards)"),
(3513*100+03, 9, 9, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 1, 0, 0, 0, 0, 0, 0, 19, 3511, 25, 0, 0, 0, 0, 0, "Miss Danna - On Script - Say Text Line 0 (Creature ID: 3511, 25 Yards)"),
(3513*100+03, 9, 10, 0, 0, 0, 100, 0, 3500, 3500, 0, 0, 1, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Miss Danna - On Script - Say Text Line 5"),
(3513*100+03, 9, 11, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 5, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Miss Danna - On Script - Play Emote (1)"),

(3513*100+04, 9, 0, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 1, 6, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Miss Danna - On Script - Say Text Line 6"),
(3513*100+04, 9, 1, 0, 0, 0, 100, 0, 200, 200, 0, 0, 5, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Miss Danna - On Script - Play Emote (1)"),

(3513*100+05, 9, 0, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 1, 7, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Miss Danna - On Script - Say Text Line 7"),
(3513*100+05, 9, 1, 0, 0, 0, 100, 0, 200, 200, 0, 0, 5, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Miss Danna - On Script - Play Emote (1)"),

(3513*100+06, 9, 0, 0, 0, 0, 100, 0, 1500, 1500, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 3.849771, "Miss Danna - On Script - Set Orientation (3.849771)"),
(3513*100+06, 9, 1, 0, 0, 0, 100, 0, 500, 500, 0, 0, 45, 3, 3, 0, 0, 0, 0, 19, 3512, 25, 0, 0, 0, 0, 0, "Miss Danna - On Script - Set Data 3 3 (Creature ID: 3512, 25 Yards)"),
(3513*100+06, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 3, 3, 0, 0, 0, 0, 19, 3508, 25, 0, 0, 0, 0, 0, "Miss Danna - On Script - Set Data 3 3 (Creature ID: 3508, 25 Yards)"),
(3513*100+06, 9, 3, 0, 0, 0, 100, 0, 500, 500, 0, 0, 45, 3, 3, 0, 0, 0, 0, 19, 3505, 25, 0, 0, 0, 0, 0, "Miss Danna - On Script - Set Data 3 3 (Creature ID: 3505, 25 Yards)"),
(3513*100+06, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 3, 3, 0, 0, 0, 0, 19, 3511, 25, 0, 0, 0, 0, 0, "Miss Danna - On Script - Set Data 3 3 (Creature ID: 3511, 25 Yards)"),
(3513*100+06, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 3, 3, 0, 0, 0, 0, 19, 3510, 25, 0, 0, 0, 0, 0, "Miss Danna - On Script - Set Data 3 3 (Creature ID: 3510, 25 Yards)"),
(3513*100+06, 9, 6, 0, 0, 0, 100, 0, 500, 500, 0, 0, 45, 3, 3, 0, 0, 0, 0, 19, 3507, 25, 0, 0, 0, 0, 0, "Miss Danna - On Script - Set Data 3 3 (Creature ID: 3507, 25 Yards)"),
(3513*100+06, 9, 7, 0, 0, 0, 100, 0, 100, 100, 0, 0, 45, 3, 3, 0, 0, 0, 0, 19, 3509, 25, 0, 0, 0, 0, 0, "Miss Danna - On Script - Set Data 3 3 (Creature ID: 3509, 25 Yards)"),
(3513*100+06, 9, 8, 0, 0, 0, 100, 0, 2500, 2500, 0, 0, 1, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Miss Danna - On Script - Say Text Line 8"),
(3513*100+06, 9, 9, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 5, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Miss Danna - On Script - Play Emote (1)"),
(3513*100+06, 9, 10, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 5, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Miss Danna - On Script - Play Emote (1)");

DELETE FROM `creature_text` WHERE `entry` IN (3513, 3512, 3511);
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `text_female`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(3513, 0, 0, "", "Here we have the Cathedral of Light, the center of spiritual enlightenment here in Stormwind.", 12, 7, 100, 1, 0, 0, "Miss Danna"),
(3513, 1, 0, "", "Yes, that is true. Paladins and Priests alike train their skills and research great truths behind the walls of the Cathedral.", 12, 7, 100, 1, 0, 0, "Miss Danna"),
(3513, 2, 0, "", "Children if you would please follow me, we will now be going to see the keep where King Anduin Wrynn himself sits on his throne.", 12, 7, 100, 1, 0, 0, "Miss Danna"),
(3513, 3, 0, "", "Here we have Stormwind Keep. Built upon the ruins of Stormwind Castle, which was destroyed by the Horde in the first Great War.", 12, 7, 100, 0, 0, 0, "Miss Danna"),
(3513, 4, 0, "", "When the Horde was shattered, men returned here and began to rebuild the once great city as a testament to our own survival.", 12, 7, 100, 0, 0, 0, "Miss Danna"),
(3513, 5, 0, "", "Yes, well...let's head on to the monument dedicated to the heroes of the two Great Wars, the Valley of Heroes. Follow Me.", 12, 7, 100, 1, 0, 0, "Miss Danna"),
(3513, 6, 0, "", "Isn't it amazing, children? All who enter the city must walk beneath the watchful eyes of the greatest heroes of our lands.", 12, 7, 100, 0, 0, 0, "Miss Danna"),
(3513, 7, 0, "", "Breathtaking. Children, when we return to the school, you will each give an oral report on one of these legendary people.", 12, 7, 100, 0, 0, 0, "Miss Danna"),
(3513, 8, 0, "", "Now, take another long look before we make our way to the Holy District and the great Cathedral of Light.", 12, 7, 100, 1, 0, 0, "Miss Danna"),

(3512, 0, 0, "Is it true that the paladins train here?", "", 12, 7, 100, 1, 0, 0, "Jimmy"),

(3511, 0, 0, "Why do we have to learn this stuff anyway?", "", 12, 7, 100, 1, 0, 0, "Steven");
