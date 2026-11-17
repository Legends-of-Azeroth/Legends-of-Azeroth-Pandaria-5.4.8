DELETE FROM `creature_sparring_template` WHERE `creature_id` IN (34884, 34916);
INSERT INTO `creature_sparring_template`(`creature_id`, `health_limit_pct`, `comment`) VALUES
(34884,75,"Rampaging Worgen"),
(34916,75,"Gilneas City Guard");

UPDATE `creature` SET `movement_type` = 1, `wander_distance` = 35, `walk_mode` = 2 WHERE `id` = 4075 AND `map` = 654;
DELETE FROM `creature_addon` WHERE `guid` IN (220889, 219624, 219825, 219675);
DELETE FROM `waypoint_data` WHERE `id` IN (220889, 219624, 219825, 219675);

UPDATE `creature_template` SET `InhabitType` = 7 WHERE `entry` = 35383;
DELETE FROM `creature_template_addon` WHERE `entry` IN (35383, 44105);
INSERT INTO `creature_template_addon` (`entry`, `bytes1`, `bytes2`, auras) VALUES
(35383, 8, 1, ""),
(44105, 0, 1, "29266");

UPDATE `creature` SET `wander_distance` = 0, `movement_type` = 0 WHERE `guid` IN (219625, 220183, 220188, 219589, 219982, 220185, 221422, 221415,
                                                                                  221417, 220867, 221418, 221416, 221421, 221419, 221420, 219626,
                                                                                  220577, 220576, 220886, 220575, 219704, 219701, 219705, 219702,
                                                                                  220887, 220473, 219703, 221459, 221452, 221454, 221453, 221458,
                                                                                  221456, 221457, 221460, 221455, 221217, 221210, 221215, 220534,
                                                                                  221211, 220537, 221209, 221213, 220530, 220532, 220535, 221212,
                                                                                  220531, 221214, 220529, 220536, 220533, 371594, 371595);

UPDATE `creature` SET `position_z` = 41.023338 WHERE `guid` = 219626;
UPDATE `creature` SET `position_x` = -1534.185547, `position_y` = 1381.463623, `position_z` = 37.730263, `orientation` = 5.899164 WHERE `guid` = 220575;
UPDATE `creature` SET `position_z` = 36.7 WHERE `guid` = 219702;
UPDATE `creature` SET `position_z` = 36.8 WHERE `guid` = 219704;
UPDATE `creature` SET `position_z` = 36.7 WHERE `guid` = 219703;

UPDATE `creature` SET `phaseMask` = 262144 WHERE `guid` IN (222878, 222875, 222290, 222275, 222858, 222331, 222323, 222325, 222333, 222326,
                                                            222334, 222332, 222320, 222319, 222344, 222293, 222350, 222292, 222353, 222297,
                                                            222349, 222356, 222295, 222296, 222298, 222808, 222810, 222805, 222889, 222895,
                                                            222893, 222896, 222891, 222903, 222907, 222899, 222886, 222908, 222892, 222336,
                                                            222961, 222957, 222956, 222972, 222978, 222975, 222977, 222971, 222976, 222965,
                                                            222964);
UPDATE `creature` SET `phaseMask` = 4 WHERE `guid` IN (371579, 221541, 221544, 221542, 221539, 221536, 221537, 221195);
UPDATE `creature` SET `phaseMask` = 8 WHERE `guid` IN (221591, 221584, 221586, 221577, 371594, 371595);

DELETE FROM `creature` WHERE `guid` = 220012;
DELETE FROM `creature_addon` WHERE `guid` = 220012;
DELETE FROM `waypoint_data` WHERE `id` = 220012;

UPDATE `creature_template` SET `AIName` = "SmartAI", `ScriptName` = "" WHERE `entry` = 34884;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 34884 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(34884, 0, 0, 1, 2, 0, 100, 1, 0, 10, 0, 0, 11, 8599, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Rampaging Worgen - At 10% HP - Cast Spell 'Enrage'"),
(34884, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Rampaging Worgen - At 10% HP (Link) - Say Text Line 0");

UPDATE `creature` SET `movement_type` = 1, `wander_distance` = 25, `walk_mode` = 2 WHERE `id` = 34884 AND NOT guid IN (221068, 221483, 221435, 221424, 221472);
UPDATE `creature` SET `movement_type` = 1, `wander_distance` = 10 WHERE `id` = 34916;
UPDATE `creature` SET `movement_type` = 1, `wander_distance` = 3 WHERE guid = 221472;
