UPDATE `creature_template` SET `ScriptName` = "npc_gilneas_city_guard_gate" WHERE `entry` = 34864;
UPDATE `creature_template` SET `ScriptName` = "npc_gilneas_crow", `InhabitType` = 4 WHERE `entry` = 50260;

DELETE FROM `creature_text` WHERE `entry` IN (34864, 44086);
INSERT INTO `creature_text` (`entry`, `text_group`, `id`, `text`, `text_female`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(34864, 0, 0, "No one's allowed to leave. Prince Liam's orders.", "", 12, 0, 100, 1, 0, 0, "Gilneas City Guard"),
(34864, 0, 1, "It isn't safe to leave, there's something lurking out there in the woods.", "", 12, 0, 100, 1, 0, 0, "Gilneas City Guard"),
(34864, 0, 2, "Stay calm. We have everything under control.", "", 12, 0, 100, 25, 0, 0, "Gilneas City Guard"),

(44086, 0, 0, "You can't do this to us!", "You can't do this to us!", 12, 0, 33, 5, 0, 0, "Panicked Citizen"),
(44086, 0, 1, "My children are in Stormglen alone! I have to get to them!", "My children are in Stormglen alone! I have to get to them!", 12, 0, 33, 5, 0, 0, "Panicked Citizen"),
(44086, 0, 2, "What's going on? I thought I saw something on the rooftops.", "What's going on? I thought I saw something on the rooftops.", 12, 0, 33, 6, 0, 0, "Panicked Citizen"),
(44086, 0, 3, "We have to get out of here! The howling gets louder every night.", "We have to get out of here! The howling gets louder every night.", 12, 0, 33, 5, 0, 0, "Panicked Citizen"),
(44086, 0, 4, "They told us Gilneas City was safe...", "They told us Gilneas City was safe...", 12, 0, 33, 1, 0, 0, "Panicked Citizen");

DELETE FROM `creature` WHERE `guid` IN (219638, 219632, 219630, 219629);
DELETE FROM `creature_addon` WHERE `guid` IN (219638, 219632, 219630, 219629);
DELETE FROM `waypoint_data` WHERE `id` IN (219638, 219632, 219630, 219629);

UPDATE `creature` SET `position_x` = -1532.569702, `position_y` = 1324.009521, `position_z` = 35.861324, `orientation` = 0.838399, `wander_distance` = 0, `movement_type` = 0 WHERE `guid` = 219640 AND `id` = 34851;
UPDATE `creature` SET `position_x` = -1482.562988, `position_y` = 1337.003784, `position_z` = 35.871769, `orientation` = 3.595148, `wander_distance` = 0, `movement_type` = 0 WHERE `guid` = 219639 AND `id` = 34851;
UPDATE `creature` SET `position_x` = -1538.670299, `position_y` = 1381.714233, `position_z` = 36.004162, `orientation` = 6.104491, `wander_distance` = 0, `movement_type` = 0 WHERE `guid` = 219633 AND `id` = 34851;
UPDATE `creature` SET `position_x` = -1559.391602, `position_y` = 1409.633057, `position_z` = 35.955856, `orientation` = 0.015523, `wander_distance` = 0, `movement_type` = 0 WHERE `guid` = 221448 AND `id` = 34851;
UPDATE `creature` SET `position_x` = -1397.222900, `position_y` = 1402.700439, `position_z` = 35.960724, `orientation` = 1.985302, `wander_distance` = 0, `movement_type` = 0 WHERE `guid` = 221447 AND `id` = 34851;
UPDATE `creature` SET `position_x` = -1396.093384, `position_y` = 1361.986572, `position_z` = 35.556969, `orientation` = 2.679593, `wander_distance` = 0, `movement_type` = 0 WHERE `guid` = 221444 AND `id` = 34851;
UPDATE `creature` SET `position_x` = -1581.99939, `position_y` = 1361.849854, `position_z` = 35.99939, `orientation` = 2.079495, `wander_distance` = 0, `movement_type` = 0 WHERE `guid` = 221441 AND `id` = 34851;
UPDATE `creature` SET `position_x` = -1510.293457, `position_y` = 1371.880249, `position_z` = 35.998672, `orientation` = 4.001499, `wander_distance` = 0, `movement_type` = 0 WHERE `guid` = 221440 AND `id` = 34851;
UPDATE `creature` SET `position_x` = -1477.174072, `position_y` = 1379.595459, `position_z` = 36.0047, `orientation` = 2.410171, `wander_distance` = 0, `movement_type` = 0 WHERE `guid` = 221439 AND `id` = 34851;
UPDATE `creature` SET `position_x` = -1524.065918, `position_y` = 1406.494263, `position_z` = 35.555931, `orientation` = 5.669565, `wander_distance` = 0, `movement_type` = 0 WHERE `guid` = 221438 AND `id` = 34851;
UPDATE `creature` SET `position_x` = -1539.934204, `position_y` = 1440.324463, `position_z` = 35.976925, `orientation` = 3.941693, `wander_distance` = 0, `movement_type` = 0 WHERE `guid` = 221437 AND `id` = 34851;
UPDATE `creature` SET `position_x` = -1515.657471, `position_y` = 1412.916016, `position_z` = 35.556549, `orientation` = 0.339493, `wander_distance` = 0, `movement_type` = 0 WHERE `guid` = 221432 AND `id` = 34851;
UPDATE `creature` SET `position_x` = -1476.542114, `position_y` = 1439.608276, `position_z` = 35.961662, `orientation` = 4.066211, `wander_distance` = 0, `movement_type` = 0 WHERE `guid` = 220188 AND `id` = 34851;
UPDATE `creature` SET `position_x` = -1429.768921, `position_y` = 1439.704102, `position_z` = 35.968613, `orientation` = 2.530758, `wander_distance` = 0, `movement_type` = 0 WHERE `guid` = 220185 AND `id` = 34851;
UPDATE `creature` SET `position_x` = -1449.906982, `position_y` = 1369.512817, `position_z` = 35.603382, `orientation` = 5.28279, `wander_distance` = 0, `movement_type` = 0 WHERE `guid` = 220183 AND `id` = 34851;
UPDATE `creature` SET `position_x` = -1471.026123, `position_y` = 1412.337646, `position_z` = 35.556248, `orientation` = 2.424378, `wander_distance` = 0, `movement_type` = 0 WHERE `guid` = 220006 AND `id` = 34851;
UPDATE `creature` SET `position_x` = -1472.311523, `position_y` = 1407.288452, `position_z` = 35.566099, `orientation` = 3.574259, `wander_distance` = 0, `movement_type` = 0 WHERE `guid` = 220005 AND `id` = 34851;

UPDATE `creature` SET `position_x` = -1502.160034, `position_y` = 1372.489990, `position_z` = 35.556000, `orientation` = 1.561972, `spawntimesecs` = 30, `wander_distance` = 0, `movement_type` = 2 WHERE `guid` = 219989 AND `id` = 34851;
UPDATE `creature` SET `position_x` = -1421.198730, `position_y` = 1414.781616, `position_z` = 36.004940, `orientation` = 1.629003, `spawntimesecs` = 30, `wander_distance` = 0, `movement_type` = 2 WHERE `guid` = 219986 AND `id` = 34851;
UPDATE `creature` SET `position_x` = -1529.618408, `position_y` = 1325.179199, `position_z` = 35.847145, `orientation` = 1.748103, `spawntimesecs` = 30, `wander_distance` = 0, `movement_type` = 2 WHERE `guid` = 219985 AND `id` = 34851;
UPDATE `creature` SET `wander_distance` = 0, `movement_type` = 2 WHERE `guid` IN (221442, 221445) AND `id` = 34851;

DELETE FROM `creature_template_addon` WHERE `entry` = 34851;
DELETE FROM `creature_addon` WHERE `guid` IN (219989, 219986, 219985, 221442, 221445, 221448, 221447, 221444, 221441, 221440, 221439, 221438);
DELETE FROM `creature_addon` WHERE `guid` IN (221437, 221432, 220188, 220185, 220183, 220006, 219982, 219589, 219640, 219639, 219633, 220005);
INSERT INTO `creature_addon` (`guid`, `path_id`, `bytes1`, `bytes2`, `emote`, `auras`) VALUES
(219989, 219989, 0, 1, 0, "52385"),
(219986, 219986, 0, 1, 0, "52385"),
(219985, 219985, 0, 1, 0, "52385"),
(221442, 221442, 0, 1, 0, "52385"),
(221445, 221445, 0, 1, 0, "52385"),

(221448, 0, 0, 1, 431, ""),
(221447, 0, 0, 1, 431, ""),
(221444, 0, 0, 1, 431, ""),
(221441, 0, 0, 1, 431, ""),
(221440, 0, 0, 1, 431, ""),
(221439, 0, 0, 1, 431, ""),
(221438, 0, 0, 1, 431, ""),
(221437, 0, 0, 1, 431, ""),
(221432, 0, 0, 1, 431, ""),
(220188, 0, 0, 1, 431, ""),
(220185, 0, 0, 1, 431, ""),
(220183, 0, 0, 1, 431, ""),
(220006, 0, 0, 1, 431, ""),
(219982, 0, 0, 1, 431, ""),
(219589, 0, 0, 1, 431, ""),
(219640, 0, 0, 1, 431, ""),
(219639, 0, 0, 1, 431, ""),
(219633, 0, 0, 1, 431, ""),
(220005, 0, 0, 1, 431, "");

UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` = 34851;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (34851, -219989, -219986, -219985, -221448, -221447, -221444, -221441, -221440, -221439, -221438) AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (-221437, -221432, -220188, -220185, -220183, -220006, -219982, -219589, -219640, -219639, -219633, -220005) AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(-219989, 0, 0, 0, 34, 0, 100, 0, 2, 10, 0, 0, 41, 2000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Reach Point 10 - Despawn (2000 ms)"),
(-219989, 0, 1, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Aggro - Say Text Line 1"),

(-219986, 0, 0, 0, 34, 0, 100, 0, 2, 6, 0, 0, 41, 2000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Reach Point 6 - Despawn (2000 ms)"),
(-219986, 0, 1, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Aggro - Say Text Line 1"),

(-219985, 0, 0, 0, 34, 0, 100, 0, 2, 10, 0, 0, 41, 2000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Reach Point 10 - Despawn (2000 ms)"),
(-219985, 0, 1, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Aggro - Say Text Line 1"),

(-221448, 0, 0, 0, 1, 0, 60, 0, 10000, 120000, 60000, 240000, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Update - Say Text Line 0"),
(-221448, 0, 1, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Aggro - Say Text Line 1"),

(-221447, 0, 0, 0, 1, 0, 60, 0, 10000, 120000, 60000, 240000, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Update - Say Text Line 0"),
(-221447, 0, 1, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Aggro - Say Text Line 1"),

(-221444, 0, 0, 0, 1, 0, 60, 0, 10000, 120000, 60000, 240000, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Update - Say Text Line 0"),
(-221444, 0, 1, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Aggro - Say Text Line 1"),

(-221441, 0, 0, 0, 1, 0, 60, 0, 10000, 120000, 60000, 240000, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Update - Say Text Line 0"),
(-221441, 0, 1, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Aggro - Say Text Line 1"),

(-221440, 0, 0, 0, 1, 0, 60, 0, 10000, 120000, 60000, 240000, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Update - Say Text Line 0"),
(-221440, 0, 1, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Aggro - Say Text Line 1"),

(-221439, 0, 0, 0, 1, 0, 60, 0, 10000, 120000, 60000, 240000, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Update - Say Text Line 0"),
(-221439, 0, 1, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Aggro - Say Text Line 1"),

(-221438, 0, 0, 0, 1, 0, 60, 0, 10000, 120000, 60000, 240000, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Update - Say Text Line 0"),
(-221438, 0, 1, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Aggro - Say Text Line 1"),

(-221437, 0, 0, 0, 1, 0, 60, 0, 10000, 120000, 60000, 240000, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Update - Say Text Line 0"),
(-221437, 0, 1, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Aggro - Say Text Line 1"),

(-221432, 0, 0, 0, 1, 0, 60, 0, 10000, 120000, 60000, 240000, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Update - Say Text Line 0"),
(-221432, 0, 1, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Aggro - Say Text Line 1"),

(-220188, 0, 0, 0, 1, 0, 60, 0, 10000, 120000, 60000, 240000, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Update - Say Text Line 0"),
(-220188, 0, 1, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Aggro - Say Text Line 1"),

(-220185, 0, 0, 0, 1, 0, 60, 0, 10000, 120000, 60000, 240000, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Update - Say Text Line 0"),
(-220185, 0, 1, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Aggro - Say Text Line 1"),

(-220183, 0, 0, 0, 1, 0, 60, 0, 10000, 120000, 60000, 240000, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Update - Say Text Line 0"),
(-220183, 0, 1, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Aggro - Say Text Line 1"),

(-220006, 0, 0, 0, 1, 0, 60, 0, 10000, 120000, 60000, 240000, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Update - Say Text Line 0"),
(-220006, 0, 1, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Aggro - Say Text Line 1"),

(-219982, 0, 0, 0, 1, 0, 60, 0, 10000, 120000, 60000, 240000, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Update - Say Text Line 0"),
(-219982, 0, 1, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Aggro - Say Text Line 1"),

(-219589, 0, 0, 0, 1, 0, 60, 0, 10000, 120000, 60000, 240000, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Update - Say Text Line 0"),
(-219589, 0, 1, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Aggro - Say Text Line 1"),

(-219640, 0, 0, 0, 1, 0, 60, 0, 10000, 120000, 60000, 240000, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Update - Say Text Line 0"),
(-219640, 0, 1, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Aggro - Say Text Line 1"),

(-219639, 0, 0, 0, 1, 0, 60, 0, 10000, 120000, 60000, 240000, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Update - Say Text Line 0"),
(-219639, 0, 1, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Aggro - Say Text Line 1"),

(-219633, 0, 0, 0, 1, 0, 60, 0, 10000, 120000, 60000, 240000, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Update - Say Text Line 0"),
(-219633, 0, 1, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Aggro - Say Text Line 1"),

(-220005, 0, 0, 0, 1, 0, 60, 0, 10000, 120000, 60000, 240000, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Update - Say Text Line 0"),
(-220005, 0, 1, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Panicked Citizen - On Aggro - Say Text Line 1");

DELETE FROM `waypoint_data` WHERE `id` IN (219989, 219986, 219985, 221442, 221445);
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_flag`, `entry`) VALUES
(219989, 1, -1502.16, 1372.49, 35.556, 1.508, 0, 1, 34851),
(219989, 2, -1502.47, 1391.86, 35.556, 1.586, 0, 1, 34851),
(219989, 3, -1508.6, 1395.27, 35.556, 2.635, 0, 1, 34851),
(219989, 4, -1548.69, 1396.63, 35.556, 3.114, 0, 1, 34851),
(219989, 5, -1552.45, 1403.35, 35.556, 2.081, 0, 1, 34851),
(219989, 6, -1552.08, 1420.15, 35.556, 1.559, 0, 1, 34851),
(219989, 7, -1545.65, 1426.27, 35.556, 0.754, 0, 1, 34851),
(219989, 8, -1444.34, 1425.76, 35.556, 6.244, 0, 1, 34851),
(219989, 9, -1432.26, 1420.81, 35.556, 5.894, 0, 1, 34851),
(219989, 10, -1421.06, 1420.63, 35.556, 6.267, 0, 1, 34851),
(219989, 11, -1420.76, 1413.39, 36.519, 4.775, 4000, 1, 34851),

