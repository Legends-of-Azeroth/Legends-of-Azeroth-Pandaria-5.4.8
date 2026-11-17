UPDATE `creature_template` SET `InhabitType` = 4 WHERE `entry` IN (1513, 1553);

DELETE FROM `creature_addon` WHERE `guid` IN (193272, 194133, 194649, 193748, 194563, 192742, 192695, 192696, 192697, 194454, 194052, 193474,
                                              193213, 193829, 194130, 193679, 193680, 194434, 194135, 193652, 193899, 193458);
DELETE FROM `waypoint_data` WHERE `id` IN (193272, 194133, 194649, 193748, 194563, 192742, 192695, 192696, 192697, 194454, 194052, 193474,
                                           193213, 193829, 194130, 193679, 193680, 194434, 194135, 193652, 193899, 193458);
UPDATE `creature` SET `movement_type` = 1, `wander_distance` = 20 WHERE `id` IN (1513, 1553);
UPDATE `creature` SET `movement_type` = 1, `wander_distance` = 10 WHERE `id` IN (1547, 1548, 1890);
UPDATE `creature` SET `movement_type` = 1, `wander_distance` = 5 WHERE `id` IN (1508, 1509);
