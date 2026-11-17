UPDATE `creature` SET `wander_distance` = 0, `movement_type` = 0 WHERE `guid` IN (219625, 219985, 220183, 220188, 219589, 219982, 220185, 219986,
                                                                                  219989, 221422, 221415, 221417, 220867, 221418, 221416, 221421,
                                                                                  221419, 221420);

UPDATE `creature` SET `position_x` = -1494.238, `position_y` = 1336.437, `position_z` = 58.767, `orientation` = 1.931 WHERE `guid` = 219545 AND `id` = 34867;
UPDATE `creature` SET `position_x` = -1486.67, `position_y` = 1440.716, `position_z` = 59.125, `orientation` = 4.659 WHERE `guid` = 219546 AND `id` = 34867;
UPDATE `creature` SET `position_x` = -1446.26, `position_y` = 1450.723, `position_z` = 71.052, `orientation` = 3.149093 WHERE `guid` = 219547 AND `id` = 34867;
UPDATE `creature` SET `position_x` = -1509.6, `position_y` = 1370.527, `position_z` = 73.233, `orientation` = 3.149093 WHERE `guid` = 219548 AND `id` = 34867;
UPDATE `creature` SET `position_x` = -1507.03, `position_y` = 1321.1, `position_z` = 73.285, `orientation` = 3.377 WHERE `guid` = 219549 AND `id` = 34867;
UPDATE `creature` SET `position_x` = -1460.46, `position_y` = 1370.69, `position_z` = 64.475, `orientation` = 6.209 WHERE `guid` = 219550 AND `id` = 34867;
UPDATE `creature` SET `position_x` = -1421.93, `position_y` = 1401.452, `position_z` = 71.819, `orientation` = 4.755233 WHERE `guid` = 219551 AND `id` = 34867;
UPDATE `creature` SET `position_x` = -1409.69, `position_y` = 1429.24, `position_z` = 59.063, `orientation` = 6.205 WHERE `guid` = 219552 AND `id` = 34867;
UPDATE `creature` SET `position_x` = -1562.106, `position_y` = 1409.314, `position_z` = 71.721, `orientation` = 1.582226 WHERE `guid` = 219553 AND `id` = 34867;
UPDATE `creature` SET `position_x` = -1430.04, `position_y` = 1429.107, `position_z` = 59.057, `orientation` = 3.169 WHERE `guid` = 219554 AND `id` = 34867;

UPDATE `creature` SET `wander_distance` = 0, `movement_type` = 0 WHERE `guid` IN (219545, 219546, 219547, 219548, 219549, 219550, 219551, 219552, 219553, 219554);

DELETE FROM `creature_addon` WHERE `guid` IN (219545, 219546, 219547, 219548, 219549, 219550, 219551, 219552, 219553, 219554);
INSERT INTO `creature_addon` (`guid`, `path_id`, `bytes1`, `bytes2`, `emote`, `auras`) VALUES
(219545, 0, 8, 1, 0, "39662"),
(219546, 0, 8, 1, 0, "39662"),
(219547, 0, 0, 1, 0, "39662"),
(219548, 0, 0, 1, 0, "39662"),
(219549, 0, 0, 1, 0, "39662"),
(219550, 0, 8, 1, 0, "39662"),
(219551, 0, 0, 1, 0, "39662"),
(219552, 0, 8, 1, 0, "39662"),
(219553, 0, 0, 1, 0, "39662"),
(219554, 0, 8, 1, 0, "39662");