(219986, 1, -1441.17, 1427.62, 35.556, 3.228, 0, 1, 34851),
(219986, 2, -1544.72, 1426.79, 35.556, 3.142, 0, 1, 34851),
(219986, 3, -1551.93, 1422.04, 35.556, 3.754, 0, 1, 34851),
(219986, 4, -1553.96, 1369.56, 35.628, 4.701, 0, 1, 34851),
(219986, 5, -1556.84, 1365.27, 35.623, 4.049, 0, 1, 34851),
(219986, 6, -1583.88, 1364.09, 35.699, 3.22, 0, 1, 34851),
(219986, 7, -1583.53, 1359.91, 36.168, 4.795, 4000, 1, 34851),

(219985, 1, -1528.82, 1334.27, 35.557, 0.357, 0, 1, 34851),
(219985, 2, -1505.82, 1341.25, 35.557, 0.306, 0, 1, 34851),
(219985, 3, -1502.35, 1347.33, 35.557, 1.052, 0, 1, 34851),
(219985, 4, -1504.18, 1423.11, 35.557, 1.567, 0, 1, 34851),
(219985, 5, -1510.72, 1427.18, 35.557, 2.584, 0, 1, 34851),
(219985, 6, -1549.44, 1427.19, 35.557, 3.142, 0, 1, 34851),
(219985, 7, -1553.56, 1420.96, 35.557, 4.127, 0, 1, 34851),
(219985, 8, -1554.42, 1393.65, 35.576, 4.681, 0, 1, 34851),
(219985, 9, -1560.49, 1390.59, 35.758, 3.609, 0, 1, 34851),
(219985, 10, -1568.86, 1391.21, 36.199, 3.067, 0, 1, 34851),
(219985, 11, -1569.41, 1385.65, 36.578, 4.642, 4000, 1, 34851),

