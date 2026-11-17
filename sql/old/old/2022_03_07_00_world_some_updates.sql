UPDATE `creature_template` SET `modelid1` = 32352 WHERE `entry` = 71989;
UPDATE `creature_template` SET `InhabitType` = 4 WHERE `entry` IN (29777, 29780, 29781, 29782, 29783, 29784, 29785, 29802);
UPDATE `creature` SET `wander_distance` = 0, `movement_type` = 0 WHERE `guid` IN (87282, 87281);