DELETE FROM `waypoint_data` WHERE `id` = 219545;
DELETE FROM `waypoints` WHERE `entry` IN (348670, 348671, 348672, 348673, 348674);
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`, `point_comment`) VALUES
(348670, 1, -1446.26, 1450.723, 71.052, "Shadowy Figure"),
(348670, 2, -1442.63, 1450.802, 71.116, "Shadowy Figure"),
(348670, 3, -1440.51, 1450.681, 71.129, "Shadowy Figure"),
(348670, 4, -1420.51, 1451.143, 71.531, "Shadowy Figure"),
(348670, 5, -1420.19, 1426.869, 72.786, "Shadowy Figure"),
(348670, 6, -1420.51, 1451.143, 71.531, "Shadowy Figure"),
(348670, 7, -1409.75, 1451.397, 72.759, "Shadowy Figure"),
(348670, 8, -1387.74, 1393.781, 73.008, "Shadowy Figure"),
(348670, 9, -1387.15, 1461.186, 73.155, "Shadowy Figure"),
(348670, 10, -1386.907, 1452.008, 72.38, "Shadowy Figure"),
(348670, 11, -1439.54, 1450.702, 71.146, "Shadowy Figure"),
(348670, 12, -1442.58, 1450.784, 71.117, "Shadowy Figure"),
(348670, 13, -1473.61, 1451.046, 72.529, "Shadowy Figure"),
(348670, 14, -1513.61, 1450.771, 71.138, "Shadowy Figure"),
(348670, 15, -1516.12, 1450.739, 71.117, "Shadowy Figure"),
(348670, 16, -1542.02, 1450.936, 71.166, "Shadowy Figure"),
(348670, 17, -1546.29, 1450.326, 71.221, "Shadowy Figure"),
(348670, 18, -1549.32, 1449.374, 71.223, "Shadowy Figure"),
(348670, 19, -1553.46, 1447.001, 71.131, "Shadowy Figure"),
(348670, 20, -1557.25, 1442.885, 71.026, "Shadowy Figure"),
(348670, 21, -1560.56, 1437.244, 70.992, "Shadowy Figure"),
(348670, 22, -1562.48, 1430.272, 70.922, "Shadowy Figure"),
(348670, 23, -1562.16, 1409.398, 71.713, "Shadowy Figure"),
(348670, 24, -1594.14, 1408.106, 72.883, "Shadowy Figure"),
(348670, 25, -1562.22, 1409.616, 71.692, "Shadowy Figure"),
(348670, 26, -1562.39, 1431.084, 70.912, "Shadowy Figure"),
(348670, 27, -1560.53, 1437.465, 70.994, "Shadowy Figure"),
(348670, 28, -1556.66, 1443.712, 71.037, "Shadowy Figure"),
(348670, 29, -1553.18, 1447.33, 71.141, "Shadowy Figure"),
(348670, 30, -1549.11, 1449.37, 71.228, "Shadowy Figure"),
(348670, 31, -1546.61, 1450.35, 71.225, "Shadowy Figure"),
(348670, 32, -1515.92, 1450.72, 71.117, "Shadowy Figure"),
(348670, 33, -1513.99, 1450.71, 71.13, "Shadowy Figure"),
(348670, 34, -1483.33, 1451.37, 72.75, "Shadowy Figure"),
(348670, 35, -1462.11, 1450.82, 71.27, "Shadowy Figure"),

(348671, 1, -1509.6, 1370.527, 73.233, "Shadowy Figure"),
(348671, 2, -1475.03, 1370.223, 71.843, "Shadowy Figure"),
(348671, 3, -1497.173, 1370.632, 73.18, "Shadowy Figure"),
(348671, 4, -1531.706, 1370.083, 71.79, "Shadowy Figure"),

(348672, 1, -1564.72, 1305.238, 73.726, "Shadowy Figure"),
(348672, 2, -1503.081, 1322.301, 73.691, "Shadowy Figure"),
(348672, 3, -1475.027, 1330.954, 71.224, "Shadowy Figure"),
(348672, 4, -1472.984, 1331.376, 70.75, "Shadowy Figure"),
(348672, 5, -1453.404, 1336.731, 70.541, "Shadowy Figure"),
(348672, 6, -1472.984, 1331.376, 70.75, "Shadowy Figure"),
(348672, 7, -1475.027, 1330.954, 71.224, "Shadowy Figure"),
(348672, 8, -1494.097, 1325.942, 71.425, "Shadowy Figure"),
(348672, 9, -1509.355, 1320.472, 73.064, "Shadowy Figure"),

(348673, 1, -1421.93, 1401.452, 71.819, "Shadowy Figure"),
(348673, 2, -1421.27, 1416.098, 64.517, "Shadowy Figure"),
(348673, 3, -1421.18, 1411.319, 63.666, "Shadowy Figure"),
(348673, 4, -1421.408, 1378.319, 73.281, "Shadowy Figure"),
(348673, 5, -1421.539, 1386.362, 71.882, "Shadowy Figure"),

(348674, 1, -1562.106, 1409.314, 71.721, "Shadowy Figure"),
(348674, 2, -1562.505, 1424.309, 71.087, "Shadowy Figure"),
(348674, 3, -1550.897, 1424.948, 67.934, "Shadowy Figure"),
(348674, 4, -1554.607, 1424.859, 67.016, "Shadowy Figure"),
(348674, 5, -1562.159, 1409.213, 71.714, "Shadowy Figure"),
(348674, 6, -1583.951, 1408.604, 71.773, "Shadowy Figure"),
(348674, 7, -1583.977, 1406.82, 70.702, "Shadowy Figure"),
(348674, 8, -1583.578, 1388.626, 70.268, "Shadowy Figure"),
(348674, 9, -1583.587, 1386.165, 70.406, "Shadowy Figure"),
(348674, 10, -1583.345, 1378.357, 71.797, "Shadowy Figure"),
(348674, 11, -1593.268, 1378.017, 72.829, "Shadowy Figure"),
(348674, 12, -1560.139, 1377.728, 72.973, "Shadowy Figure"),
(348674, 13, -1536.4, 1370.865, 62.997, "Shadowy Figure"),
(348674, 14, -1536.334, 1383.368, 61.117, "Shadowy Figure"),
(348674, 15, -1536.364, 1381.037, 60.534, "Shadowy Figure"),
(348674, 16, -1546.873, 1370.736, 64.512, "Shadowy Figure"),
(348674, 17, -1583.402, 1378.333, 71.796, "Shadowy Figure"),
(348674, 18, -1583.493, 1386.039, 70.43, "Shadowy Figure"),
(348674, 19, -1583.528, 1388.739, 70.256, "Shadowy Figure"),
(348674, 20, -1583.938, 1408.432, 71.775, "Shadowy Figure"),
(348674, 21, -1567.825, 1409.405, 71.127, "Shadowy Figure");

UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` = 34867;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (34867, -219547, -219548, -219549, -219551, -219553) AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (34867*100, 34867*100+01, 34867*100+02, 34867*100+03, 34867*100+04, 34867*100+05, 34867*100+06, 34867*100+07, 34867*100+08, 34867*100+09, 34867*100+10) AND `source_type` = 9;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (34867*100+11, 34867*100+12, 34867*100+13, 34867*100+14, 34867*100+15, 34867*100+16, 34867*100+17, 34867*100+18, 34867*100+19) AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(-219547,0,0,0,25,0,100,0,0,0,0,0,53,1,348670,1,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Reset - Start Waypoint"),
(-219547,0,1,2,40,0,100,0,7,348670,0,0,54,2700,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 7 Reached - Pause Waypoint (2700 ms)"),
(-219547,0,2,0,61,0,100,0,0,0,0,0,80,34867*100,2,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 7 Reached (Link) - Start Script"),
(-219547,0,3,4,40,0,100,0,10,348670,0,0,54,2700,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 10 Reached - Pause Waypoint (2700 ms)"),
(-219547,0,4,0,61,0,100,0,0,0,0,0,80,34867*100+01,2,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 10 Reached (Link) - Start Script"),
(-219547,0,5,6,40,0,100,0,13,348670,0,0,54,2700,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 13 Reached - Pause Waypoint (2700 ms)"),
(-219547,0,6,0,61,0,100,0,0,0,0,0,80,34867*100+02,2,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 13 Reached (Link) - Start Script"),
(-219547,0,7,8,40,0,100,0,34,348670,0,0,54,2700,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 34 Reached - Pause Waypoint (2700 ms)"),
(-219547,0,8,0,61,0,100,0,0,0,0,0,80,34867*100+03,2,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 34 Reached (Link) - Start Script"),

(-219548,0,0,0,25,0,100,0,0,0,0,0,53,1,348671,1,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Reset - Start Waypoint"),
(-219548,0,1,2,40,0,100,0,1,348671,0,0,54,2700,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 1 Reached - Pause Waypoint (2700 ms)"),
(-219548,0,2,0,61,0,100,0,0,0,0,0,80,34867*100+04,2,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 1 Reached (Link) - Start Script"),
(-219548,0,3,4,40,0,100,0,3,348671,0,0,54,2700,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 3 Reached - Pause Waypoint (2700 ms)"),
(-219548,0,4,0,61,0,100,0,0,0,0,0,80,34867*100+05,2,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 3 Reached (Link) - Start Script"),

(-219549,0,0,0,25,0,100,0,0,0,0,0,53,1,348672,1,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Reset - Start Waypoint"),
(-219549,0,1,2,40,0,100,0,2,348672,0,0,54,2700,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 2 Reached - Pause Waypoint (2700 ms)"),
(-219549,0,2,0,61,0,100,0,0,0,0,0,80,34867*100+06,2,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 2 Reached (Link) - Start Script"),
(-219549,0,3,4,40,0,100,0,8,348672,0,0,54,2700,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 8 Reached - Pause Waypoint (2700 ms)"),
(-219549,0,4,0,61,0,100,0,0,0,0,0,80,34867*100+07,2,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 8 Reached (Link) - Start Script"),

(-219551,0,0,0,25,0,100,0,0,0,0,0,53,0,348673,1,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Reset - Start Waypoint"),
(-219551,0,1,2,40,0,100,0,1,348673,0,0,54,2700,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 1 Reached - Pause Waypoint (2700 ms)"),
(-219551,0,2,0,61,0,100,0,0,0,0,0,80,34867*100+08,2,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 1 Reached (Link) - Start Script"),
(-219551,0,3,4,40,0,100,0,2,348673,0,0,54,6000,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 2 Reached - Pause Waypoint (6000 ms)"),
(-219551,0,4,0,61,0,100,0,0,0,0,0,80,34867*100+09,2,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 2 Reached (Link) - Start Script"),
(-219551,0,5,6,40,0,100,0,3,348673,0,0,54,2700,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 3 Reached - Pause Waypoint (2700 ms)"),
(-219551,0,6,0,61,0,100,0,0,0,0,0,80,34867*100+10,2,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 3 Reached (Link) - Start Script"),
(-219551,0,7,8,40,0,100,0,4,348673,0,0,54,6000,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 4 Reached - Pause Waypoint (6000 ms)"),
(-219551,0,8,0,61,0,100,0,0,0,0,0,80,34867*100+11,2,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 4 Reached (Link) - Start Script"),

(-219553,0,0,0,25,0,100,0,0,0,0,0,53,0,348674,1,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Reset - Start Waypoint"),
(-219553,0,1,2,40,0,100,0,2,348674,0,0,54,2700,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 2 Reached - Pause Waypoint (2700 ms)"),
(-219553,0,2,0,61,0,100,0,0,0,0,0,80,34867*100+12,2,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 2 Reached (Link) - Start Script"),
(-219553,0,3,4,40,0,100,0,3,348674,0,0,54,14000,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 3 Reached - Pause Waypoint (14000 ms)"),
(-219553,0,4,0,61,0,100,0,0,0,0,0,80,34867*100+13,2,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 3 Reached (Link) - Start Script"),
(-219553,0,5,6,40,0,100,0,4,348674,0,0,54,2700,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 4 Reached - Pause Waypoint (2700 ms)"),
(-219553,0,6,0,61,0,100,0,0,0,0,0,80,34867*100+14,2,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 4 Reached (Link) - Start Script"),
(-219553,0,7,8,40,0,100,0,11,348674,0,0,54,14000,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 11 Reached - Pause Waypoint (14000 ms)"),
(-219553,0,8,0,61,0,100,0,0,0,0,0,80,34867*100+15,2,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 11 Reached (Link) - Start Script"),
(-219553,0,9,10,40,0,100,0,12,348674,0,0,54,2700,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 12 Reached - Pause Waypoint (2700 ms)"),
(-219553,0,10,0,61,0,100,0,0,0,0,0,80,34867*100+16,2,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 12 Reached (Link) - Start Script"),
(-219553,0,11,12,40,0,100,0,14,348674,0,0,54,14000,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 14 Reached - Pause Waypoint (14000 ms)"),
(-219553,0,12,0,61,0,100,0,0,0,0,0,80,34867*100+17,2,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 14 Reached (Link) - Start Script"),
(-219553,0,13,14,40,0,100,0,15,348674,0,0,54,2700,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 15 Reached - Pause Waypoint (2700 ms)"),
(-219553,0,14,0,61,0,100,0,0,0,0,0,80,34867*100+18,2,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 15 Reached (Link) - Start Script"),
(-219553,0,15,16,40,0,100,0,16,348674,0,0,54,2700,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 16 Reached - Pause Waypoint (2700 ms)"),
(-219553,0,16,0,61,0,100,0,0,0,0,0,80,34867*100+19,2,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Waypoint 16 Reached (Link) - Start Script"),

(34867*100,9,0,0,0,0,100,0,0,0,0,0,48,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active On"),
(34867*100,9,1,0,0,0,100,0,0,0,0,0,97,20,20,0,0,0,0,1,0,0,0,-1387.127,1452.005,72.38,0,"Shadowy Figure - On Script - Jump To Position"),
(34867*100,9,2,0,0,0,100,0,2500,2500,0,0,48,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active Off"),

(34867*100+01,9,0,0,0,0,100,0,0,0,0,0,48,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active On"),
(34867*100+01,9,1,0,0,0,100,0,0,0,0,0,97,20,20,0,0,0,0,1,0,0,0,-1409.852,1451.495,72.74,0,"Shadowy Figure - On Script - Jump To Position"),
(34867*100+01,9,2,0,0,0,100,0,2500,2500,0,0,48,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active Off"),

(34867*100+02,9,0,0,0,0,100,0,0,0,0,0,48,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active On"),
(34867*100+02,9,1,0,0,0,100,0,0,0,0,0,97,20,10,0,0,0,0,1,0,0,0,-1483.831,1451.561,72.65,0,"Shadowy Figure - On Script - Jump To Position"),
(34867*100+02,9,2,0,0,0,100,0,2500,2500,0,0,48,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active Off"),

(34867*100+03,9,0,0,0,0,100,0,0,0,0,0,48,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active On"),
(34867*100+03,9,1,0,0,0,100,0,0,0,0,0,97,20,10,0,0,0,0,1,0,0,0,-1473.696,1451.069,72.56,0,"Shadowy Figure - On Script - Jump To Position"),
(34867*100+03,9,2,0,0,0,100,0,2500,2500,0,0,48,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active Off"),

(34867*100+04,9,0,0,0,0,100,0,0,0,0,0,48,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active On"),
(34867*100+04,9,1,0,0,0,100,0,0,0,0,0,97,20,10,0,0,0,0,1,0,0,0,-1497.173,1370.632,73.18,0,"Shadowy Figure - On Script - Jump To Position"),
(34867*100+04,9,2,0,0,0,100,0,2500,2500,0,0,48,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active Off"),

(34867*100+05,9,0,0,0,0,100,0,0,0,0,0,48,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active On"),
(34867*100+05,9,1,0,0,0,100,0,0,0,0,0,97,20,10,0,0,0,0,1,0,0,0,-1509.436,1370.704,73.279,0,"Shadowy Figure - On Script - Jump To Position"),
(34867*100+05,9,2,0,0,0,100,0,2500,2500,0,0,48,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active Off"),

(34867*100+06,9,0,0,0,0,100,0,0,0,0,0,48,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active On"),
(34867*100+06,9,1,0,0,0,100,0,0,0,0,0,97,20,10,0,0,0,0,1,0,0,0,-1494.099,1325.924,71.427,0,"Shadowy Figure - On Script - Jump To Position"),
(34867*100+06,9,2,0,0,0,100,0,2500,2500,0,0,48,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active Off"),

(34867*100+07,9,0,0,0,0,100,0,0,0,0,0,48,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active On"),
(34867*100+07,9,1,0,0,0,100,0,0,0,0,0,97,20,10,0,0,0,0,1,0,0,0,-1502.94,1322.25,73.7,0,"Shadowy Figure - On Script - Jump To Position"),
(34867*100+07,9,2,0,0,0,100,0,2500,2500,0,0,48,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active Off"),

(34867*100+08,9,0,0,0,0,100,0,0,0,0,0,48,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active On"),
(34867*100+08,9,1,0,0,0,100,0,0,0,0,0,97,20,15,0,0,0,0,1,0,0,0,-1421.19,1409.8,63.4,0,"Shadowy Figure - On Script - Jump To Position"),
(34867*100+08,9,2,0,0,0,100,0,2500,2500,0,0,48,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active Off"),

(34867*100+09,9,0,0,0,0,100,0,1000,1000,0,0,66,0,0,0,0,0,0,8,0,0,0,0,0,0,1.559,"Shadowy Figure - On Script - Set Orientation (1.559)"),

(34867*100+10,9,0,0,0,0,100,0,0,0,0,0,48,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active On"),
(34867*100+10,9,1,0,0,0,100,0,0,0,0,0,97,20,15,0,0,0,0,1,0,0,0,-1421.94,1401.76,71.81,0,"Shadowy Figure - On Script - Jump To Position"),
(34867*100+10,9,2,0,0,0,100,0,2500,2500,0,0,48,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active Off"),

(34867*100+11,9,0,0,0,0,100,0,1000,1000,0,0,66,0,0,0,0,0,0,8,0,0,0,0,0,0,4.783,"Shadowy Figure - On Script - Set Orientation (4.783)"),

(34867*100+12,9,0,0,0,0,100,0,0,0,0,0,48,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active On"),
(34867*100+12,9,1,0,0,0,100,0,0,0,0,0,97,20,10,0,0,0,0,1,0,0,0,-1557.18,1424.72,67.07,0,"Shadowy Figure - On Script - Jump To Position"),
(34867*100+12,9,2,0,0,0,100,0,2500,2500,0,0,48,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active Off"),

(34867*100+13,9,0,0,0,0,100,0,1000,1000,0,0,66,0,0,0,0,0,0,8,0,0,0,0,0,0,0.0431,"Shadowy Figure - On Script - Set Orientation (0.0431)"),

(34867*100+14,9,0,0,0,0,100,0,0,0,0,0,48,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active On"),
(34867*100+14,9,1,0,0,0,100,0,0,0,0,0,97,20,10,0,0,0,0,1,0,0,0,-1562.2,1424.838,71.068,0,"Shadowy Figure - On Script - Jump To Position"),
(34867*100+14,9,2,0,0,0,100,0,2500,2500,0,0,48,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active Off"),

(34867*100+15,9,0,0,0,0,100,0,1000,1000,0,0,66,0,0,0,0,0,0,8,0,0,0,0,0,0,3.1415,"Shadowy Figure - On Script - Set Orientation (3.1415)"),

(34867*100+16,9,0,0,0,0,100,0,0,0,0,0,48,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active On"),
(34867*100+16,9,1,0,0,0,100,0,0,0,0,0,97,20,10,0,0,0,0,1,0,0,0,-1546.94,1370.72,64.519,0,"Shadowy Figure - On Script - Jump To Position"),
(34867*100+16,9,2,0,0,0,100,0,2500,2500,0,0,48,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active Off"),

(34867*100+17,9,0,0,0,0,100,0,1000,1000,0,0,66,0,0,0,0,0,0,8,0,0,0,0,0,0,0.4868,"Shadowy Figure - On Script - Set Orientation (0.4868)"),

(34867*100+18,9,0,0,0,0,100,0,0,0,0,0,48,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active On"),
(34867*100+18,9,1,0,0,0,100,0,0,0,0,0,97,20,15,0,0,0,0,1,0,0,0,-1536.37,1370.77,62.998,0,"Shadowy Figure - On Script - Jump To Position"),
(34867*100+18,9,2,0,0,0,100,0,2500,2500,0,0,48,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active Off"),

(34867*100+19,9,0,0,0,0,100,0,0,0,0,0,48,1,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active On"),
(34867*100+19,9,1,0,0,0,100,0,0,0,0,0,97,20,15,0,0,0,0,1,0,0,0,-1559.48,1377.65,73.239,0,"Shadowy Figure - On Script - Jump To Position"),
(34867*100+19,9,2,0,0,0,100,0,2500,2500,0,0,48,0,0,0,0,0,0,1,0,0,0,0,0,0,0,"Shadowy Figure - On Script - Set Active Off");

DELETE FROM `disables` WHERE `entry` = 34867 AND `sourceType` = 10;
INSERT INTO `disables` (`sourceType`, `entry`, `flags`, `params_0`, `params_1`, `comment`) VALUES
(10, 34867, 1, "", "", "Shadowy Figure - MMAP");