(221442, 1, -1557.69, 1332.67, 35.5567, 0, 0, 1, 34851),
(221442, 2, -1511.86, 1343.79, 35.5567, 0, 0, 1, 34851),
(221442, 3, -1502.91, 1353.88, 35.5567, 0, 0, 1, 34851),
(221442, 4, -1503.16, 1382.47, 35.5567, 0, 0, 1, 34851),
(221442, 5, -1497.24, 1393.37, 35.5567, 0, 0, 1, 34851),
(221442, 6, -1438.17, 1394.41, 35.5567, 0, 0, 1, 34851),
(221442, 7, -1435.39, 1419.84, 35.5567, 0, 0, 1, 34851),
(221442, 8, -1405.91, 1418.12, 35.5567, 0, 0, 1, 34851),
(221442, 9, -1402.8, 1390.08, 35.5567, 0, 0, 1, 34851),
(221442, 10, -1424.8, 1365.57, 35.5567, 0, 0, 1, 34851),
(221442, 11, -1517.5, 1337.497, 35.5567, 0, 0, 1, 34851),
(221442, 12, -1566.1, 1320.586, 35.5567, 0, 0, 1, 34851),

(221445, 1, -1553.72, 1391.36, 35.5714, 0, 0, 1, 34851),
(221445, 2, -1540.54, 1393.87, 35.5563, 0, 0, 1, 34851),
(221445, 3, -1522.59, 1394.47, 35.5563, 0, 0, 1, 34851),
(221445, 4, -1507.78, 1394.14, 35.5563, 0, 0, 1, 34851),
(221445, 5, -1501.33, 1401.19, 35.5563, 0, 0, 1, 34851),
(221445, 6, -1500.24, 1413.15, 35.5563, 0, 0, 1, 34851),
(221445, 7, -1495.03, 1424.15, 35.5563, 0, 0, 1, 34851),
(221445, 8, -1485.34, 1426.21, 35.5563, 0, 0, 1, 34851),
(221445, 9, -1457.24, 1426.59, 35.5563, 0, 0, 1, 34851),
(221445, 10, -1443.77, 1424.88, 35.5563, 0, 0, 1, 34851),
(221445, 11, -1437.21, 1418.57, 35.5563, 0, 0, 1, 34851),
(221445, 12, -1437.1, 1397.73, 35.5563, 0, 0, 1, 34851),
(221445, 13, -1442.47, 1373, 35.5563, 0, 0, 1, 34851),
(221445, 14, -1449.17, 1363.34, 35.5563, 0, 0, 1, 34851),
(221445, 15, -1474.46, 1350.9, 35.5563, 0, 0, 1, 34851),
(221445, 16, -1485.1, 1353.39, 35.5671, 0, 0, 1, 34851),
(221445, 17, -1498.15, 1349.15, 35.5563, 0, 0, 1, 34851),
(221445, 18, -1502.75, 1355.02, 35.5563, 0, 0, 1, 34851),
(221445, 19, -1503.81, 1385.74, 35.5563, 0, 0, 1, 34851),
(221445, 20, -1504.02, 1406.74, 35.5563, 0, 0, 1, 34851),
(221445, 21, -1509.41, 1418.46, 35.5563, 0, 0, 1, 34851),
(221445, 22, -1521.87, 1426.38, 35.5563, 0, 0, 1, 34851),
(221445, 23, -1542.9, 1426.84, 35.5563, 0, 0, 1, 34851),
(221445, 24, -1552.11, 1418.9, 35.5563, 0, 0, 1, 34851),
(221445, 25, -1553.31, 1390.12, 35.5692, 0, 0, 1, 34851),
(221445, 26, -1554.03, 1365.39, 35.5678, 0, 0, 1, 34851),
(221445, 27, -1554.34, 1343.59, 35.5567, 0, 0, 1, 34851),
(221445, 28, -1542.8, 1332.8, 35.5567, 0, 0, 1, 34851),
(221445, 29, -1513.19, 1337.98, 35.5567, 0, 0, 1, 34851),
(221445, 30, -1445.78, 1358.5, 35.5567, 0, 0, 1, 34851),
(221445, 31, -1415.59, 1365.53, 35.5567, 0, 0, 1, 34851),
(221445, 32, -1404.28, 1377.33, 35.5567, 0, 0, 1, 34851),
(221445, 33, -1403.22, 1405.88, 35.5567, 0, 0, 1, 34851),
(221445, 34, -1407.56, 1418.01, 35.5567, 0, 0, 1, 34851),
(221445, 35, -1436.94, 1421.79, 35.5567, 0, 0, 1, 34851),
(221445, 36, -1471.4, 1424.21, 35.5567, 0, 0, 1, 34851),
(221445, 37, -1511.17, 1424.79, 35.5567, 0, 0, 1, 34851),
(221445, 38, -1542.08, 1424.6, 35.5567, 0, 0, 1, 34851),
(221445, 39, -1550.87, 1418.47, 35.5567, 0, 0, 1, 34851),
(221445, 40, -1554.48, 1394.58, 35.5781, 0, 0, 1, 34851),
(221445, 41, -1572.96, 1393.06, 36.4174, 0, 0, 1, 34851),
(221445, 42, -1567.68, 1390.9, 36.1181, 0, 0, 1, 34851);

DELETE FROM `creature_text` WHERE `entry` = 34851;
INSERT INTO `creature_text` (`entry`, `text_group`, `id`, `text`, `text_female`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES
(34851, 0, 0, "What... what are those things on the rooftops?", "What... what are those things on the rooftops?", 12, 0, 50, 6, 0, 0, "Panicked Citizen"),
(34851, 0, 1, "Somebody save me!", "Somebody save me!", 12, 0, 50, 22, 0, 0, "Panicked Citizen"),
(34851, 0, 2, "Spare my life! I will leave this place forever! Please!", "Spare my life! I will leave this place forever! Please!", 12, 0, 50, 22, 0, 0, "Panicked Citizen"),
(34851, 0, 3, "You make my children orphans on this day, friend!", "You make my children orphans on this day, friend!", 12, 0, 50, 22, 0, 0, "Panicked Citizen"),
(34851, 0, 4, "The... They promised that we'd be safe...", "The... They promised that we'd be safe...", 12, 0, 50, 1, 0, 0, "Panicked Citizen"),
(34851, 0, 5, "I'm too young to die!", "I'm too young to die!", 12, 0, 50, 22, 0, 0, "Panicked Citizen"),
(34851, 0, 6, "Ask for anything and you shall have it! Just spare my life!", "Ask for anything and you shall have it! Just spare my life!", 12, 0, 50, 22, 0, 0, "Panicked Citizen"),
(34851, 0, 7, "HELP! HELP!", "HELP! HELP!", 12, 0, 50, 22, 0, 0, "Panicked Citizen"),
(34851, 1, 0, "I won't go down that easy!", "I won't go down that easy!", 12, 0, 50, 0, 0, 0, "Panicked Citizen"),
(34851, 1, 1, "Die Scourge dog! DIE!", "Die Scourge dog! DIE!", 12, 0, 50, 0, 0, 0, "Panicked Citizen"),
(34851, 1, 2, "Is this the best the Scourge can do?", "Is this the best the Scourge can do?", 12, 0, 50, 0, 0, 0, "Panicked Citizen"),
(34851, 1, 3, "You may take my life, but you won't take my freedom!", "You may take my life, but you won't take my freedom!", 12, 0, 50, 0, 0, 0, "Panicked Citizen"),
(34851, 1, 4, "DIE!", "DIE!", 12, 0, 50, 0, 0, 0, "Panicked Citizen"),
(34851, 1, 5, "You're not taking me without a fight!", "You're not taking me without a fight!", 12, 0, 50, 0, 0, 0, "Panicked Citizen");
